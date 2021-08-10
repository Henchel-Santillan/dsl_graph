#ifndef DS_GRAPH_BINARY_TREE_H
#define DS_GRAPH_BINARY_TREE_H


#include <limits>
#include <ostream>
#include <queue>
#include <stack>
#include <stdexcept>
#include <vector>

#include "traits.h"


namespace dsl::nonlinear::tree
{
    namespace details
    {
        template <Comparable Tp>
        struct typename binary_tree<Tp>::bitree_node
        {
            // No-throw constructors
            constexpr typename binary_tree<Tp>::bitree_node() noexcept = default;
            constexpr explicit typename binary_tree<Tp>::bitree_node(const Tp &value) noexcept
                : m_value(value),
                  m_left(nullptr),
                  m_right(nullptr) {}

            // No-throw copy assignment
            constexpr typename binary_tree<Tp>::bitree_node& operator=(const typename binary_tree<Tp>::bitree_node &rhs) noexcept
            {
                if (this == &rhs) return *this;
                m_value = rhs.m_value;
                m_left = rhs.m_left;
                m_right = rhs.m_right;
                return *this;
            }

            // Delegate resource destruction responsibility to the containing class
            virtual ~typename binary_tree<Tp>::bitree_node() noexcept = default;

            Tp m_value;
            typename binary_tree<Tp>::bitree_node *m_left, *m_right;

        };  // struct typename binary_tree<Tp>::bitree_node

    }   // namespace details



    template <Comparable Tp>
    class binary_tree
    {
    public:
        using typename binary_tree<Tp>::bitree_node = typename details::typename binary_tree<Tp>::bitree_node<Tp>;


        //****** Member Functions ******//

        // No-throw constructor
        binary_tree() noexcept
            : m_root(nullptr),
              m_size(0) {}

        // Copy constructor
        binary_tree(const binary_tree &rhs)
            : m_root(rhs.m_root == nullptr ? nullptr : copy()),
              m_size(rhs.m_size) {}

        // Move constructor
        binary_tree(binary_tree &&rhs) noexcept
            : binary_tree()
        { swap(rhs); }

        // Pass-by-value copy/move assignment
        binary_tree& operator=(binary_tree rhs) noexcept
        { swap(rhs); return *this; }

        virtual ~binary_tree();
        constexpr void swap(binary_tree &rhs) noexcept;


        //****** Access and Traversal ******//

        [[nodiscard]] constexpr typename binary_tree<Tp>::bitree_node* root() const noexcept    { return m_root; }
        [[nodiscard]] constexpr int size() const noexcept             { return m_size; }

        template <Comparable Tp>
        [[nodiscard]] constexpr int binary_tree<Tp>::depth(binary_tree::typename binary_tree<Tp>::bitree_node *const root) const noexcept
        { return (root == nullptr) ? 0 : std::max(depth(root->m_left) + 1, depth(root->m_right) + 1); }

        constexpr void in_order(typename binary_tree<Tp>::bitree_node*, std::ostream&) const noexcept;
        constexpr void post_order(typename binary_tree<Tp>::bitree_node*, std::ostream&) const noexcept;
        constexpr void pre_order(typename binary_tree<Tp>::bitree_node*, std::ostream&) const noexcept;
        constexpr typename binary_tree<Tp>::bitree_node* last_level_order() const;

        friend std::ostream& operator<<(std::ostream&, const binary_tree<Tp>&) noexcept;

        virtual constexpr const typename binary_tree<Tp>::bitree_node& maxKey(const typename binary_tree<Tp>::bitree_node&) const noexcept;
        virtual constexpr const typename binary_tree<Tp>::bitree_node& minKey(const typename binary_tree<Tp>::bitree_node&) const noexcept;

        [[nodiscard]] virtual constexpr const Tp& max(const typename binary_tree<Tp>::bitree_node &from) const noexcept  { return maxKey(from).m_value; }
        [[nodiscard]] virtual constexpr const Tp& min(const typename binary_tree<Tp>::bitree_node &from) const noexcept  { return minKey(from).m_value; }
        virtual constexpr const Tp& max() const;
        virtual constexpr const Tp& min() const;    // throws if root is nullptr

        virtual constexpr std::optional<typename binary_tree<Tp>::bitree_node&> parentOf(const typename binary_tree<Tp>::bitree_node&) const noexcept;
        virtual constexpr std::optional<typename binary_tree<Tp>::bitree_node&> parentOf(const Tp&) const noexcept;
        virtual constexpr std::optional<typename binary_tree<Tp>::bitree_node&> find(const Tp&) const noexcept;

        virtual std::optional<std::stack<typename binary_tree<Tp>::bitree_node*>> pathTo(typename binary_tree<Tp>::bitree_node*) const noexcept;
        virtual std::vector<typename binary_tree<Tp>::bitree_node*> toVector(const typename binary_tree<Tp>::bitree_node&) const noexcept;


        //****** Properties ******//
        [[nodiscard]] constexpr bool is_complete(typename binary_tree<Tp>::bitree_node *const node) const noexcept  { return complete(node, 0); }
        [[nodiscard]] constexpr bool is_perfect(typename binary_tree<Tp>::bitree_node *const node) const noexcept   { return perfect(node, 0); }
        constexpr bool is_balanced(typename binary_tree<Tp>::bitree_node*) const noexcept;
        constexpr bool is_full(typename binary_tree<Tp>::bitree_node*) const noexcept;
        constexpr bool is_mirror(typename binary_tree<Tp>::bitree_node*, typename binary_tree<Tp>::bitree_node*) const noexcept;


        //****** Modifiers ******//
        virtual constexpr bool push(const Tp&);
        virtual constexpr bool pop(const Tp&);


    protected:
        typename binary_tree<Tp>::bitree_node *m_root;    // prefer smart ptr; this impl uses owning raw ptr
        int m_size;


    private:
        constexpr typename binary_tree<Tp>::bitree_node* copy(typename binary_tree<Tp>::bitree_node*);
        constexpr bool complete(typename binary_tree<Tp>::bitree_node*, int) const noexcept;
        constexpr bool perfect(typename binary_tree<Tp>::bitree_node*, int, int=0) const noexcept;
        constexpr bool hasPath(typename binary_tree<Tp>::bitree_node*, const typename binary_tree<Tp>::bitree_node&, std::stack<typename binary_tree<Tp>::bitree_node*>&) const noexcept;
        constexpr void vectorize(typename binary_tree<Tp>::bitree_node*, std::vector<typename binary_tree<Tp>::bitree_node*>&) const noexcept;

    };  // class binary_tree



    //************ Member Function Implementations ************//


    // Copy constructor helper function
    template <Comparable Tp>
    constexpr typename binary_tree<Tp>::bitree_node*
    binary_tree<Tp>::copy(typename binary_tree<Tp>::bitree_node *const root)
    {
        if (root == nullptr) return nullptr;
        root->m_left = copy(root->m_left);
        root->m_right = copy(root->m_right);
        return root;
    }


    // Virtual destructor
    template <Comparable Tp>
    binary_tree<Tp>::~binary_tree()
    {
        if (m_root != nullptr)
        {
            auto q { std::queue<typename binary_tree<Tp>::bitree_node*>{} };
            q.push(m_root);
            while (!q.empty())
            {
                auto *curr = q.front();
                q.pop();

                if (curr->m_left != nullptr)
                    q.push(curr->m_left);
                if (curr->m_right != nullptr)
                    q.push(curr->m_right);

                delete curr;
                curr = nullptr;
            }
        }
    }


    // Member swap function
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::swap(binary_tree<Tp> &rhs) noexcept
    {
        using std::swap;
        swap(rhs.m_root, m_root);
        swap(rhs.m_size, m_size);
    }


    // Writes the tree to an output stream via in-order traversal
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::in_order(typename binary_tree<Tp>::bitree_node *const node, std::ostream &os) const noexcept
    {
        if (node == nullptr) return;
        in_order(node->m_left);
        os << node->m_value;
        in_order(node->m_right);
    }


    // Writes the tree to an output stream via post-order traversal
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::post_order(typename binary_tree<Tp>::bitree_node *const node, std::ostream &os) const noexcept
    {
        if (node == nullptr) return;
        post_order(node->m_left);
        post_order(node->m_right);
        os << node->m_value;
    }


    // Writes the tree to an output stream via pre-order traversal
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::pre_order(typename binary_tree<Tp>::bitree_node *const node, std::ostream &os) const noexcept
    {
        if (node == nullptr) return;
        os << node->m_value;
        pre_order(node->m_left);
        pre_order(node->m_right);
    }


    // Finds the deepest, rightmost node in the tree
    template <Comparable Tp>
    constexpr typename binary_tree<Tp>::bitree_node*
    binary_tree<Tp>::last_level_order() const
    {
        if (m_root == nullptr)
            throw std::out_of_range("Cannot find last node in level order in empty tree.");

        bitree_node *curr = m_root, *prev = nullptr;
        while (curr != nullptr)
        {
            prev = curr;
            curr = ( depth(curr->m_right) >= depth(curr->m_left()) )
                    ? curr->m_right : curr->m_left;
        }
        return prev;
    }


    // Overload of operator<< : writes the tree to an output stream in level order
    template <Comparable Tp>
    std::ostream& operator<<(std::ostream &os, const binary_tree<Tp> &t) noexcept
    {
        if (t.m_root != nullptr)
        {
            auto q { std::queue<binary_tree<Tp>::typename binary_tree<Tp>::bitree_node *const>{} };
            q.push(m_root);
            while (!q.empty())
            {
                auto *curr = q.front();
                q.pop();

                os << curr->m_value << " ";

                if (curr->m_left != nullptr)
                    q.push(curr->m_left);
                if (curr->m_right != nullptr)
                    q.push(curr->m_right);
            }
        }
        return os;
    }


    // Returns the node in the tree with the maximum value, if the tree is not empty
    template<Comparable Tp>
    constexpr const typename binary_tree<Tp>::bitree_node&
    binary_tree<Tp>::maxKey(const typename binary_tree<Tp>::bitree_node &root) const noexcept
    {
        auto max;
        auto maxval = std::numeric_limits<Tp>::min();

        auto q { std::queue<bitree_node *const>{} };
        q.push(root);
        while (!q.empty())
        {
            auto *curr = q.front();
            q.pop();

            if (curr->m_value > maxval)
            {
                maxval = curr->m_value;
                max = *curr;
            }

            if (curr->m_left != nullptr)
                q.push(curr->m_left);
            if (curr->m_right != nullptr)
                q.push(curr->m_right);
        }

        return max;
    }


    // Returns the node in the tree with the minimum value, if the tree is not empty
    template <Comparable Tp>
    constexpr const typename binary_tree<Tp>::typename binary_tree<Tp>::bitree_node&
    binary_tree<Tp>::minKey(const typename binary_tree<Tp>::bitree_node &root) const noexcept
    {
        auto min;
        auto minval = std::numeric_limits<Tp>::max();

        auto q { std::queue<bitree_node *const>{} };
        q.push(root);
        while (!q.empty())
        {
            auto *curr = q.front();
            q.pop();

            if (curr->m_value < minval)
            {
                minval = curr->m_value;
                min = *curr;
            }

            if (curr->m_left != nullptr)
                q.push(curr->m_left);
            if (curr->m_right != nullptr)
                q.push(curr->m_right);
        }

        return min;
    }


    // Finds the maximum value in the tree, beginning from the root
    template <Comparable Tp>
    constexpr const Tp& binary_tree<Tp>::max() const
    {
        if (m_root == nullptr)
            throw std::out_of_range("Cannot find maximum value in empty tree.");
        return max(*m_root);
    }


    // Finds the minimum value in the tree, beginning from the root
    template <Comparable Tp>
    constexpr const Tp& binary_tree<Tp>::min() const
    {
        if (m_root == nullptr)
            throw std::out_of_range("Cannot find maximum value in empty tree.");
        return min(*m_root);
    }


    // Finds the parent node of the given node
    template <Comparable Tp>
    constexpr std::optional<typename binary_tree<Tp>::bitree_node&>
    binary_tree<Tp>::parentOf(const typename binary_tree<Tp>::bitree_node &node) const noexcept
    {
        bitree_node *curr = nullptr, *prev = nullptr;
        auto q { std::queue<bitree_node&>{} };
        q.push(*m_root);

        while (!q.empty())
        {
            curr = &(q.front());
            q.pop();

            if (curr->m_value == node.m_value)
                break;
            prev = curr;

            if (curr->m_left != nullptr)
                q.push(*(curr->m_left));
            if (curr->m_right != nullptr)
                q.push(*(curr->m_right));
        }

        return (prev == nullptr) ? std::nullopt : *prev;
    }


    // Finds the parent node of the node with the given value, if it exists
    template <Comparable Tp>
    constexpr std::optional<typename binary_tree<Tp>::bitree_node&>
    binary_tree<Tp>::parentOf(const Tp &value) const noexcept
    {
        auto q { std::queue<bitree_node&>{} };
        q.push(m_root);
        bool found = false;

        bitree_node *curr = nullptr, *prev = nullptr;
        while (!q.empty())
        {
            curr = &(q.front());
            q.pop();

            if (curr->m_value == value)
            {
                found = true;
                break;
            }

            prev = curr;

            if (curr->m_left != nullptr)
                q.push(*(curr->m_left));
            if (curr->m_right != nullptr)
                q.push(*(curr->m_right));
        }
        return (prev == nullptr || !found) ? std::nullopt : *prev;
    }


    // Finds the node with the value in the tree, if it exists
    template <Comparable Tp>
    constexpr std::optional<typename binary_tree<Tp>::bitree_node&>
    binary_tree<Tp>::find(const Tp &value) const noexcept {
        if (m_root != nullptr && m_root->m_value == value)
            return *m_root;

        auto p = parentOf(value);
        if (p) {
            auto parent = *p;
            if (parent.m_left != nullptr && parent.m_right != nullptr)
                return (parent.m_left->m_value == value)
                       ? *(parent.m_left) : *(parent.m_right);
            return (parent.m_left != nullptr) ? *(parent.m_left) : *(parent.m_right);
        }
        return std::nullopt;
    }


    // is_complete helper function
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::complete(typename binary_tree<Tp>::bitree_node *const node, const int index) const noexcept
    {
        if (node == nullptr) return true;
        if (index >= m_size) return false;
        return complete(node->m_left, 2 * index + 1) &&
               complete(node->m_right, 2 * index + 2);
    }


    // is_perfect helper function
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::perfect(typename binary_tree<Tp>::bitree_node *const node, const int depth, const int level) const noexcept
    {
        if (node == nullptr) return true;

        if (node->m_left != nullptr && node->m_right != nullptr)
            return depth == level + 1;

        if (node->m_left == nullptr || node->m_right == nullptr)
            return false;

        return perfect(node->m_left, depth, level + 1) &&
               perfect(node->m_right, depth, level + 1);
    }
    
    
    // Checks if the tree is balanced (difference between left and right depth is less than or equal to 1)
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::is_balanced(typename binary_tree<Tp>::bitree_node *const node) const noexcept
    {
        if (node == nullptr) return true;
        if (abs(depth(node->m_left) - depth(node->m_right)) <= 1 &&
            is_balanced(node->m_left) &&
            is_balanced(node->m_right))
            return true;
        return false;
    }


    // Checks if the tree is full (every node other than leaves have 2 children)
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::is_full(typename binary_tree<Tp>::bitree_node *const node) const noexcept
    {
        if (node == nullptr || (node->m_left == nullptr || node->m_right == nullptr))
            return true;

        if (node->m_left != nullptr && node->m_right != nullptr)
            return is_full(node->m_left) && is_full(node->m_right);

        return false;
    }


    // Checks if tree and each subtree are mirrors of each other w.r.t keys
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::is_mirror(typename binary_tree<Tp>::bitree_node *const lnode, typename binary_tree<Tp>::bitree_node *const rnode) const noexcept
    {
        if (lnode == nullptr && rnode == nullptr) return true;
        if (lnode == nullptr || rnode == nullptr) return false;
        return lnode->m_value == rnode->m_value &&
               is_mirror(lnode->m_left, rnode->m_right) &&
               is_mirror(lnode->m_right, rnode->m_left);
    }


    // Constructs a new node with the given value and inserts into the rightmost leaf of the tree
    // For a binary tree with max capacity == std::numeric_limits<Tp>::max(), will always return true
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::push(const Tp &value)
    {
        if (m_root == nullptr)
            m_root = new typename binary_tree<Tp>::bitree_node<Tp>(value);
        else
        {
            auto q { std::queue<bitree_node*>{} };
            q.push(m_root);

            while (!q.empty())
            {
                auto *curr = q.front();
                q.pop();

                if (curr->m_left == nullptr)
                {
                    curr->m_left = new typename binary_tree<Tp>::bitree_node<Tp>(value);
                    break;
                }
                else
                    q.push(curr->m_left);

                if (curr->m_right == nullptr)
                {
                    curr->m_right = new typename binary_tree<Tp>::bitree_node<Tp>(value);
                    break;
                }
                else
                    q.push(curr->m_right);
            }
        }
        m_size++;
        return true;
    }


    // Removes the node in the tree with the given value
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::pop(const Tp &value)
    {
        auto *last = last_level_order();
        if (m_root != nullptr && m_root->m_value == value)
            m_root->m_value = last->m_value;

        else
        {
            auto p = parentOf(value);
            if (p)
            {
                auto parent = *p;
                if (parent.m_left != nullptr && parent.m_right != nullptr)
                    auto *node = (parent.m_left->m_value == value) ? parent.m_left : parent.m_right;
                else
                    auto *node = (parent.m_left != nullptr) ? parent.m_left : parent.m_right;

                node->m_value = last->m_value;
            }
            else
                return false;
        }

        delete last;
        last = nullptr;

        size--;
        return true;
    }


    // pathTo helper function
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::hasPath(typename binary_tree<Tp>::bitree_node *const root,
                                            const typename binary_tree<Tp>::bitree_node &node,
                                            std::stack<typename binary_tree<Tp>::bitree_node*> &nodes) const noexcept
    {
        if (root == nullptr)
            return false;
        nodes.push(root);

        if (root->m_value == node.m_value)
        {
            nodes.pop();
            return true;
        }

        if (hasPath(root->m_left, node, nodes) ||
            hasPath(root->m_right, node, nodes))
            return true;

        return false;
    }


    // Returns a stack of all the nodes, beginning from the root of the tree, to the given node
    template <Comparable Tp>
    std::optional<std::stack<typename binary_tree<Tp>::bitree_node*>>
    binary_tree<Tp>::pathTo(typename binary_tree<Tp>::bitree_node *const node) const noexcept
    {
        auto s { std::stack<typename binary_tree<Tp>::bitree_node*>{} };
        auto b = hasPath(m_root, node, s);
        return (b) ? s : std::nullopt;
    }


    // toVector helper function
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::vectorize(typename binary_tree<Tp>::bitree_node *const root, std::vector<typename binary_tree<Tp>::bitree_node*> &vector) const noexcept
    {
        if (root == nullptr)
            return;

        vectorize(root->m_left, vector);
        vector.push_back(root);
        vectorize(root->m_right, vector);
    }


    // Returns a vector representation of the tree starting at a given node
    template <Comparable Tp>
    std::vector<typename binary_tree<Tp>::bitree_node*>
    binary_tree<Tp>::toVector(const typename binary_tree<Tp>::bitree_node &root) const noexcept
    {
        auto v { std::vector<typename binary_tree<Tp>::bitree_node*>{} };
        vectorize(root);
        return v;
    }


    //************ Non-Member Function Implementations ************//

    // operator== helper function
    template <Comparable Tp>
    constexpr bool bitree_same(details::bitree_node<Tp> *const lroot, details::bitree_node<Tp> *const rroot) noexcept
    {
        if (lroot == nullptr && rroot == nullptr)
            return true;

        if (lroot != nullptr && rroot != nullptr) {
            return lroot->m_value == rroot->m_value &&
                   bitree_same(lroot->m_left, rroot->m_left) &&
                   bitree_same(lroot->m_right, rroot->m_right);
        }
        return false;
    }


    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator==(const binary_tree<Tp> &lhs,
                              const binary_tree<Tp> &rhs) noexcept
    { return lhs.size() == rhs.size() && bitree_same(lhs.m_root, rhs.m_root); }


    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator!=(const binary_tree<Tp> &lhs,
                                            const binary_tree<Tp> &rhs) noexcept
    { return !operator==(lhs, rhs); }


    template <Comparable Tp>
    constexpr void swap(binary_tree<Tp> &lhs, binary_tree<Tp> &rhs) noexcept
    { lhs.swap(rhs); }

}   // namespace nonlinear::tree

#endif //DS_GRAPH_BINARY_TREE_H
