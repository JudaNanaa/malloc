#include "../../includes/malloc_internal.h"
#include "rbTree.h"

void fix_insert_case_1_2(t_block **node, t_block *uncle)
{
	SET_BLOCK_BLACK(uncle);
	SET_BLOCK_BLACK((*node)->parent);
	SET_BLOCK_RED((*node)->parent->parent);
	*node = (*node)->parent->parent;
}

void fix_insert(t_block **root, t_block *node, t_block *NIL)
{
	t_block *uncle;

	while (IS_BLOCK_RED(node->parent)) // if parent is red, red-red confilct
	{
		if (node->parent == node->parent->parent->left)
		{
			uncle = node->parent->parent->right;
			if (IS_BLOCK_RED(uncle))
				fix_insert_case_1_2(&node, uncle);
			else
			{
				if (node == node->parent->right)
				{
					node = node->parent;
					left_rotation(root, node, NIL);
				}
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_RED(node->parent->parent);
				right_rotation(root, node->parent->parent, NIL);
			}

		}
		else
		{
			uncle = node->parent->parent->left;
			if (IS_BLOCK_RED(uncle))
				fix_insert_case_1_2(&node, uncle);
			else
			{
				if (node == node->parent->left)
				{
					node = node->parent;
					right_rotation(root, node, NIL);
				}
				SET_BLOCK_BLACK(node->parent);
				SET_BLOCK_RED(node->parent->parent);
				left_rotation(root, node->parent->parent, NIL);
			}
		}
	}
	SET_BLOCK_BLACK(*root);
}

void add_to_same_size(t_block *first, t_block *new)
{
    new->same_next = first->same_next;
    if (first->same_next)
        first->same_next->same_prev = new;
    new->same_prev = first;
    COPY_BLOCK_COLOR(new, first);
    first->same_next = new;
}

void insert_node_tree(t_block **root, t_block *to_add, t_block *NIL)
{
	t_block *parent;
	t_block *current;

	parent = NIL;
	current = *root;
	to_add->left = NIL;
	to_add->right = NIL;
	SET_BLOCK_RED(to_add);
	while (current && current != NIL) {
		parent = current;
		if (current->true_size == to_add->true_size)
		{
			add_to_same_size(current, to_add);
			return ;
		}
		else if (to_add->true_size < current->true_size) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
	if (parent == NIL)
		*root = to_add;
	else if (to_add->true_size < parent->true_size)
	{
		parent->left = to_add;
	}
	else {
		parent->right = to_add;
	}
	to_add->parent = parent;
	fix_insert(root, to_add, NIL);
	SET_BLOCK_BLACK(NIL);
}