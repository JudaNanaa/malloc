#include "../includes/malloc_internal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void	left_rotation(t_block **root, t_block *node)
{
	t_block	*right_child;

	right_child = node->right;
	node->right = right_child->left;
	if (right_child->left != g_malloc.NIL)
		right_child->left->parent = node;
	right_child->parent = node->parent;
	if (node->parent == NULL)
		*root = right_child;
	else if (node == node->parent->left)
		node->parent->left = right_child;
	else
		node->parent->right = right_child;
	right_child->left = node;
	node->parent = right_child;
}

void	right_rotation(t_block **root, t_block *node)
{
	t_block	*left_child;

	left_child = node->left;
	node->left = left_child->right;
	if (left_child->right != g_malloc.NIL)
		left_child->right->parent = node;
	left_child->parent = node->parent;
	if (node->parent == NULL)
		*root = left_child;
	else if (node == node->parent->right)
		node->parent->right = left_child;
	else
		node->parent->left = left_child;
	left_child->right = node;
	node->parent = left_child;
}

void	fix_insert(t_block **root, t_block *node)
{
	t_block	*uncle;

	while (node->parent && IS_BLOCK_RED(node->parent))
	{
		if (node->parent == node->parent->parent->left)
		{
			uncle = node->parent->parent->right;
			if (IS_BLOCK_RED(uncle))
			{
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_BLACK(uncle);
				SET_BLOCK_RED(node->parent->parent);
				node = node->parent->parent;
			}
			else
			{
				if (node == node->parent->right)
				{
					node = node->parent;
					left_rotation(root, node);
				}
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_RED(node->parent->parent);
				right_rotation(root, node->parent->parent);
			}
		}
		else
		{
			uncle = node->parent->parent->left;
			if (IS_BLOCK_RED(uncle))
			{
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_BLACK(uncle);
				SET_BLOCK_RED(node->parent->parent);
				node = node->parent->parent;
			}
			else
			{
				if (node == node->parent->left)
				{
					node = node->parent;
					right_rotation(root, node);
				}
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_RED(node->parent->parent);
				left_rotation(root, node->parent->parent);
			}
		}
	}
	SET_BLOCK_BLACK(*root);
}

void add_to_same_size(t_block **first, t_block *new)
{
	new->same_next = *first;
	new->same_prev = NULL;
	COPY_BLOCK_COLOR(new, (*first));
	*first = new;
}

void	insert_node_tree(t_block **root, t_block *to_add)
{
	t_block	*current;
	t_block	*parent;

	current = *root;
	parent = NULL;
	to_add->left = to_add->right = g_malloc.NIL;
	while (current && current != g_malloc.NIL)
	{
		parent = current;
		if (current->size == to_add->size)
		{
			add_to_same_size(&current, to_add);
			return ;
		}
		else if (to_add->size < current->size)
			current = current->left;
		else
			current = current->right;
	}
	to_add->parent = parent;
	if (parent == NULL)
		*root = to_add;
	else if (to_add->size < parent->size)
		parent->left = to_add;
	else
		parent->right = to_add;
	SET_BLOCK_RED(to_add);
	// to_add->color = RED;
	fix_insert(root, to_add);
}

void	transplant(t_block **root, t_block *u, t_block *v)
{
	if (u->parent == NULL)
		*root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

t_block	*minimum_in_tree(t_block *node)
{
	while (node->left != g_malloc.NIL)
		node = node->left;
	return (node);
}

void	fix_delete(t_block **root, t_block *node)
{
	t_block	*sibling;

	while (node != *root && IS_BLOCK_BLACK(node))
	{
		if (node == node->parent->left)
		{
			sibling = node->parent->right;
			if (IS_BLOCK_RED(sibling))
			{
				SET_BLOCK_BLACK(sibling);
				SET_BLOCK_RED(node->parent);
				left_rotation(root, node->parent);
				sibling = node->parent->right;
			}
			if (IS_BLOCK_BLACK(sibling->left) && IS_BLOCK_BLACK(sibling->left))
			{
				SET_BLOCK_RED(sibling);
				node = node->parent;
			}
			else
			{
				if (IS_BLOCK_BLACK(sibling->right))
				{
					SET_BLOCK_BLACK(sibling->left);
					SET_BLOCK_RED(sibling);
					right_rotation(root, sibling);
					sibling = node->parent->right;
				}
				COPY_BLOCK_COLOR(sibling, node->parent);
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_BLACK(sibling->right);
				left_rotation(root, node->parent);
				node = *root;
			}
		}
		else
		{
			sibling = node->parent->left;
			if (IS_BLOCK_RED(sibling))
			{
				SET_BLOCK_BLACK(sibling);
				SET_BLOCK_RED(node->parent);
				right_rotation(root, node->parent);
				sibling = node->parent->left;
			}
			if (IS_BLOCK_BLACK(sibling->right) && IS_BLOCK_BLACK(sibling->right))
			{
				SET_BLOCK_RED(sibling);
				node = node->parent;
			}
			else
			{
				if (IS_BLOCK_BLACK(sibling->left))
				{
					SET_BLOCK_BLACK(sibling->right);
					SET_BLOCK_RED(sibling);
					left_rotation(root, sibling);
					sibling = node->parent->left;
				}
				COPY_BLOCK_COLOR(sibling, node->parent);
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_BLACK(sibling->left);
				right_rotation(root, node->parent);
				node = *root;
			}
		}
	}
	SET_BLOCK_BLACK(node);
}

void remove_front_same(t_block *to_del)
{
	t_block *next;

	next = to_del->same_next;
	COPY_BLOCK_COLOR(next, to_del);
	next->left = to_del->left;
	next->right = to_del->right;
	next->parent = to_del->parent;
	next->same_prev = to_del->same_next;
	if (to_del->same_prev)
		to_del->same_prev->same_next = next;
}

void remove_in_list(t_block *to_del)
{
	t_block *next;
	t_block *prev;

	prev = to_del->same_prev;
	next = to_del->same_next;
	if (prev)
		prev->same_next = next;
	if (next)
		next->same_prev = prev;
}

void	delete_node_tree(t_block **root, t_block *to_del)
{
	t_block	*target_node;
	t_block	*child;
	bool original_color;

	t_block *x = search_best_node(*root, to_del->size);
	
	if (x != to_del)
	{
		remove_in_list(to_del);
		return;
	}
	if (x == to_del && to_del->same_next)
	{
		remove_front_same(to_del);
		return;
	}
	target_node = to_del;
	original_color = IS_BLOCK_BLACK(target_node);
	if (to_del->left == g_malloc.NIL)
	{
		child = to_del->right;
		transplant(root, to_del, to_del->right);
	}
	else if (to_del->right == g_malloc.NIL)
	{
		child = to_del->left;
		transplant(root, to_del, to_del->left);
	}
	else
	{
		target_node = minimum_in_tree(to_del->right);
		original_color = IS_BLOCK_BLACK(target_node);
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
		COPY_BLOCK_COLOR(target_node, to_del);
	}
	if (original_color)
		fix_delete(root, child);
}

t_block	*search_best_node(t_block *root, size_t size)
{
	t_block	*best_fit;
	t_block	*current;

	current = root;
	best_fit = NULL;
	while (current && current != g_malloc.NIL)
	{
		if (size == current->size)
			return (current);
		else if (size < current->size)
		{
			best_fit = current;
			current = current->left;
		}
		else
			current = current->right;
	}
	return (best_fit);
}
