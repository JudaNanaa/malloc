#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>

void	block_merge_with_next(t_block *first, t_block *second)
{
	size_t total_size;

	total_size = GET_BLOCK_SIZE(first) + BLOCK_HEADER_SIZE + GET_BLOCK_SIZE(second);

	SET_BLOCK_SIZE(first, total_size);
	if (IS_BLOCK_LAST(second))
		SET_BLOCK_LAST(first);
}

void block_free(t_block *block)
{
	if (IS_BLOCK_FREE(block) == true)
	{
		ft_putendl_fd("free(): double free detected in tcache 2",
						STDERR_FILENO);
		abort();
	}
	SET_BLOCK_FREE(block);
}

int merge_block(t_block *block, t_block *prev_block) {
	int	nb_merge;
	t_block *next_block;

	nb_merge = 0;
	next_block = NEXT_BLOCK(block);
	if (next_block && IS_BLOCK_FREE(next_block) == true)
	{
		block_merge_with_next(block, next_block);
		nb_merge++;
	}
	if (prev_block && IS_BLOCK_FREE(prev_block) == true)
	{
		block_merge_with_next(prev_block, block);
		nb_merge++;
	}
	return nb_merge;
}

void	remove_page(t_page **pages_list, t_page *page)
{
	t_page	*current_page;

	if (*pages_list == page)
		*pages_list = page->next;
	else
	{
		current_page = *pages_list;
		while (current_page->next != page)
			current_page = current_page->next;
		current_page->next = page->next;
	}
	munmap(page, page->length);
}

int	page_list_free_block(t_page **pages_list, void *ptr)
{
	t_page	*current_page;
	t_block	*block;
	t_block	*prev_block;

	current_page = *pages_list;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr, &prev_block);
		if (block)
		{
			block_free(block);
			if (merge_block(block, prev_block) == 0)
				current_page->nb_block_free++;
			if (IS_BLOCK_LAST(current_page->blocks) && IS_BLOCK_FREE(current_page->blocks) == true)
				remove_page(pages_list, current_page);
			return (1);
		}
		current_page = current_page->next;
	}
	return (0);
}

void	free(void *ptr)
{
	if (ptr == NULL)
		return ;
	if (page_list_free_block(&g_malloc.tiny, ptr))
		return ;
	if (page_list_free_block(&g_malloc.small, ptr))
		return ;

	ft_putendl_fd("free(): invalid pointer", STDERR_FILENO);
	abort();
}