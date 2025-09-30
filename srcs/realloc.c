#include "../includes/malloc_internal.h"

void	*need_to_reallocate(void *ptr, size_t size, size_t previous_size,
		t_mutex_zone *zone)
{
	void	*new_ptr;

	pthread_mutex_unlock(&zone->mutex);
	new_ptr = malloc_internal(size);
	if (!new_ptr)
		return (NULL);
	memmove(new_ptr, ptr, previous_size);
	free_internal(ptr);
	return (new_ptr);
}

void	*shrink_memory(size_t size, t_block *block,
		t_mutex_zone *zone)
{
	split_block(block, size, zone);
	pthread_mutex_unlock(&zone->mutex);
	return (GET_BLOCK_PTR(block));
}

bool	try_expand_block(t_block *block, size_t size, t_mutex_zone *zone)
{
	size_t	total;
	t_block	*next_block;

	next_block = NEXT_BLOCK(block);
	if (next_block == NULL || IS_BLOCK_FREE(next_block) == false)
		return (false);
	total = GET_BLOCK_TRUE_SIZE(block) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_TRUE_SIZE(next_block);
	if (total < size)
		return (false);
	merge_block_with_next(zone, block);
	split_block(block, size, zone);
	return (true);
}

void	*realloc_block(void *ptr, size_t new_size, t_block *block,
		size_t min_size, size_t max_size, t_mutex_zone *zone)
{
	size_t	old_size;

	old_size = block->size;
	if (new_size == old_size)
		return (pthread_mutex_unlock(&zone->mutex), ptr);
	if (new_size < old_size)
		return (shrink_memory(new_size, block, zone));
	if (new_size >= min_size && new_size <= max_size)
	{
		if (try_expand_block(block, new_size, zone))
			return (pthread_mutex_unlock(&zone->mutex),
				GET_BLOCK_PTR(block));
	}
	return (need_to_reallocate(ptr, new_size, old_size, zone));
}

void	*realloc_tiny_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, 1, g_malloc.tiny.max_size_malloc,
			&g_malloc.tiny));
}

void	*realloc_small_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, g_malloc.tiny.max_size_malloc + 1,
			g_malloc.small.max_size_malloc, &g_malloc.small));
}

void	*realloc_large_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, g_malloc.small.max_size_malloc + 1,
			SIZE_MAX, &g_malloc.large));
}

void	*realloc_internal(void *ptr, size_t size)
{
	t_block *block;
	void			*new_ptr;

	if (ptr == NULL)
		return (malloc_internal(size));
	else if (size == 0)
	{
		free_internal(ptr);
		return (NULL);
	}
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	if (find_block(g_malloc.tiny.pages, ptr, &block))
		new_ptr = realloc_tiny_block(ptr, size, block);
	else
	{
		pthread_mutex_unlock(&g_malloc.tiny.mutex);
		pthread_mutex_lock(&g_malloc.small.mutex);
		if (find_block(g_malloc.small.pages, ptr, &block))
			new_ptr = realloc_small_block(ptr, size, block);
		else
		{
			pthread_mutex_unlock(&g_malloc.small.mutex);
			pthread_mutex_lock(&g_malloc.large.mutex);
			if (find_block(g_malloc.large.pages, ptr, &block))
				new_ptr = realloc_large_block(ptr, size, block);
			else
			{
				pthread_mutex_unlock(&g_malloc.large.mutex);
				ft_printf_fd(STDERR_FILENO, "realloc(): invalid pointer");
				abort();
				return (NULL);
			}
		}
	}
	return (new_ptr);
}

__attribute__((visibility("default")))
void	*REALLOC_NAME(void *ptr, size_t size)
{
	void	*new_ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
	new_ptr = realloc_internal(ptr, size);
	if (g_malloc.verbose)
	{
		ft_printf_fd(STDERR_FILENO, "[DEBUG] realloc(%p, %u) -> %p\n", ptr,
				size, new_ptr);
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
	return (new_ptr);
}
