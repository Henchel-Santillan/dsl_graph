#ifndef DS_GRAPH_DIGRAPH_H
#define DS_GRAPH_DIGRAPH_H


#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

#include "traits.h"


namespace nonlinear::graph
{
    namespace details
    {
        static constexpr const int default_weight = 0;

        template <Comparable Tp>
        struct digraph_node
        {
            // Constructors
            constexpr digraph_node() noexcept = default;
            constexpr explicit digraph_node(const Tp &value, const int cost = default_weight) noexcept
                : m_value(value), m_cost(cost) {}

            // Copy/move assignment
            constexpr digraph_node& operator=(const digraph_node &rhs) noexcept
            {
                if (this == &rhs) return *this;
                m_value = rhs.m_value;
                m_cost = rhs.m_cost;
                return *this;
            }

            // Delegate resource destruction responsibility to the containing class
            virtual ~digraph_node() noexcept = default;

            Tp m_value;
            int m_cost;

        };  // struct digraph_node

    }   // namespace details



    // Digraph class is rooted, by default, to the first element inserted
    template <Comparable Tp>
    class digraph
    {
    public:
        using digraph_node = typename details::digraph_node;


        // Constructors
        digraph() noexcept = default;
        explicit digraph(const int capacity = default_capacity)
            : m_capacity(capacity),
              m_size(0),
              m_root(nullptr),
              m_adjList(m_capacity <= 0 ? nullptr : new digraph_node<Tp>*[m_capacity])
        {
            if (m_capacity <= 0)
                throw std::invalid_argument("Failed to initialize for capacity <= 0.");
        }

        // Copy constructor
        digraph(const digraph&);

        // Move constructor
        digraph(digraph &&rhs) noexcept
            : digraph<Tp>()
        { swap(rhs); }

        // Pass-by-value copy/move assignment
        digraph& operator=(digraph rhs) noexcept
        { swap(rhs); return *this; }

        ~digraph();
        constexpr void swap(digraph &rhs) noexcept;


        //****** Access, Traversal, and Properties ******//
        [[nodiscard]] constexpr int capacity() const noexcept   { return m_capacity; }
        [[nodiscard]] constexpr int size() const noexcept       { return m_size; }
        [[nodiscard]] constexpr bool empty() const noexcept     { return m_size == 0; }
        [[nodiscard]] constexpr bool full() const noexcept      { return m_size == m_capacity; }

        [[nodiscard]] constexpr bool contains(const Tp &value) const noexcept
        { return indexOf(value) != -1; }

        constexpr std::optional<const digraph_node&> find_bfs(const Tp&) const noexcept;
        constexpr std::optional<const digraph_node&> find_dfs(const Tp&) const noexcept;

        const digraph& transpose() const noexcept;
        constexpr bool is_strongly_connected() const noexcept;
        constexpr bool has_link(const Tp&, const Tp&) const noexcept;
        [[nodiscard]] constexpr int count_disconnected() const noexcept;
        constexpr int in_degree(const Tp&) const noexcept;
        constexpr int out_degree(const Tp&) const noexcept;


        //****** Modifiers ******//
        constexpr bool try_link(const Tp&, const Tp&) noexcept;
        constexpr bool push_vertex(const Tp&, int = default_weight) noexcept;
        constexpr bool push_edge(const Tp&, const Tp&, int = default_weight, int = default_weight) noexcept;
        constexpr bool pop_vertex(const Tp&) noexcept;
        constexpr bool pop_edge(const Tp&) noexcept;

        friend std::ostream& operator<<(std::ostream&, const digraph<Tp>&) noexcept;

    private:
        int m_capacity, m_size;
        digraph_node<Tp> *m_root;       // will point to first inserted node, else next inserted
        digraph_node<Tp> **m_adjList;


        constexpr int indexOf(const Tp&) const noexcept;
        constexpr int try_push(const Tp&, int = default_weight) noexcept;
        constexpr void recompute_root() noexcept;
        constexpr int edge_length() const noexcept;

    };  // class digraph-proto



    //************ Member Function Implementations ************//

    // Copy constructor
    template <Comparable Tp>
    digraph<Tp>::digraph(const digraph<Tp> &rhs)
        : m_capacity(rhs.m_capacity),
          m_size(rhs.m_size),
          m_root(nullptr),
          m_adjList(rhs.m_capcacity <= 0 ? nullptr : new digraph_node<Tp>*[rhs.m_capacity])
    {
        if (m_adjList != nullptr)
        {
            for (auto i = 0; i < rhs.m_capacity; i++)
            {
                auto *rhs_curr = rhs.m_adjList[i];
                if (rhs_curr == rhs.m_root)
                    m_root = rhs_curr;

                if (rhs_curr != nullptr)
                {
                    m_adjList[i] = new digraph_node<Tp>(rhs_curr->m_value);
                    auto *lhs_curr = m_adjList[i];
                    rhs_curr = rhs_curr->m_next;

                    while (rhs_curr != nullptr)
                    {
                        lhs_curr->m_next = new digraph_node<Tp>(rhs_curr->m_value);
                        lhs_curr = lhs_curr->m_next;
                        rhs_curr = rhs_curr->m_next;
                    }
                }

                else
                    m_adjList[i] = nullptr;
            }
        }
    }


    // Destructor
    template <Comparable Tp>
    digraph<Tp>::~digraph()
    {
        m_root = nullptr;
        for (const auto &node : m_adjList)
        {
            auto *curr = node;
            while (curr != nullptr)
            {
                auto *temp = curr->m_next;
                delete curr;
                curr = temp;
            }
        }

        delete[] m_adjList;
        m_adjList = nullptr;
    }


    // Member swap specialization
    template <Comparable Tp>
    constexpr void digraph<Tp>::swap(digraph<Tp> &rhs) noexcept
    {
        using std::swap;
        swap(rhs.m_capacity, m_capacity);
        swap(rhs.m_size, m_size);
        swap(rhs.m_root, m_root);
        swap(rhs.m_adjList, m_adjList);
    }


    template <Comparable Tp>
    constexpr std::optional<const typename digraph<Tp>::digraph_node&>
    digraph<Tp>::find_bfs(const Tp &value) const noexcept
    {
        auto *curr = m_root;
        if (curr != nullptr)
        {
            auto visited { std::set<digraph_node *const>{} };
            auto q { std::queue<digraph_node *const>{} };
            q.push(curr);

            while (!q.empty())
            {
                auto *qf = q.front();
                q.pop();

                if (qf->m_value == value)
                    return *qf;

                if (visited.find(qf) != visited.end())
                    continue;

                visited.insert(*qf);
                auto *it = qf;
                while (it != nullptr)
                {
                    q.push(it);
                    it = it->m_next;
                }
            }
        }
        return std::nullopt;
    }


    template <Comparable Tp>
    constexpr std::optional<const digraph_node&>
    digraph<Tp>::find_dfs(const Tp &value) const noexcept
    {
        auto *curr = m_root;
        if (m_root != nullptr)
        {
            auto visited { std::set<digraph_node *const>{} };
            auto s { std::stack<digraph_node *const>{} };
            s.push(curr);

            while (!s.empty())
            {
                auto *st = s.top();
                s.pop();

                if (st->m_value == value)
                    return *st;

                if (visited.find(st) != visited.end())
                    continue;
                visited.insert(st);

                auto *it = st;
                while (it != nullptr)
                {
                    s.push(it);
                    it = it->m_next;
                }
            }
        }
        return std::nullopt;
    }


    // Returns a copy of the current graph transposed (i.e., all edge directions reversed)
    template <Comparable Tp>
    const digraph& digraph<Tp>::transpose() const noexcept
    {
        digraph<Tp> temp(m_capacity);
        for (auto i = 0; i < m_capacity; i++)
        {

        }
    }


    // if each pair of nodes is connected (i.e. has a link)
    // uses Kosaraju's + back-to-back dfs
    template <Comparable Tp>
    constexpr bool digraph<Tp>::is_strongly_connected() const noexcept
    {

    }


    template <Comparable Tp>
    constexpr bool digraph<Tp>::has_link(const Tp &start, const Tp &end) const noexcept
    {
        for (const auto &node : m_adjList)
        {
            if (node != nullptr && node->m_value == start)
            {
                auto *curr = node;
                while (curr != nullptr)
                {
                    if (curr->m_value == end)
                        return true;
                    curr = curr->m_next;
                }
            }
        }
        return false;
    }


    template <Comparable Tp>
    constexpr int digraph<Tp>::count_disconnected() const noexcept
    {
        auto count = 0;
        if (m_root != nullptr)
        {
            auto visited { std::set<digraph_node *const>{} };
            auto q { std::queue<digraph_node *const>{} };
            q.push(m_root);
            while (!q.empty())
            {
                auto *qf = q.front();
                q.pop();
                count++;

                if (visited.find(qf) != visited.end())
                    continue;
                visited.insert(qf);

                auto *it = qf;
                while (it != nullptr)
                {
                    q.push(it);
                    it = it->m_next;
                }
            }
        }
        return m_size - count;
    }


    template <Comparable Tp>
    constexpr int digraph<Tp>::in_degree(const Tp &value) const noexcept
    {
        digraph_node<Tp> *curr = nullptr;
        for (const auto &node : m_adjList)
        {
            if (node != nullptr && node.m_value != value)
                continue;
            curr = node;
            break;
        }

        auto count = 0;
        while (node != nullptr)
        {
            ++count;
            node = node->m_next;
        }

        return count;
    }


    template <Comparable Tp>
    constexpr int digraph<Tp>::out_degree(const Tp &value) const noexcept
    {
        auto count = 0;
        for (const auto &node : m_adjList)
        {
            auto *curr = node;
            while (curr != nullptr)
            {
                if (curr->m_value == value)
                {
                    ++count;
                    break;
                }
                curr = curr->m_next;
            }
        }
        return count;
    }


    // Private helper function; gets index of node in the adjacency list, if it exists
    template <Comparable Tp>
    constexpr int digraph<Tp>::indexOf(const Tp &value) const noexcept
    {
        for (const auto &node : m_adjList)
        {
            if (node != nullptr && node->m_value == value)
                return i;
        }
        return -1;
    }


    template <Comparable Tp>
    constexpr bool digraph<Tp>::try_link(const Tp &lhs, const Tp &rhs) noexcept
    {
        auto i = indexOf(lhs);
        auto *curr = (i != -1) ? m_adjList[i] : nullptr;
        if (curr != nullptr)
        {
            digraph_node<Tp> *prev = nullptr;
            while (curr != nullptr)
            {
                prev = curr;
                curr = curr->m_next;
            }
            prev->m_next = m_adjList[indexOf(rhs)];
            return true;
        }
        return false;
    }


    // Pushes a weighted node into the graph. Does not establish connectivity.
    template <Comparable Tp>
    constexpr bool digraph<Tp>::push_vertex(const Tp &value, const int weight) noexcept
    {
        if (full() || contains(value)) return false;
        for (auto i = 0; i < m_capacity; i++)
        {
            if (m_adjList[i] == nullptr)
            {
                m_adjList[i] = new digraph_node<Tp>(value, weight);
                return true;
            }
        }
        return false;
    }


    // push_edge helper function
    template <Comparable Tp>
    constexpr int digraph<Tp>::try_push(const Tp &value, const int weight) noexcept
    {
        for (auto i = 0; i < m_capacity; i++)
        {
            if (m_adjList[i] != nullptr && m_adjList[i]->m_value == value)
                break;

            if (m_adjList[i] == nullptr)
            {
                m_adjList[i] = new digraph_node<Tp>(value, weight);
                return i;
            }
        }
        return -1;
    }


    // Pushes a weighted edge into the graph.
    template <Comparable Tp>
    constexpr bool digraph<Tp>::push_edge(const Tp &start,
                                          const Tp &end,
                                          const int start_weight = default_weight,
                                          const int end_weight = default_weight) noexcept
    {
        // if contains start, link to end. add end to adjacency list, point to start, disregard(?) or overwrite weight.
        // if contains end, link to start. add start to adjacency list, point to end, disregard(?) or overwrite weight.
        // if contains start and contains end, point start to end and end to start.
        // if does not contain start and does not contain end, add start to adjacency, add end to adjacency
        // then point start to end and end to start. Add start and end with specified weights.

        // Has link already been set if both exist? What do do if no space in m_adjList?
        if (full()) return false;
        auto has_start = contains(start), has_end = contains(end);

        // At least one is contained
        if (has_start || has_end)
        {
            // Both are contained + matching link condition
            if (has_start && has_end && !has_link(start, end))
            {
                try_link(m_adjList[indexOf(start)], end);
                try_link(m_adjList[indexOf(end)], start);
            }

            // One is contained only or double containment fails link condition
            else
            {
                if (m_capacity - m_size < 1) return false;  // cannot push, leads to overflow

                auto nvalue = (has_start) ? end : start;
                auto nweight = (has_start) ? end_weight : start_weight;

                int index = try_push(nvalue, nweight);
                if (index != -1)
                {
                    try_link(m_adjList[index], (has_start) ? start : end);
                    try_link(m_adjList[indexOf((has_start) ? end : start)], (has_start) ? start : end);
                }
            }
        }

        // None of them are contained
        else
        {
            if (m_capacity - m_size < 2) return false;  // cannot push, leads to overflow
            auto s_index = try_push(start, start_weight), e_index = try_push(end, end_weight);
            try_link(m_adjList[s_index], end);
            try_link(m_adjList[e_index], start);
        }
    }


    // Traversal operates on Dijkstra's algorithm
    std::ostream& operator<<(std::ostream &os, const digraph<Tp> &graph) noexcept
    {
        auto s    { std::set<digraph_node *const>{} };

        // dist should be a map?
        auto dist { std::vector<std::pair<digraph_node *const, int>(m_size, std::make_pair(nullptr, std::numeric_limits<int>::max())) };
        auto prev { std::vector<digraph_node *const>{} };

        for (auto i = 0; i < m_capacity; i++)
        {
            if (node != nullptr)
            {
                dist[i].first = node;
                s.insert(node);
            }
        }
        dist[0].second = 0;

        while (!s.empty())
        {
            auto *curr = *(s.begin());
            s.erase(curr);

            auto *it = curr;
            while (it != nullptr)
            {

            }
        }

    }


    //************ Non-Member Function Implementations ************//


    template <Comparable Tp>
    constexpr void swap(digraph<Tp> &lhs, digraph<Tp> &rhs) noexcept
    { lhs.swap(rhs); }


    /*
     * Two directed acyclic graphs (DAGs) are deemed equal if:
     * They both have the same size (but not necessarily the same capacity)
     * One graph has nodes with the same containing values and weights as the other graph
     * The adjacency list of each node with a specific (value, weight) pair is the same
     * Adjacency list sameness ignores insertion order
     * */
    template <Comparable Tp>
    constexpr bool operator==(const digraph<Tp> &lhs, const digraph<Tp> &rhs) noexcept
    {
        if (lhs.size() != rhs.size())
            return false;
        // Search for one value for all values in one array and compare to all values if other
        // if found, then check parents of both
    }


    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator!=(const digraph<Tp> &lhs, const digraph<Tp> &rhs) noexcept
    { return !operator==(lhs, rhs); }


}   // namespace nonlinear::graph


#endif //DS_GRAPH_DIGRAPH_H
