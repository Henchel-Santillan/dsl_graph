#ifndef DS_GRAPH_AVL_TREE_H
#define DS_GRAPH_AVL_TREE_H


#include <cmath>
#include "binary_search_tree.h"


namespace dsl::nonlinear::tree
{
    template <Comparable Tp>
    class avl_tree : public binary_search_tree<Tp>
    {
    public:
        using bitree_node = typename binary_search_tree<Tp>::bitree_node;


        //****** Member Functions ******//

        // No-throw Constructor
        avl_tree() noexcept
            : binary_search_tree<Tp>() {}

        // Copy constructor
        avl_tree(const avl_tree &rhs)
            : binary_search_tree<Tp>(rhs) {}

        // Move constructor
        avl_tree(avl_tree &&rhs) noexcept
            : binary_search_tree<Tp>(rhs) {}

        // Pass-by-value copy/move assignment
        avl_tree& operator=(avl_tree rhs) noexcept
        { binary_search_tree<Tp>::operator=(rhs); return *this; }

        ~avl_tree() = default;


        //****** Access ******//
        [[nodiscard]] constexpr int balanceOf(bitree_node*) const noexcept;


        //****** Modifiers ******//
        constexpr bool push(const Tp&) override;
        constexpr bool pop(const Tp&) override;


    private:
        constexpr void rotate_left(bitree_node*, bool);
        constexpr void rotate_right(bitree_node*, bool);
        constexpr void rotate_left_right(bitree_node*, bool);
        constexpr void rotate_right_left(bitree_node*, bool);
        constexpr void update_balance(std::stack<bitree_node*>&) noexcept;

    }; // class avl_tree



    //************ Member Function Implementations ************//


    // Determines left-to-right balance of one of the nodes in the tree
    template <Comparable Tp>
    constexpr int avl_tree<Tp>::balanceOf(bitree_node *const root) const noexcept
    {
        if (root == nullptr) return 0;
        return depth(root->m_left) - depth(root->m_right);
    }


    // Performs left rotation and returns new root at the rotation scope
    template <Comparable Tp>
    constexpr void avl_tree<Tp>::rotate_left(bitree_node *parent, const bool isLeft)
    {
        if (parent == nullptr)
        {
            auto *root = this->m_root;
            auto *temp = root->m_right;
            root->m_right = temp->m_left;
            temp->m_left = root;
            root = temp;
        }
        else
        {
            auto *unb = (isLeft) ? parent->m_left : parent->m_right;
            auto *temp = unb->m_right;
            unb->m_right = temp->m_left;
            temp->m_left = unb;

            if (isLeft)
                parent->m_left = temp;
            else
                parent->m_right = temp;
        }
    }


    // Performs right rotation and returns new root at the rotation scope
    template <Comparable Tp>
    constexpr void avl_tree<Tp>::rotate_right(bitree_node *parent, const bool isLeft)
    {
        if (parent == nullptr)
        {
            auto *root = this->m_root;
            auto *temp = root->m_left;
            root->m_left = temp->m_right;
            temp->m_right = root;
            root = temp;
        }
        else
        {
            auto *unb = (isLeft) ? parent->m_left : parent->m_right;
            auto *temp = unb->m_left;
            unb->m_left = temp->m_right;
            temp->m_right = unb;

            if (isLeft)
                parent->m_left = temp;
            else
                parent->m_right = temp;
        }
    }


    template <Comparable Tp>
    constexpr void avl_tree<Tp>::rotate_left_right(bitree_node *parent, const bool isLeft)
    {
        if (parent == nullptr)
        {
            rotate_left(this->m_root, true);
            rotate_right(nullptr, false);
        }
        else
        {
            auto *child = (isLeft) ? parent->m_left : parent->m_right;
            rotate_left(child, true);
            rotate_right(parent, isLeft);
        }
    }


    template <Comparable Tp>
    constexpr void avl_tree<Tp>::rotate_right_left(bitree_node *parent, const bool isLeft)
    {
        if (parent == nullptr)
        {
            rotate_right(this->m_root, false);
            rotate_left(nullptr, false);
        }
        else
        {
            auto *child = (isLeft) ? parent->m_left : parent->m_right;
            rotate_right(child, isLeft);
            rotate_left(parent, isLeft);
        }
    }


    template <Comparable Tp>
    constexpr void avl_tree<Tp>::update_balance(std::stack<bitree_node*> &s) noexcept
    {
        while (!s.empty())
        {
            bool isLeft = false;
            bitree_node *parent = nullptr, *curr = s.top();
            s.pop();

            if (s.empty())
            {
                parent = s.top();
                isLeft = curr->m_value < parent->m_value;
            }

            if (abs(balanceOf(curr)) > 1)
            {
                if (ld > rd)
                {
                    if (balanceOf(curr->m_left) > 0) rotate_right(parent, isLeft);
                    else rotate_left_right(parent, isLeft);
                }
                else
                {
                    if (balanceOf(curr->m_right) > 0) rotate_right_left(parent, isLeft);
                    else rotate_left(parent, isLeft);
                }
            }
        }
    }


    // Inserts a node with the given value and balances the tree
    template <Comparable Tp>
    constexpr bool avl_tree<Tp>::push(const Tp &value)
    {
        if (!binary_search_tree<Tp>::push(value))
            return false;

        auto s = this->pathTo(value);
        update_balance(s);
        return true;
    }


    // Removes a node with the given value and balances the tree
    template <Comparable Tp>
    constexpr bool avl_tree<Tp>::pop(const Tp &value)
    {
        auto s = this->pathTo(value);
        if (!binary_search_tree<Tp>::pop(value))
            return false;

        update_balance(s);
        return true;
    }

}   // namespace nonlinear::tree


#endif //DS_GRAPH_AVL_TREE_H
