#include "RBTree.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

t_rbtree sentinel = {
	0,
	BLACK,
	NULL,
	NULL,
	NULL,
};

t_rbtree *NIL = &sentinel;

t_rbtree *create_note(size_t size)
{
	t_rbtree *new;

	new = malloc(sizeof(t_rbtree));
	if (new == NULL)
		return NULL;
	new->size = size;
	new->color = RED;
	new->left = NIL;
	new->right = NIL;
	new->parent = NIL;
	return new;
}

int main(void)
{
	t_rbtree *test[8];
	
	test[0] = create_note(10);
	test[1] = create_note(5);
	test[2] = create_note(15);
	test[3] = create_note(3);
	test[4] = create_note(2);
	test[5] = create_note(4);
	test[6] = create_note(20);
	test[7] = create_note(16);


	t_rbtree *root = NULL;

	for (int i = 0; i < 8; i++) {
		insert_node_tree(&root, test[i]);
	}


	print_rbtree_root(root);

	delete_node_tree(&root, test[4]);
}