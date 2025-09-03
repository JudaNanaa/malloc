

#include "../includes/malloc_internal.h"
#include <stdbool.h>

t_block	*find_block_in_page(t_page *page, void *ptr)
{
	int	i;

	i = 0;
	while (i < NB_BLOCK)
	{
		if (page->blocks[i].ptr == ptr)
		{
			return (&page->blocks[i]);
		}
		i++;
	}
	return (NULL);
}

int	find_in_page_list_to_free(t_page *first_page, void *ptr)
{
	t_page	*current_page;
	t_block	*block_with_ptr;

	current_page = first_page;
	while (current_page)
	{
		block_with_ptr = find_block_in_page(current_page, ptr);
		if (block_with_ptr)
		{
			if (block_with_ptr->is_free == true)
			{
				
				abort(); // TODO changer ca pour le double free
			}
			block_with_ptr->size = 0;
			block_with_ptr->is_free = true;
			return (1);
		}
		current_page = current_page->next;
	}
	return (0);
}

void	free(void *ptr)
{
	if (find_in_page_list_to_free(g_malloc.tiny, ptr))
		return ;
	if (find_in_page_list_to_free(g_malloc.small, ptr))
		return ;
	abort(); // TODO changer pour le invalid pointer
}