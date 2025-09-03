#include "../includes/malloc_internal.h"
#include <sys/mman.h>

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

int	block_free(t_block *block)
{
	int nb_merge;

	nb_merge = 0;
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
		nb_merge++;
	}
	if (block->prev && block->prev->is_free == true) {
		block_merge_with_next(block->prev, block);
		nb_merge++;
	}
	return nb_merge;
}

void remove_page(t_page **pages_list, t_page *page) {
	t_page *current_page;

	
	if (*pages_list == page) {
		*pages_list = page->next;
	}
	else {
		current_page = *pages_list;
		while (current_page->next != page) {
			current_page = current_page->next;
		}
		current_page->next = page->next;
	}
	munmap(page, page->length);
}

int	page_list_free_block(t_page **pages_list, void *ptr)
{
	t_page	*current_page;
	t_block	*block;
	int nb_merge;

	current_page = *pages_list;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr);
		if (block)
		{
			nb_merge = block_free(block);
			current_page->nb_block -= nb_merge;
			if (nb_merge == 0)
				current_page->nb_block_free++;
			if (current_page->nb_block == current_page->nb_block_free) {
				remove_page(pages_list, current_page);
			}
			return (1);
		}
		current_page = current_page->next;
	}
	return (0);
}

void	free(void *ptr)
{
	if (page_list_free_block(&g_malloc.tiny, ptr))
		return ;
	if (page_list_free_block(&g_malloc.small, ptr))
		return ;

	ft_putendl_fd("free(): invalid pointer", STDERR_FILENO);
	abort();
}