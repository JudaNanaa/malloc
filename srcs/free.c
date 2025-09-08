#include "../includes/malloc_internal.h"

void	block_free(t_block *block)
{
	if (IS_BLOCK_FREE(block) == true)
	{
		ft_putendl_fd("free(): double free detected in tcache 2",
						STDERR_FILENO);
		abort();
	}
	SET_BLOCK_FREE(block);
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
		{
			current_page = next_page(current_page);
			// current_page = current_page->next;
		}
		current_page->next = page->next;
	}
	munmap(page, page->length);
}

bool	is_all_blocks_free(t_block *blocks)
{
	t_block	*current_block;

	current_block = blocks;
	while (current_block)
	{
		if (IS_BLOCK_FREE(current_block) == false)
			return (false);
		current_block = NEXT_BLOCK(current_block);
	}
	return (true);
}

int	free_block_from_zone(t_page **pages_list, void *ptr)
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
			if (is_all_blocks_free(current_page->blocks) == true)
				remove_page(pages_list, current_page);
			return (1);
		}
		current_page = next_page(current_page);
		// current_page = current_page->next;
	}
	return (0);
}

int	free_large_block(void *ptr)
{
	t_page	*current_page;

	current_page = g_malloc.large;
	while (current_page)
	{
		if (page_find_block_by_ptr(current_page, ptr, NULL) != NULL)
		{
			remove_page(&g_malloc.large, current_page);
			return (1);
		}
		current_page = next_page(current_page);
		// current_page = current_page->next;
	}
	return (0);
}

void	free_internal(void *ptr)
{
	if (ptr == NULL)
		return ;
	if (free_block_from_zone(&g_malloc.tiny, ptr))
		return ;
	if (free_block_from_zone(&g_malloc.small, ptr))
		return ;
	if (free_large_block(ptr))
		return ;
	ft_putendl_fd("free(): invalid pointer", STDERR_FILENO);
	abort();
}

void	free(void *ptr)
{
	if (g_malloc.verbose)
	{
		if (ptr == NULL)
			ft_printf_fd(STDERR_FILENO,
							"[DEBUG] free(NULL)\n");
		else
			ft_printf_fd(STDERR_FILENO,
							"[DEBUG] free(%p)\n",
							ptr);
	}
	if (g_malloc.trace_file_fd != -1)
	{
		if (ptr == NULL)
			ft_printf_fd(g_malloc.trace_file_fd, "free(NULL)\n");
		else
			ft_printf_fd(g_malloc.trace_file_fd, "free(%p)\n",
					ptr);
	}
	free_internal(ptr);
}
