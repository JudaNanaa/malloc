#ifndef RBTREE_H
#define RBTREE_H

#include "../../includes/malloc_internal.h"

void left_rotation(t_block **root, t_block *node, t_block *NIL);
void right_rotation(t_block **root, t_block *node, t_block *NIL);


#endif