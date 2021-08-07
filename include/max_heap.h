#ifndef DS_GRAPH_MAX_HEAP_H
#define DS_GRAPH_MAX_HEAP_H


#include "binary_tree.h"


namespace nonlinear::tree
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
        constexpr void sift_up(bitree_node&) noexcept;
        constexpr void sift_down(const Tp&) noexcept;
        constexpr void sift_down(bitree_node&) noexcept;

        constexpr void increase_key(const Tp&, const Tp&) noexcept;
        constexpr void decrease_key(const Tp&, const Tp&) noexcept;

        constexpr const bitree_node& pushpop() noexcept;
        constexpr const bitree_node& poppush() noexcept;

        constexpr const Tp& max() const override { return this->m_root->m_value; }
        constexpr bool push(const Tp&) override;
        constexpr bool pop(const Tp&) override;

    };  // class max_heap



    //************ Member Function Implementations ************//


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_up(const Tp &value) noexcept
    {

    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_up(bitree_node &) noexcept
    {

    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_down(const Tp &) noexcept
    {

    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::sift_down(bitree_node &) noexcept
    {

    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::increase_key(const Tp &, const Tp &) noexcept
    {

    }


    template <Comparable Tp>
    constexpr void max_heap<Tp>::decrease_key(const Tp &, const Tp &) noexcept
    {

    }


    template <Comparable Tp>
    constexpr const bitree_node& max_heap<Tp>::poppush() noexcept
    {

    }


    template <Comparable Tp>
    constexpr const bitree_node & max_heap<Tp>::pushpop() noexcept
    {

    }


    template <Comparable Tp>
    constexpr bool max_heap<Tp>::push(const Tp &)
    {

    }


    template <Comparable Tp>
    constexpr bool max_heap<Tp>::pop(const Tp &)
    {

    }


}   // namespace nonlinear::tree

#endif //DS_GRAPH_MAX_HEAP_H
