#ifndef DS_GRAPH_RB_TREE_H
#define DS_GRAPH_RB_TREE_H


#include "binary_search_tree.h"


namespace dsl::nonlinear::tree
{
    namespace details
    {
        enum class Color { red, black };


        template <Comparable Tp>
        struct rbtree_node : public bitree_node<Tp>
        {
            using enum Color;
            constexpr rbtree_node(const Tp &value, const Color &color) noexcept
                : bitree_node<Tp>(value),
                  m_parent(nullptr),
                  m_color(color)  {}

            constexpr rbtree_node& operator=(const rbtree_node &rhs) noexcept
            {
                if (this != &rhs)
                {
                    binary_tree<Tp>::operator=(rhs);
                    rhs.m_parent = m_parent;
                    rhs.m_color = m_color;
                }
                return *this;
            }

            ~rbtree_node() noexcept = default;

            constexpr void recolor() noexcept
            {  rhs.m_color = (rhs.m_color == Color::red) ? Color::black : Color::red; }


            rbtree_node *m_parent;
            Color m_color;

        };  // struct rbtree_node

    }   // namespace details


}   // namespace dsl::nonlinear::tree


#endif //DS_GRAPH_RB_TREE_H
