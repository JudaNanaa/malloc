
#include "RBTree.h"
#include <stddef.h>
#include <stdio.h>

void left_rotation(t_rbtree **root, t_rbtree *node)
{
	t_rbtree* right_child = node->right;
	node->right = right_child->left;
	if (right_child->left != NIL) {
		right_child->left->parent = node;
	}
	right_child->parent = node->parent;
	if (node->parent == NIL) {
		*root = right_child;
	}
	else if (node == node->parent->left) {
		node->parent->left = right_child;
	}
	else {
		node->parent->right = right_child;
	}
	right_child->left = node;
	node->parent = right_child;
}

void right_rotation(t_rbtree **root, t_rbtree *node)
{
	t_rbtree* left_child = node->left;
	node->left = left_child->right;
	if (left_child->right != NIL) {
		left_child->right->parent = node;
	}
	left_child->parent = node->parent;
	if (node->parent == NIL) {
		*root = left_child;
	}
	else if (node == node->parent->right) {
		node->parent->right = left_child;
	}
	else {
		node->parent->left = left_child;
	}
	left_child->right = node;
	node->parent = left_child;
}

void fix_insert(t_rbtree **root, t_rbtree *node)
{
	t_rbtree *uncle;

	while (node->parent != NIL && node->parent->color == RED)
	{
		if (node->parent == node->parent->parent->left)
		{
			uncle = node->parent->parent->right;
			printf("uncle == %p\n", uncle);
			if (uncle->color == RED)
			{
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			}
			else {
				if (node == node->parent->right)
				{
					node = node->parent;
					left_rotation(root, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				right_rotation(root, node->parent->parent);
			}
		}
		else
		{
			uncle = node->parent->parent->left;
			if (uncle->color == RED)
			{
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			}
			else {
				if (node == node->parent->left)
				{
					node = node->parent;
					right_rotation(root, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				left_rotation(root, node->parent->parent);
			}
		}

	}
	(*root)->color = BLACK;
}

void insert_node_tree(t_rbtree **root, t_rbtree *to_add)
{
    t_rbtree *current = *root;
    t_rbtree *parent = NIL;

    to_add->left = to_add->right = to_add->parent = NIL;
    while (current && current != NIL)
    {
        parent = current;
        if (current->size == to_add->size)
            return; // TODO: gérer liste chaînée de même taille
        else if (to_add->size < current->size)
            current = current->left;
        else
            current = current->right;
    }
    to_add->parent = parent;
    if (parent == NIL)
        *root = to_add;
    else if (to_add->size < parent->size)
        parent->left = to_add;
    else
        parent->right = to_add;

    to_add->color = RED; /* convention d'insertion */
    fix_insert(root, to_add);
}

void transplant(t_rbtree **root, t_rbtree *u, t_rbtree *v)
{
    if (u->parent == NIL)
        *root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v != NIL)
        v->parent = u->parent;
}

t_rbtree *minimum_in_tree(t_rbtree *node)
{
    if (node == NIL)
        return NIL;
    while (node->left != NIL)
        node = node->left;
    return node;
}


void fix_delete(t_rbtree **root, t_rbtree *node)
{
    while (node != *root && node->color == BLACK)
    {
        if (node->parent != NIL && node == node->parent->left)
        {
            t_rbtree *sibling = node->parent->right;

            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                left_rotation(root, node->parent);
                sibling = node->parent->right;
            }

            if (sibling->left->color == BLACK && sibling->right->color == BLACK)
            {
                sibling->color = RED;
                node = node->parent;
            }
            else {
                if (sibling->right->color == BLACK) {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotation(root, sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->right->color = BLACK;
                left_rotation(root, node->parent);
                node = *root;
            }
        }
        else if (node->parent != NIL) // côté droit
        {
            t_rbtree *sibling = node->parent->left;

            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                right_rotation(root, node->parent);
                sibling = node->parent->left;
            }

            if (sibling->right->color == BLACK && sibling->left->color == BLACK)
            {
                sibling->color = RED;
                node = node->parent;
            }
            else {
                if (sibling->left->color == BLACK) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotation(root, sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->left->color = BLACK;
                right_rotation(root, node->parent);
                node = *root;
            }
        }
        else {
            break; // sécurité si node->parent == NIL
        }
    }
    node->color = BLACK;
}


void delete_node_tree(t_rbtree **root, t_rbtree *to_del)
{
	t_rbtree *target_node;
	t_rbtree *child;
	t_color original_color;

	target_node = to_del;
	original_color = to_del->color;
	if (to_del->left == NIL)
	{
		child = to_del->right;
		transplant(root, to_del, to_del->right);
	}
	else if (to_del->right == NIL) {
		child = to_del->left;
		transplant(root, to_del, to_del->left);
	}
	else {
		target_node = minimum_in_tree(to_del->right);
		original_color = target_node->color;
		child = target_node->right;
		if (target_node->parent == to_del)
			child->parent = target_node;
		else
		{
			transplant(root, target_node, target_node->right);
			target_node->right = to_del->right;
			target_node->right->parent = target_node;
		}
		transplant(root, to_del, target_node);
		target_node->left = to_del->left;
		target_node->left->parent = target_node;
		target_node->color = to_del->color;
	}
	printf("child == %p\n", child);
	if (original_color == BLACK)
		fix_delete(root, child);
}

t_rbtree *search_best_node(t_rbtree *root, size_t size)
{
	t_rbtree *best_fit;
	t_rbtree *current;

	current = root;
	best_fit = NULL;
	while (current != NIL)
	{
		if (size == current->size)
			return current;
		else if (size < current->size) {
			best_fit = current;
			current = current->left;
		}
		else
			current = current->right;
	}
	return best_fit;
}
