#include "../../includes/malloc_internal.h"

void left_rotation(t_block **root, t_block *node, t_block *NIL)
{
	t_block *right_node;

	right_node = node->right; // set right_node 

	node->right = right_node->left; // met l'enfant de gauche de right dans le droite de node 
	if (right_node->left != NIL) // si l'enfant n'est pas une leaf
		right_node->left->parent = node; // alors on met son parent a node
	right_node->parent = node->parent;
	if (node->parent == NIL) // si node etait le root
	{
		*root = right_node; // alors right_child devient le root
	}
	else if (node == node->parent->left) {
		node->parent->left = right_node;
	}
	else {
		node->parent->right = right_node;
	}
	right_node->left = node; // on dit que la gauche du right_node sera la node
	node->parent = right_node; // set node.parent
}

void right_rotation(t_block **root, t_block *node, t_block *NIL)
{
	t_block *left_node;

	left_node = node->left; // set left_node 

	node->left = left_node->right; // met l'enfant de gauche de left dans le droite de node 
	if (left_node->right != NIL) // si l'enfant n'est pas une leaf
		left_node->right->parent = node; // alors on met son parent a node
	left_node->parent = node->parent;
	if (node->parent == NIL) // si node etait le root
	{
		*root = left_node; // alors left_child devient le root
	}
	else if (node == node->parent->right) {
		node->parent->right = left_node;
	}
	else {
		node->parent->left = left_node;
	}
	left_node->right = node; // on dit que la gauche du left_node sera la node
	node->parent = left_node; // set node.parent
}
