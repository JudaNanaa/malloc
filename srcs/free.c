#include "../includes/malloc_internal.h"

t_block	*page_find_block_by_ptr(t_page *page, void *ptr)
{
	t_block *current_block;

	current_block = page->blocks;
	while (current_block)
	{

		if (current_block->ptr == ptr)
		{
			return (current_block);
		}
		current_block = current_block->next;
	}
	return (NULL);
}

void block_merge_with_next(t_block *first, t_block *second) {
	first->next = second->next;
	first->size += second->size + sizeof(t_block);
}

void	block_free(t_block *block)
{
	if (block->is_free == true)
	{
		ft_putendl_fd("free(): double free detected in tcache 2",
						STDERR_FILENO);
		abort();
	}
	block->size = 0;
	block->is_free = true;
	if (block->next && block->next->is_free == true) {
		block_merge_with_next(block, block->next);
	}
	if (block->prev && block->prev->is_free == true) {
		block_merge_with_next(block->prev, block);
	}
}

int	page_list_free_block(t_page *pages, void *ptr)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr);
		if (block)
		{
			block_free(block);
			return (1);
		}
		current_page = current_page->next;
	}
	return (0);
}

void	free(void *ptr)
{
	if (page_list_free_block(g_malloc.tiny, ptr))
		return ;
	if (page_list_free_block(g_malloc.small, ptr))
		return ;

	ft_putendl_fd("free(): invalid pointer", STDERR_FILENO);
	abort();
}