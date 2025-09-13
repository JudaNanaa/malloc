#include "../includes/malloc_internal.h"
#include <unistd.h>

void double_free(void)
{
	ft_putendl_fd("free(): double free detected in tcache 2",
					STDERR_FILENO);
	abort();
}

void invalid_pointer(void)
{
	ft_putendl_fd("free(): invalid pointer", STDERR_FILENO);
	abort();
}

void	block_free(t_free_list *free_lists, t_block *block)
{
	if (IS_BLOCK_FREE(block) == true)
		double_free();
	add_block_to_free_list(free_lists, block);
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
			current_page = current_page->next;
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

bool ptr_is_in_page(t_page *page, void *ptr)
{
    return ptr > (void *)page && ptr < (void *)((char *)page + page->length);
}

int	free_block_from_zone(t_page **pages_list, void *ptr)
{
	t_page	*current_page;
	t_block	*block;
	t_block	*prev_block;
	
	current_page = *pages_list;
	while (current_page)
	{
		if (ptr_is_in_page(current_page, ptr))
		{
			block = page_find_block_by_ptr(current_page, ptr, &prev_block);
			if (block)
			{
				block_free(&current_page->free_lists, block);
				merge_block(current_page, block, prev_block);
				if (is_all_blocks_free(current_page->blocks) == true)
					remove_page(pages_list, current_page);
				return (1);
			}
			invalid_pointer();
		}
		current_page = current_page->next;
	}
	return (0);
}

int	free_large_block(void *ptr)
{
	t_page	*current_page;

	current_page = g_malloc.large.pages;
	while (current_page)
	{
		if (page_find_block_by_ptr(current_page, ptr, NULL) != NULL)
		{
			remove_page(&g_malloc.large.pages, current_page);
			return (1);
		}
		current_page = current_page->next;
	}
	return (0);
}

void	free_internal(void *ptr)
{
	if (ptr == NULL)
		return ;
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	if (free_block_from_zone(&g_malloc.tiny.pages, ptr))
		return (void)pthread_mutex_unlock(&g_malloc.tiny.mutex);
	pthread_mutex_unlock(&g_malloc.tiny.mutex);
	pthread_mutex_lock(&g_malloc.small.mutex);
	if (free_block_from_zone(&g_malloc.small.pages, ptr))
		return (void)pthread_mutex_unlock(&g_malloc.small.mutex);
	pthread_mutex_unlock(&g_malloc.small.mutex);
	pthread_mutex_lock(&g_malloc.large.mutex);
	if (free_large_block(ptr))
		return (void)pthread_mutex_unlock(&g_malloc.large.mutex);
	pthread_mutex_unlock(&g_malloc.large.mutex);
	invalid_pointer();
}

void	free(void *ptr)
{
	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
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
