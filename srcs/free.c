#include "../includes/malloc_internal.h"

void double_free(void)
{
	ft_printf_fd(STDERR_FILENO, "free(): double free detected in tcache 2");
	abort();
}

void invalid_pointer(void)
{
	ft_printf_fd(STDERR_FILENO, "free(): invalid pointer");
	abort();
}

bool is_ptr_in_page(t_page *page, void *ptr)
{
	return ptr >= GET_BLOCK_PTR(page->blocks) && ptr < (void *)page + page->length;
}

void free_the_block(t_block *block)
{
	if (IS_BLOCK_free(block) == true)
		double_free();
	SET_BLOCK_free(block);
}

bool is_all_block_free(t_page *page)
{
	t_block *current_block;

	current_block = page->blocks;
	while (current_block) {
		if (IS_BLOCK_free(current_block) == false)
			return false;
		current_block = NEXT_BLOCK(current_block);
	}
	return true;
}

void remove_page(t_mutex_zone *zone, t_page *page)
{
	if (zone->pages == page)
		zone->pages = page->next;
	if (zone->last == page)
		zone->last = page->prev;
	if (page->next)
		page->next->prev = page->prev;
	if (page->prev)
		page->prev->next = page->next;
}

bool	free_block_from_zone(t_mutex_zone *zone, void *ptr)
{
	t_page *current_page;
	t_block *block;
	t_block *prev_block;

	current_page = zone->pages;
	while (current_page) {
		if (is_ptr_in_page(current_page, ptr))
		{
			block = page_find_block_by_ptr(current_page, ptr, &prev_block);
			if (block)
			{
				free_the_block(block);
				merge_block_with_next(&current_page->free_lists, block);
				merge_block_with_prev(&current_page->free_lists, &block, prev_block);
				add_block_to_free_list(&current_page->free_lists, block);
				if (is_all_block_free(current_page) == true)
					remove_page(zone, current_page);
				return true;
			}
			invalid_pointer();
		}
		current_page = current_page->next;
	}
	return false;
}

bool free_large_block(void *ptr)
{
	t_page *current_page;
	t_block *block;

	current_page = g_malloc.large.pages;
	while (current_page) {
		if (is_ptr_in_page(current_page, ptr))
		{
			block = page_find_block_by_ptr(current_page, ptr, NULL);
			if (block)
			{
				remove_page(&g_malloc.large, current_page);
				return true;
			}
			invalid_pointer();
		}
		current_page = current_page->next;
	}
	return false;
}

static bool try_free_zone(t_mutex_zone *zone, void *ptr)
{
    bool freed;

    pthread_mutex_lock(&zone->mutex);
    freed = free_block_from_zone(zone, ptr);
    pthread_mutex_unlock(&zone->mutex);

    return freed;
}

void free_internal(void *ptr)
{
    if (ptr == NULL)
        return;

    if (try_free_zone(&g_malloc.tiny, ptr))
        return;

    if (try_free_zone(&g_malloc.small, ptr))
        return;

    pthread_mutex_lock(&g_malloc.large.mutex);
    if (free_large_block(ptr)) {
        pthread_mutex_unlock(&g_malloc.large.mutex);
        return;
    }
    pthread_mutex_unlock(&g_malloc.large.mutex);
    // invalid_pointer();
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
							"[DEBUG] free(NULL) by pid == %d\n", getpid());
		else
			ft_printf_fd(STDERR_FILENO,
							"[DEBUG] free(%p) by pid == %d\n",
							ptr, getpid());
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
