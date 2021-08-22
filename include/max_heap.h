#ifndef DS_GRAPH_MAX_HEAP_H
#define DS_GRAPH_MAX_HEAP_H


#include "binary_tree.h"


namespace dsl::nonlinear::tree
{
    template <Comparable Tp>
    class max_heap : public binary_tree<Tp>
    {
    public:
        using bitree_node = typename binary_tree<Tp>::bitree_node;


        //****** Member Functions ******//
        // Constructor
        max_heap() noexcept
            : binary_tree<Tp>() {}

        // Copy constructor
        max_heap(const max_heap &rhs)
            : binary_tree<Tp>(rhs) {}

        // Move constructor
        max_heap(max_heap &&rhs) noexcept
            : binary_tree<Tp>(rhs) {}

        // Pass-by-value copy/move assignment
        max_heap& operator=(max_heap rhs) noexcept
        { binary_tree<Tp>::operator=(rhs); return *this; }

        ~max_heap() = default;


        //****** Modifiers ******//
        constexpr void sift_up(const Tp&) noexcept;
        constexpr void sift_down(const Tp&) noexcept;

        constexpr void sift_up(bitree_node &node) noexcept       { try_sift_up(node); }
        constexpr void sift_down(bitree_node &node) noexcept     { try_sift_down(node); }

        constexpr void increase_key(const Tp&, const Tp&) noexcept;
        constexpr void decrease_key(const Tp&, const Tp&) noexcept;

        [[nodiscard]] constexpr const Tp& max() const override { return this->m_root->m_value; }
        constexpr bool push(const Tp&) override;
        constexpr bool pop(const Tp&) override;


    private:
        constexpr void try_sift_up(bitree_node&) noexcept;
        constexpr void try_sift_down(bitree_node&) noexcept;

    };  // class max_heap



    //************ Member Function Implementations ************//

    // sift_up helper function
    template <Comparable Tp>
    constexpr void max_heap<Tp>::try_sift_up(bitree_node &node) noexcept
    {
        auto path = this->pathTo(&node);
        while (!path.empty())
        {
            auto *parent = path.front();
            path.pop();

            if (parent->m_value >= value)
                break;

            // Swap
            auto temp = parent->m_value;
            parent->m_value = node->m_value;
            node.m_value = temp;
            node = *parent;
        }
    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_up(const Tp &value) noexcept
    {
        auto found = this->find(value);
        if (found)
        {
            auto node = *found;
            try_sift_up(node);
        }
    }


    // sift_down helper function
    template <Comparable Tp>
    constexpr void max_heap<Tp>::try_sift_down(bitree_node &node) noexcept
    {
        auto *curr = &node;
        while (curr->m_left != nullptr || curr->m_right != nullptr)
        {
            bitree_node *b = nullptr;
            if (curr->m_left != nullptr && curr->m_right != nullptr)
                b = (curr->m_left->m_value < curr->m_right->m_value) ? curr->m_left : curr->m_right;
            else
                b = (curr->m_left != nullptr) ? curr->m_left : curr->m_right;

            auto temp = b->m_value;
            b->m_value = curr->m_value;
            curr->m_value = temp;
            curr = b;
        }
    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_down(const Tp &value) noexcept
    {
        auto found = this->find(value);
        bitree_node *node = (found) ? &(*found) : nullptr;

        if (node != nullptr)
            try_sift_down(node);
    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::increase_key(const Tp &old_value, const Tp &new_value) noexcept
    {
        if (new_value < old_value)
            throw std::invalid_argument("Argument 'new_value' must be greater than 'old_value'.");

        auto found_p = this->parentOf(old_value);
        if (found_p)
        {
            auto p = *found_p;
            auto *child = (p.m_left->m_value == value) ? p.m_left : p.m_right;
            child->m_value = new_value;
            sift_up(*child);
        }
    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::decrease_key(const Tp &old_value, const Tp &new_value) noexcept
    {
        if (old_value < new_value)
            throw std::invalid_argument("Argument 'new_value' must be less than 'old_value'.");

        auto found_p = this->parentOf(old_value);
        if (found_p)
        {
            auto p = *found_p;
            auto *child = (p.m_left->m_value == value) ? p.m_left : p.m_right;
            child->m_value = new_value;
            sift_down(*child);
        }
    }


    template <Comparable Tp>
    constexpr bool max_heap<Tp>::push(const Tp &value)
    {
        if (!binary_tree<Tp>::push(value)) return false;
        sift_up(value);
    }


    template <Comparable Tp>
    constexpr bool max_heap<Tp>::pop(const Tp &value)
    {
        auto found_n = this->find(value);
        if (!found_n) return false;

        auto *n = *found_n, *last = this->last_level_order();
        n->m_value = last->m_value;
        delete last;
        last = nullptr;

        auto found_p = this->parentOf(*n);
        if (found_p && (*found_p).m_value < n->m_value)
            sift_up(*n);
        else if ((n->m_left != nullptr && n->m_left->m_value > n->m_value) ||
                 (n->m_right != nullptr && n->m_right->m_value > n->m_value))
            sift_down(*n);

        this->m_size--;
        return true;
    }


}   // namespace nonlinear::tree

#endif //DS_GRAPH_MAX_HEAP_H
