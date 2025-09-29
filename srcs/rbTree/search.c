#include "../../includes/malloc_internal.h"
#include <stddef.h>

t_block	*search_best_node(t_block *root, size_t size, t_block *NIL)
{
	t_block	*best_fit;
	t_block	*current;

	current = root;
	best_fit = NULL;
	while (current && current != NIL)
	{
		if (size == current->true_size)
			return (current);
		else if (size < current->true_size)
		{
			best_fit = current;
			current = current->left;
		}
		else
			current = current->right;
	}
	return (best_fit);
}
