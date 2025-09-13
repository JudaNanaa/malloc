#include "../includes/malloc_internal.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

void	initialize_blocks(t_block **block, size_t size)
{
	t_block	*current_block;

	current_block = *block;
	memset(current_block, 0, sizeof(t_block));
	SET_BLOCK_SIZE(current_block, size);
	SET_BLOCK_FREE(current_block);
	SET_BLOCK_LAST(current_block);
}

void	block_merge_with_next(t_free_list *free_lists, t_block *current, t_block *next)
{
	size_t	total_size;

	remove_block_free_list(free_lists, next);
	remove_block_free_list(free_lists, current);
	total_size = GET_BLOCK_SIZE(current) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(next);
	if (IS_BLOCK_LAST(next))
		SET_BLOCK_LAST(current);
	SET_BLOCK_SIZE(current, total_size);
	add_block_to_free_list(free_lists, current);
}

void	merge_block(t_page *page, t_block *block, t_block *prev_block)
{
	t_block	*next_block;

	if (g_malloc.no_defrag == true)
		return ;
	next_block = NEXT_BLOCK(block);
	if (next_block && IS_BLOCK_FREE(next_block) == true)
		block_merge_with_next(&page->free_lists, block, next_block);
	if (prev_block && IS_BLOCK_FREE(prev_block) == true)
		block_merge_with_next(&page->free_lists, prev_block, block);
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
	size_t	aligned_size;

	if (GET_BLOCK_SIZE(block) == size)
		return ;
	if (size > ALIGN(block->size) - MEMORY_ALIGNMENT)
	{
		SET_BLOCK_SIZE(block, size);
		return ;
	}
	aligned_size = ALIGN(size);
	if (GET_BLOCK_SIZE(block) <= aligned_size + BLOCK_HEADER_SIZE) {
		return;
	}
	new_size = GET_BLOCK_SIZE(block) - aligned_size - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(block, size);
	new_block = (void *)block + BLOCK_HEADER_SIZE + aligned_size;
	initialize_blocks(&new_block, new_size);
	if (IS_BLOCK_LAST(block))
		SET_BLOCK_LAST(new_block);
	else
		SET_BLOCK_NOT_LAST(new_block);
	add_block_to_free_list(&page->free_lists, new_block);
	merge_block(page, new_block, NULL);
	SET_BLOCK_NOT_LAST(block);
}

bool	find_block(t_page *pages, void *ptr, t_page_block *out)
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
bool	is_gonna_overflow(size_t nmemb, size_t size)
{
	size_t	check_overflow;

	check_overflow = nmemb * size;
	if (nmemb != 0 && check_overflow / nmemb != size)
	{
		return (true);
	}
	return (false);
}

size_t	get_size_class(size_t size, size_t max_size)
{
	size_t	increment;
	size_t	class;

	increment = max_size >> NB_CLASS;
	class = (size + increment - 1) >> __builtin_ctz(increment);
	if (class >= NB_CLASS)
		return (NB_CLASS - 1);
	return (class);
}

void	add_block_to_free_list(t_free_list *free_lists, t_block *block)
{
	size_t	class;

	class = get_size_class(GET_BLOCK_SIZE(block), free_lists->max_size);

    block->next_free = free_lists->head[class];
    if (free_lists->head[class])
        free_lists->head[class]->prev_free = block;

    free_lists->head[class] = block;
    block->prev_free = NULL;
}

void	remove_block_free_list(t_free_list *free_lists, t_block *block)
{
	size_t class;
	
	class = get_size_class(GET_BLOCK_SIZE(block), free_lists->max_size);
	if (block->prev_free)
	block->prev_free->next_free = block->next_free;
	else
        free_lists->head[class] = block->next_free;
	if (block->next_free)
        block->next_free->prev_free = block->prev_free;
	block->next_free = NULL;
	block->prev_free = NULL;
}
