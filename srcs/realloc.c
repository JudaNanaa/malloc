#include "../includes/malloc_internal.h"

void	*need_to_reallocate(void *ptr, size_t size, size_t previous_size)
{
	void	*new_ptr;

	new_ptr = malloc_internal(size);
	if (!new_ptr)
		return (NULL);
	memmove(new_ptr, ptr, previous_size);
	free_internal(ptr);
	return (new_ptr);
}

void	*shrink_memory(size_t size, t_page_block *page_block)
{
	if (split_block(page_block->block, size) == 0)
		page_block->page->nb_block_free--;
	return (GET_BLOCK_PTR(page_block->block));
}

int	increase_memory(t_page_block *page_block, size_t size)
{
	size_t	new_size;
	size_t	total;
	t_block	*new_free_block;
	t_block	*next_block;

	next_block = NEXT_BLOCK(page_block->block);
	if (next_block == NULL)
		return (0);
	if (IS_BLOCK_FREE(next_block) == false)
		return (0);
	total = GET_BLOCK_SIZE(page_block->block) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(next_block);
	if (total < size)
		return (0);
	new_size = total - ALIGN(size) - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(page_block->block, size);
	new_free_block = (void *)page_block->block + BLOCK_HEADER_SIZE + ALIGN(size);
	memmove(new_free_block, next_block, BLOCK_HEADER_SIZE);
	SET_BLOCK_SIZE(new_free_block, new_size);
	page_block->page->nb_block_free++;
	return (1);
}

void	*realloc_block(void *ptr, size_t new_size, t_page_block *page_block,
		size_t min_size, size_t max_size)
{
	size_t	old_size;

	old_size = page_block->block->size;
	if (new_size == old_size)
		return (ptr);
	if (new_size < old_size)
		return (shrink_memory(new_size, page_block));
	if (new_size >= min_size && new_size <= max_size)
	{
		if (increase_memory(page_block, new_size))
			return (GET_BLOCK_PTR(page_block->block));
	}
	return (need_to_reallocate(ptr, new_size, old_size));
}

void	*realloc_tiny_block(void *ptr, size_t size, t_page_block *page_block)
{
	return (realloc_block(ptr, size, page_block, 1, n));
}

void	*realloc_small_block(void *ptr, size_t size, t_page_block *page_block)
{
	return (realloc_block(ptr, size, page_block, n + 1, m));
}

void	*realloc_large_block(void *ptr, size_t size, t_page_block *page_block)
{
	return (realloc_block(ptr, size, page_block, m + 1, SIZE_MAX));
}

void	*realloc_internal(void *ptr, size_t size)
{
	t_page_block res;
	void	*new_ptr;

	if (ptr == NULL)
		return (malloc_internal(size));
	else if (size == 0)
	{
		free_internal(ptr);
		return (NULL);
	}
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	if (find_block(g_malloc.tiny.pages, ptr, &res))
	{
		new_ptr = realloc_tiny_block(ptr, size, &res);
		pthread_mutex_unlock(&g_malloc.tiny.mutex);
	}
	else
	{
		pthread_mutex_unlock(&g_malloc.tiny.mutex);
		pthread_mutex_lock(&g_malloc.small.mutex);
		if (find_block(g_malloc.small.pages, ptr, &res))
		{
			new_ptr = realloc_small_block(ptr, size, &res);
			pthread_mutex_unlock(&g_malloc.small.mutex);
			
		}
		else
		{
			pthread_mutex_unlock(&g_malloc.small.mutex);
			pthread_mutex_lock(&g_malloc.large.mutex);
			if (find_block(g_malloc.large.pages, ptr, &res))
			{
				new_ptr = realloc_large_block(ptr, size, &res);
				pthread_mutex_unlock(&g_malloc.large.mutex);
			}
			else
			{
				pthread_mutex_unlock(&g_malloc.large.mutex);
				ft_putendl_fd("realloc(): invalid pointer", STDERR_FILENO);
				abort();
				return (NULL);
			}
		}
	}
	return (new_ptr);
}

void *realloc(void *ptr, size_t size)
{
    void *new_ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
    new_ptr = realloc_internal(ptr, size);
    if (g_malloc.verbose)
    {
        ft_printf_fd(STDERR_FILENO, "[DEBUG] realloc(%p, %u) -> %p\n", ptr, size, new_ptr);
        ft_printf_fd(STDERR_FILENO, "Stack trace (most recent first):\n");
    }

    if (g_malloc.trace_file_fd != -1)
    {
        ft_printf_fd(g_malloc.trace_file_fd,
                     "realloc(%p, %u) -> %p\n",
                     ptr,
                     size,
                     new_ptr);
    }

    return new_ptr;
}
