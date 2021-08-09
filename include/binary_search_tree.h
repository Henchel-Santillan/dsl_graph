#ifndef DS_GRAPH_BINARY_SEARCH_TREE_H
#define DS_GRAPH_BINARY_SEARCH_TREE_H


#include "binary_tree.h"


namespace nonlinear::tree
{
    template <Comparable Tp>
    class binary_search_tree : public binary_tree<Tp>
    {
    public:
        using bitree_node = typename binary_tree<Tp>::bitree_node;


        //****** Member Functions ******//
        // Constructor
        binary_search_tree() noexcept
            : binary_tree<Tp>() {}

        // Copy constructor
        binary_search_tree(const binary_search_tree &rhs)
            : binary_tree<Tp>(rhs) {}

        // Move constructor
        binary_search_tree(binary_search_tree &&rhs) noexcept
            : binary_tree<Tp>(rhs) {}

        // Pass-by-value copy/move assignment
        binary_search_tree& operator=(binary_search_tree rhs) noexcept
        { binary_tree<Tp>::operator=(rhs); return *this; }

        virtual ~binary_search_tree() = default;


        //****** Access ******//
        constexpr const bitree_node& maxKey(const bitree_node&) const noexcept override;
        constexpr const bitree_node& minKey(const bitree_node&) const noexcept override;

        constexpr std::optional<bitree_node&> parentOf(const Tp&) const noexcept override;
        [[nodiscard]] constexpr std::optional<bitree_node&> parentOf(const bitree_node &node) const noexcept override
        { return parentOf(node->m_value); }

        constexpr std::optional<const bitree_node&> find(const Tp&) const noexcept override;
        virtual std::optional<std::stack<bitree_node*>> pathTo(const Tp&) const noexcept;

        [[nodiscard]] std::optional<std::stack<bitree_node*>> pathTo(bitree_node *const node) const noexcept override
        { return (node == nullptr) ? std::nullopt : pathTo(node->m_value); }


        //****** Modifiers ******//
        constexpr bool push(const Tp&) override;
        constexpr bool pop(const Tp &) override;


    private:
        constexpr bool remove(bitree_node*, const Tp&);
        using binary_tree<Tp>::is_mirror;

    };  // class binary_search_tree



    //************ Member Function Implementations ************//


    template <Comparable Tp>
    constexpr const typename binary_search_tree<Tp>::bitree_node&
    binary_search_tree<Tp>::maxKey(const bitree_node &root) const noexcept
    {
        auto *curr = &root;
        while (curr->m_right != nullptr)
            curr = curr->m_right;
        return *curr;
    }


    template <Comparable Tp>
    constexpr const typename binary_search_tree<Tp>::bitree_node&
    binary_search_tree<Tp>::minKey(const bitree_node &root) const noexcept
    {
        auto *curr = &root;
        while (curr->m_left != nullptr)
            curr = curr->m_left;
        return *curr;
    }


    template <Comparable Tp>
    constexpr std::optional<typename binary_search_tree<Tp>::bitree_node&>
    binary_search_tree<Tp>::parentOf(const Tp &value) const noexcept
    {
        if (this->m_root == nullptr || this->m_root->m_value == value)
            return std::nulopt;

        bitree_node *curr = this->m_root, *prev = nullptr;
        while (curr != nullptr)
        {
            if (curr->m_value == value)
                return prev;
            prev = curr;
            curr = (value < curr->m_value) ? curr->m_left : curr->m_right;
        }
        return std::nullopt;
    }


    template <Comparable Tp>
    constexpr std::optional<const typename binary_search_tree<Tp>::bitree_node&>
    binary_search_tree<Tp>::find(const Tp &value) const noexcept
    {
        if (this->m_root == nullptr)
            return std::nullopt;

        auto *curr = this->m_root;
        while (curr != nullptr)
        {
            if (curr->m_value == value)
                return *curr;
            curr = (value < curr->m_value) ? curr->m_left : curr->m_right;
        }
        return std::nullopt;
    }


    template <Comparable Tp>
    std::optional<std::stack<typename binary_search_tree<Tp>::bitree_node*>>
    binary_search_tree<Tp>::pathTo(const Tp &value) const noexcept
    {
        auto s = { std::stack<bitree_node*>{} };

        auto *curr = this->m_root;
        while (curr != nullptr)
        {
            if (curr->m_value == value)
                break;
            s.push(curr);
            curr = (value < curr->m_value) ? curr->m_left : curr->m_right;
        }

        return (curr == nullptr) ? std::nullopt : s;
    }


    template <Comparable Tp>
    constexpr bool binary_search_tree<Tp>::push(const Tp &value)
    {
        if (this->m_root == nullptr)
            this->m_root = new bitree_node<Tp>(value);
        else
        {
            bitree_node *curr = this->m_root, *prev = nullptr;
            while (curr != nullptr)
            {
                if (curr->m_value == value)
                    return false;
                prev = curr;
                curr = (value < curr->m_value) ? curr->m_left : curr->m_right;
            }

            if (value < prev->m_value)
                prev->m_left = new bitree_node<Tp>(value);
            else
                prev->m_right = new bitree_node<Tp>(value);

        }
        this->m_size++;
        return true;
    }


    // remove helper function
    template <Comparable Tp>
    constexpr bool binary_search_tree<Tp>::remove(bitree_node *const root, const Tp &value)
    {
        if (root == nullptr)
            return false;

        if (value < root->m_value)
            return remove(root->m_left, value);
        else if (value > root->m_value)
            return remove(root->m_right, value);
        else
        {
            if (root->m_left == nullptr && root->m_right == nullptr)
            {
                delete root;
                root = nullptr;
            }
            else if (root->m_left != nullptr && root->m_right != nullptr)
            {
                auto iop = maxKey(*(root->m_left));
                root->m_value(iop.m_value);
                return remove(node->m_left, iop.m_value);
            }
            else
            {
                auto *child = (root->m_left != nullptr)
                        ? root->m_left : root->m_right;
                root = child;
            }

            return true;
        }
    }

    template <Comparable Tp>
    [[nodiscard]] constexpr bool binary_search_tree<Tp>::pop(const Tp &value)
    { return (!find(value)) false : remove(this->m_root, value); }

}   // namespace nonlinear::tree


#endif //DS_GRAPH_BINARY_SEARCH_TREE_H
