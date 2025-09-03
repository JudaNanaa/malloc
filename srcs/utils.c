#include "../includes/malloc_internal.h"

void	initialize_blocks(t_block **block, size_t size)
{
	void	*begin;
	t_block	*current_block;

	current_block = *block;
	begin = (void *)current_block + sizeof(t_block);
	current_block->ptr = begin;
	current_block->is_free = true;
	current_block->size = size;
	current_block->next = NULL;
	current_block->prev = NULL;
}

t_block	*page_find_block_by_ptr(t_page *page, void *ptr)
{
	t_block *current_block;

	current_block = page->blocks;
	while (current_block)
	{

		if (current_block->ptr == ptr)
		{
			return (current_block);
		}
		current_block = current_block->next;
	}
	return (NULL);
}

int	split_block(t_block *block, size_t size)
{
	t_block	*new_block;
	long	new_size;

	new_size = block->size - size - sizeof(t_block);
	if (new_size <= 0)
	{
		return (0);
	}
	new_block = (void *)block + sizeof(t_block) + size;
	initialize_blocks(&new_block, new_size);
	new_block->next = block->next;
	new_block->prev = block;
	block->next = new_block;
	return (1);
}

t_page *find_page_by_block(t_page *pages, t_block *block) {
	t_page *current_page;

	current_page = pages;
	while (current_page) {
		if (page_find_block_by_ptr(current_page, block->ptr) != NULL) {
			return current_page;
		}
		current_page = current_page->next;
	}
	return NULL;
}

t_block	*find_block(t_page *pages, void *ptr)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr);
		if (block)
		{
			return (block);
		}
		current_page = current_page->next;
	}
	return (NULL);
}