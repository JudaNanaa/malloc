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
	block->size = size;
	return (block->ptr);
}

// TODO changer le nb_block 
int increase_memory(t_block *block, size_t size) {
	size_t new_size;
	size_t total;
	t_block *new_free_block;
	
	if (block->next == NULL)
		return 0;
	if (block->next->is_free == false)
		return 0;
	total = block->size + block->next->size + sizeof(t_block);
	if (total < size)
		return 0;
	new_size = total - size - sizeof(t_block);
	block->size = size;
	new_free_block = (void *)block + block->size;
	if (new_size > 0) {
		memmove(new_free_block, block->next, sizeof(t_block));
		new_free_block->size = new_size;
		new_free_block->ptr = (void *)new_free_block + sizeof(t_block);
	}
	return 1;
}

void	*ptr_was_tiny_malloc(t_page *pages, void *ptr, size_t size,
		t_block *block)
{
	if (size == block->size)
		return (ptr);
	if (size < block->size)
		return (shrink_memory(pages, size, block));
	else if (size <= n) {
		if (increase_memory(block, size))
			return block->ptr;
	}
	return (need_to_reallocate(ptr, size, block->size));
}

void	*ptr_was_small_malloc(t_page *pages, void *ptr, size_t size,
		t_block *block)
{
	if (size == block->size)
		return (ptr);
	if (size > n && size < block->size)
		return (shrink_memory(pages, size, block));
	else if (size <= m) {
		if (increase_memory(block, size))
			return block->ptr;
	}
	return (need_to_reallocate(ptr, size, block->size));
}
// TODO changer le realloc car il change le pointeur si size superieur a block.size
void	*realloc(void *ptr, size_t size)
{
	t_block *block;

	if (ptr == NULL)
		return (malloc(size));
	else if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	block = find_block(g_malloc.tiny, ptr);
	if (block)
	{
		return (ptr_was_tiny_malloc(g_malloc.tiny, ptr, size, block));
	}
	block = find_block(g_malloc.small, ptr);
	if (block)
	{
		return (ptr_was_small_malloc(g_malloc.small, ptr, size, block));
	}
	// TODO faire le realloc pour le large
	if (!block)
	{
		ft_putendl_fd("realloc(): invalid pointer", STDERR_FILENO);
		abort();
	}
	return (NULL);
}