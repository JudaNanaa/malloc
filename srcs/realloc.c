#include "../includes/malloc_internal.h"

void	*need_to_reallocate(void *ptr, size_t size, size_t previous_size)
{
	void	*new_ptr;

	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);
	memmove(new_ptr, ptr, previous_size);
	free(ptr);
	return (new_ptr);
}

void	*shrink_memory(t_page *pages, size_t size, t_block *block)
{
	t_page	*current_page;

	if (split_block(block, size) == 0)
	{
		current_page = find_page_by_block(pages, block);
		current_page->nb_block_free--;
	}
	SET_BLOCK_SIZE(block, size);
	return (GET_BLOCK_PTR(block));
}

// TODO parfois pas besoin de toucher au block suivant
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
	new_size = total - size - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(block, size);
	new_free_block = (void *)block + BLOCK_HEADER_SIZE + size;
	memmove(new_free_block, next_block, BLOCK_HEADER_SIZE);
	SET_BLOCK_SIZE(new_free_block, new_size);
	current_page = find_page_by_block(pages, block);
	current_page->nb_block_free++;
	return (1);
}

void	*realloc_block(void *ptr, size_t new_size, t_block *block,
		size_t min_size, size_t max_size, t_page *zone)
{
	size_t	old_size;

	old_size = GET_BLOCK_SIZE(block);
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

void	*realloc(void *ptr, size_t size)
{
	t_block	*block;
	size_t	aligned_size;

	aligned_size = ALIGN(size);
	if (ptr == NULL)
		return (malloc(aligned_size));
	else if (aligned_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	block = find_block(g_malloc.tiny, ptr);
	if (block)
		return (realloc_tiny_block(ptr, aligned_size, block));
	block = find_block(g_malloc.small, ptr);
	if (block)
		return (realloc_small_block(ptr, aligned_size, block));
	block = find_block(g_malloc.large, ptr);
	if (block)
		return (realloc_large_block(ptr, aligned_size, block));
	ft_putendl_fd("realloc(): invalid pointer", STDERR_FILENO);
	abort();
	return (NULL);
}
