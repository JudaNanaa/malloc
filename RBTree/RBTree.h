#ifndef RBTREE_H
# define RBTREE_H

#include <stdlib.h>

typedef enum e_color
{
	RED,
	BLACK
} t_color;

typedef struct s_rbtree
{
	size_t size;
	t_color	color;
	struct s_rbtree *parent;
	struct s_rbtree *left;
	struct s_rbtree *right;
} t_rbtree;

void insert_node_tree(t_rbtree **root, t_rbtree *to_add);
void print_rbtree_root(const t_rbtree *root);
void delete_node_tree(t_rbtree **root, t_rbtree *to_del);

extern t_rbtree *NIL;

#endif