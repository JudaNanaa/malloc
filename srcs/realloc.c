#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

	if (split_block(block, size) == 1)
	{
		current_page = find_page_by_block(pages, block);
		current_page->nb_block++;
	}
	SET_BLOCK_SIZE(block, size);
	return (GET_BLOCK_PTR(block));
}

// TODO changer le nb_block 
int increase_memory(t_block *block, size_t size) {
	size_t new_size;
	size_t total;
	t_block *new_free_block;
	t_block*next_block;
	
	next_block = NEXT_BLOCK(block);
	if (next_block == NULL)
		return 0;
	if (IS_BLOCK_FREE(next_block) == false)
		return 0;
	total = GET_BLOCK_SIZE(block) + GET_BLOCK_SIZE(next_block) + BLOCK_HEADER_SIZE;
	if (total < size)
		return 0;
	new_size = total - size - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(block, size);
	new_free_block = (void *)block + GET_BLOCK_SIZE(block);
	if (new_size > 0) {
		memmove(new_free_block, next_block, BLOCK_HEADER_SIZE);
		SET_BLOCK_SIZE(new_free_block, new_size);
	}
	return 1;
}

void	*ptr_was_tiny_malloc(t_page *pages, void *ptr, size_t size,
		t_block *block)
{
	if (size == GET_BLOCK_SIZE(block))
		return (ptr);
	if (size < GET_BLOCK_SIZE(block))
		return (shrink_memory(pages, size, block));
	else if (size <= n) {
		if (increase_memory(block, size))
			return GET_BLOCK_PTR(block);
	}
	return (need_to_reallocate(ptr, size, GET_BLOCK_SIZE(block)));
}

void	*ptr_was_small_malloc(t_page *pages, void *ptr, size_t size,
		t_block *block)
{
	if (size == GET_BLOCK_SIZE(block))
		return (ptr);
	if (size > n && size < GET_BLOCK_SIZE(block))
		return (shrink_memory(pages, size, block));
	else if (size <= m) {
		if (increase_memory(block, size))
			return GET_BLOCK_PTR(block);
	}
	return (need_to_reallocate(ptr, size, GET_BLOCK_SIZE(block)));
}

void	*realloc(void *ptr, size_t size)
{
	t_block *block;
	size_t size_aligned;

	size_aligned = ALIGN8(size);
	if (ptr == NULL)
		return (malloc(size_aligned));
	else if (size_aligned == 0)
	{
		free(ptr);
		return (NULL);
	}
	block = find_block(g_malloc.tiny, ptr);
	if (block)
	{
		return (ptr_was_tiny_malloc(g_malloc.tiny, ptr, size_aligned, block));
	}
	block = find_block(g_malloc.small, ptr);
	if (block)
	{
		return (ptr_was_small_malloc(g_malloc.small, ptr, size_aligned, block));
	}
	// TODO faire le realloc pour le large
	if (!block)
	{
		ft_putendl_fd("realloc(): invalid pointer", STDERR_FILENO);
		abort();
	}
	return (NULL);
}