#include "../includes/malloc_internal.h"

void	initialize_blocks(t_block **block, size_t size)
{
	t_block	*current_block;

	current_block = *block;
	memset(current_block, 0, sizeof(t_block));
	SET_BLOCK_SIZE(current_block, size);
	SET_BLOCK_FREE(current_block);
	SET_BLOCK_LAST(current_block);
}

void	block_merge_with_next(t_page *page, t_block *first, t_block *second)
{
	size_t	total_size;
	t_block *prev;

	total_size = GET_BLOCK_SIZE(first) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(second);
	SET_BLOCK_SIZE(first, total_size);
	if (IS_BLOCK_LAST(second))
		SET_BLOCK_LAST(first);
	prev = second->prev_free;
	first->prev_free = prev;
	if (prev)
		prev->next_free = first;
	if (page->free_lists.head == second)
		page->free_lists.head = first;
}

void	block_merge_with_prev(t_page *page, t_block *first, t_block *second)
{
	size_t	total_size;
	t_block *next;

	total_size = GET_BLOCK_SIZE(first) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(second);
	SET_BLOCK_SIZE(first, total_size);
	if (IS_BLOCK_LAST(second))
		SET_BLOCK_LAST(first);
	next = second->next_free;
	first->next_free = next;
	if (next)
		next->prev_free = first;
	if (page->free_lists.last == second)
		page->free_lists.last = first;
}

void merge_block(t_page *page, t_block *block, t_block *prev_block)
{
	t_block	*next_block;

	if (g_malloc.no_defrag == true)
		return;
	next_block = NEXT_BLOCK(block);
	if (next_block && IS_BLOCK_FREE(next_block) == true)
		block_merge_with_next(page, block, next_block);
	if (prev_block && IS_BLOCK_FREE(prev_block) == true)
		block_merge_with_prev(page, prev_block, block);
}

t_block	*page_find_block_by_ptr(t_page *page, void *ptr, t_block **prev_out)
{
	t_block	*current_block;
	t_block	*prev_block;

	prev_block = NULL;
	current_block = page->blocks;
	while (current_block)
	{
		if (GET_BLOCK_PTR(current_block) == ptr)
		{
			if (prev_out)
				*prev_out = prev_block;
			return (current_block);
		}
		prev_block = current_block;
		current_block = NEXT_BLOCK(current_block);
	}
	return (NULL);
}

void	split_block(t_page *page, t_block *block, size_t size)
{
	t_block	*new_block;
	long	new_size;
	size_t aligned_size;

	if (GET_BLOCK_SIZE(block) == size)
		return ;
	if (size > ALIGN(block->size) - MEMORY_ALIGNMENT) {
		SET_BLOCK_SIZE(block, size);
		return ;
	}
	aligned_size = ALIGN(size);
	new_size = GET_BLOCK_SIZE(block) - aligned_size - BLOCK_HEADER_SIZE;
	if (new_size <= 0)
		return ;
	SET_BLOCK_SIZE(block, size);
	new_block = (void *)block + BLOCK_HEADER_SIZE + aligned_size;
	initialize_blocks(&new_block, new_size);
	if (IS_BLOCK_LAST(block))
		SET_BLOCK_LAST(new_block);
	else
		SET_BLOCK_NOT_LAST(new_block);
	add_block_to_free_list(page, new_block);
	merge_block(page, new_block, NULL);
	SET_BLOCK_NOT_LAST(block);
}

bool find_block(t_page *pages, void *ptr, t_page_block *out)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr, NULL);
		if (block)
		{
			out->block = block;
			out->page = current_page;
			return (true);
		}
		current_page = current_page->next;
	}
	out->block = NULL;
	out->page = NULL;
	return (false);
}
bool is_gonna_overflow(size_t nmemb, size_t size)
{
	size_t check_overflow;

	check_overflow = nmemb * size;

	if (nmemb != 0 && check_overflow / nmemb != size) {
		return true;
	}
	return false;
}

void add_block_to_free_list(t_page *page, t_block *block)
{
	if (page->free_lists.head == NULL)
		page->free_lists.head = block;
	else
		page->free_lists.last->next_free = block;
	page->free_lists.last = block;

}

void remove_block_free_list(t_page *page, t_block *block)
{
	t_block *prev;
	t_block *next;

	if (page->free_lists.head == block)
		page->free_lists.head = block->next_free;
	else if (page->free_lists.last == block)
		page->free_lists.last = block->prev_free;
	else
	{
		prev = block->prev_free;
		next = block->next_free;
		if (prev)
			prev->next_free = next;
		if (next)
			next->prev_free = prev;
	}


}