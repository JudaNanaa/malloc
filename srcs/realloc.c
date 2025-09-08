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

void	*shrink_memory(t_page *pages, size_t size, t_block *block)
{
	t_page	*current_page;

	if (split_block(block, size) == 0)
	{
		current_page = find_page_by_block(pages, block);
		decr_nb_free_block_in_page(current_page);
		// current_page->nb_block_free--;
	}
	SET_BLOCK_SIZE(block, size);
	return (GET_BLOCK_PTR(block));
}

int	increase_memory(t_page *pages, t_block *block, size_t size)
{
	size_t	new_size;
	size_t	total;
	t_block	*new_free_block;
	t_block	*next_block;
	t_page	*current_page;

	next_block = NEXT_BLOCK(block);
	if (next_block == NULL)
		return (0);
	if (IS_BLOCK_FREE(next_block) == false)
		return (0);
	total = GET_BLOCK_SIZE(block) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(next_block);
	if (total < size)
		return (0);
	new_size = total - ALIGN(size) - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(block, size);
	new_free_block = (void *)block + BLOCK_HEADER_SIZE + ALIGN(size);
	memmove(new_free_block, next_block, BLOCK_HEADER_SIZE);
	SET_BLOCK_SIZE(new_free_block, new_size);
	current_page = find_page_by_block(pages, block);
	incr_nb_free_block_in_page(current_page);
	// current_page->nb_block_free++;
	return (1);
}

void	*realloc_block(void *ptr, size_t new_size, t_block *block,
		size_t min_size, size_t max_size, t_page *zone)
{
	size_t	old_size;

	old_size = block->size;
	if (new_size == old_size)
		return (ptr);
	if (new_size < old_size)
		return (shrink_memory(zone, new_size, block));
	if (new_size >= min_size && new_size <= max_size)
	{
		if (increase_memory(zone, block, new_size))
			return (GET_BLOCK_PTR(block));
	}
	return (need_to_reallocate(ptr, new_size, old_size));
}

void	*realloc_tiny_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, 1, n, g_malloc.tiny));
}

void	*realloc_small_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, n + 1, m, g_malloc.small));
}

void	*realloc_large_block(void *ptr, size_t size, t_block *block)
{
	return (realloc_block(ptr, size, block, m + 1, SIZE_MAX, g_malloc.large));
}

void	*realloc_internal(void *ptr, size_t size)
{
	t_block	*block;
	void	*new_ptr;

	if (ptr == NULL)
		return (malloc_internal(size));
	else if (size == 0)
	{
		free_internal(ptr);
		return (NULL);
	}
	block = find_block(g_malloc.tiny, ptr);
	if (block)
		new_ptr = realloc_tiny_block(ptr, size, block);
	else
	{
		block = find_block(g_malloc.small, ptr);
		if (block)
			new_ptr = realloc_small_block(ptr, size, block);
		else
		{
			block = find_block(g_malloc.large, ptr);
			if (block)
				new_ptr = realloc_large_block(ptr, size, block);
			else
			{
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
