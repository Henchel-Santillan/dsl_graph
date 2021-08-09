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
        struct bitree_node
        {
            // No-throw constructors
            constexpr bitree_node() noexcept = default;
            constexpr explicit bitree_node(const Tp &value) noexcept
                : m_value(value),
                  m_left(nullptr),
                  m_right(nullptr) {}

            // No-throw copy assignment
            constexpr bitree_node& operator=(const bitree_node &rhs) noexcept
            {
                if (this == &rhs) return *this;
                m_value = rhs.m_value;
                m_left = rhs.m_left;
                m_right = rhs.m_right;
                return *this;
            }

            // Delegate resource destruction responsibility to the containing class
            virtual ~bitree_node() noexcept = default;

            Tp m_value;
            bitree_node *m_left, *m_right;

        };  // struct bitree_node

    }   // namespace details



    template <Comparable Tp>
    class binary_tree
    {
    public:
        using bitree_node = typename details::bitree_node<Tp>;


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

        [[nodiscard]] constexpr bitree_node* root() const noexcept    { return m_root; }
        [[nodiscard]] constexpr int size() const noexcept             { return m_size; }

        template <Comparable Tp>
        [[nodiscard]] constexpr int binary_tree<Tp>::depth(binary_tree::bitree_node *const root) const noexcept
        { return (root == nullptr) ? 0 : std::max(depth(root->m_left) + 1, depth(root->m_right) + 1); }

        constexpr void in_order(bitree_node*, std::ostream&) const noexcept;
        constexpr void post_order(bitree_node*, std::ostream&) const noexcept;
        constexpr void pre_order(bitree_node*, std::ostream&) const noexcept;
        constexpr bitree_node* last_level_order() const;

        friend std::ostream& operator<<(std::ostream&, const binary_tree<Tp>&) noexcept;

        virtual constexpr const bitree_node& maxKey(const bitree_node&) const noexcept;
        virtual constexpr const bitree_node& minKey(const bitree_node&) const noexcept;

        [[nodiscard]] virtual constexpr const Tp& max(const bitree_node &from) const noexcept  { return maxKey(from).m_value; }
        [[nodiscard]] virtual constexpr const Tp& min(const bitree_node &from) const noexcept  { return minKey(from).m_value; }
        virtual constexpr const Tp& max() const;
        virtual constexpr const Tp& min() const;    // throws if root is nullptr

        virtual constexpr std::optional<bitree_node&> parentOf(const bitree_node&) const noexcept;
        virtual constexpr std::optional<bitree_node&> parentOf(const Tp&) const noexcept;
        virtual constexpr std::optional<bitree_node&> find(const Tp&) const noexcept;

        virtual std::optional<std::stack<bitree_node*>> pathTo(bitree_node*) const noexcept;
        virtual std::vector<bitree_node*> toVector(const bitree_node&) const noexcept;


        //****** Properties ******//
        [[nodiscard]] constexpr bool is_complete(bitree_node *const node) const noexcept  { return complete(node, 0); }
        [[nodiscard]] constexpr bool is_perfect(bitree_node *const node) const noexcept   { return perfect(node, 0); }
        constexpr bool is_balanced(bitree_node*) const noexcept;
        constexpr bool is_full(bitree_node*) const noexcept;
        constexpr bool is_mirror(bitree_node*, bitree_node*) const noexcept;


        //****** Modifiers ******//
        virtual constexpr bool push(const Tp&);
        virtual constexpr bool pop(const Tp&);


    protected:
        bitree_node *m_root;    // prefer smart ptr; this impl uses owning raw ptr
        int m_size;


    private:
        constexpr bitree_node* copy(bitree_node*);
        constexpr bool complete(bitree_node*, int) const noexcept;
        constexpr bool perfect(bitree_node*, int, int=0) const noexcept;
        constexpr bool hasPath(bitree_node*, const bitree_node&, std::stack<bitree_node*>&) const noexcept;
        constexpr void vectorize(bitree_node*, std::vector<bitree_node*>&) const noexcept;

    };  // class binary_tree



    //************ Member Function Implementations ************//


    // Copy constructor helper function
    template <Comparable Tp>
    constexpr typename binary_tree<Tp>::bitree_node*
    binary_tree<Tp>::copy(binary_tree::bitree_node *const root)
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
            auto q { std::queue<bitree_node*>{} };
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
    constexpr void binary_tree<Tp>::in_order(bitree_node *const node, std::ostream &os) const noexcept
    {
        if (node == nullptr) return;
        in_order(node->m_left);
        os << node->m_value;
        in_order(node->m_right);
    }


    // Writes the tree to an output stream via post-order traversal
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::post_order(bitree_node *const node, std::ostream &os) const noexcept
    {
        if (node == nullptr) return;
        post_order(node->m_left);
        post_order(node->m_right);
        os << node->m_value;
    }


    // Writes the tree to an output stream via pre-order traversal
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::pre_order(bitree_node *const node, std::ostream &os) const noexcept
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
            auto q { std::queue<binary_tree<Tp>::bitree_node *const>{} };
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
    binary_tree<Tp>::maxKey(const binary_tree::bitree_node &root) const noexcept
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
    constexpr const typename binary_tree<Tp>::bitree_node&
    binary_tree<Tp>::minKey(const binary_tree::bitree_node &root) const noexcept
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
    binary_tree<Tp>::parentOf(const bitree_node &node) const noexcept
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
    constexpr std::optional<const typename binary_tree<Tp>::bitree_node&>
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
    constexpr bool binary_tree<Tp>::complete(bitree_node *const node, const int index) const noexcept
    {
        if (node == nullptr) return true;
        if (index >= m_size) return false;
        return complete(node->m_left, 2 * index + 1) &&
               complete(node->m_right, 2 * index + 2);
    }


    // is_perfect helper function
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::perfect(bitree_node *const node, const int depth, const int level) const noexcept
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
    constexpr bool binary_tree<Tp>::is_balanced(bitree_node *const node) const noexcept
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
    constexpr bool binary_tree<Tp>::is_full(bitree_node *const node) const noexcept
    {
        if (node == nullptr || (node->m_left == nullptr || node->m_right == nullptr))
            return true;

        if (node->m_left != nullptr && node->m_right != nullptr)
            return is_full(node->m_left) && is_full(node->m_right);

        return false;
    }


    // Checks if tree and each subtree are mirrors of each other w.r.t keys
    template <Comparable Tp>
    constexpr bool binary_tree<Tp>::is_mirror(bitree_node *const lnode, bitree_node *const rnode) const noexcept
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
            m_root = new bitree_node<Tp>(value);
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
                    curr->m_left = new bitree_node<Tp>(value);
                    break;
                }
                else
                    q.push(curr->m_left);

                if (curr->m_right == nullptr)
                {
                    curr->m_right = new bitree_node<Tp>(value);
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
    constexpr bool binary_tree<Tp>::hasPath(bitree_node *const root, const bitree_node &node,
                                            std::stack<bitree_node*> &nodes) const noexcept
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
    binary_tree<Tp>::pathTo(bitree_node *const node) const noexcept
    {
        auto s { std::stack<bitree_node*>{} };
        auto b = hasPath(m_root, node, s);
        return (b) ? s : std::nullopt;
    }


    // toVector helper function
    template <Comparable Tp>
    constexpr void binary_tree<Tp>::vectorize(bitree_node *const root, std::vector<bitree_node*> &vector) const noexcept
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
    binary_tree<Tp>::toVector(const bitree_node &root) const noexcept
    {
        auto v { std::vector<bitree_node*>{} };
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
