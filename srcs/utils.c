#include "../includes/malloc_internal.h"

int print_err(const char *msg)
{
	return write(STDERR_FILENO, msg, strlen(msg));
}

void	initialize_blocks(t_block *block, size_t size_of_block)
{
	memset(block, 0, sizeof(t_block));
	SET_BLOCK_SIZE(block, size_of_block);
	SET_BLOCK_TRUE_SIZE(block, ALIGN(size_of_block));
	SET_BLOCK_FREE(block);
	SET_BLOCK_LAST(block);
}

void merge_two_blocks(t_block *current, t_block *next)
{
	size_t total;

	total = GET_BLOCK_TRUE_SIZE(current) + BLOCK_HEADER_SIZE + GET_BLOCK_TRUE_SIZE(next);
	if (IS_BLOCK_LAST(next))
		SET_BLOCK_LAST(current);
	else
		SET_BLOCK_NOT_LAST(current);
	SET_BLOCK_TRUE_SIZE(current, total);
}

void merge_block_with_next(t_mutex_zone *zone, t_block *block)
{
	t_block *next_block;

	next_block = NEXT_BLOCK(block);
	if (next_block && IS_BLOCK_FREE(next_block))
	{
		if (zone != &g_malloc.large)
			delete_node_tree(&zone->root_free, next_block, &zone->sentinel);
		merge_two_blocks(block, next_block);
	}
}

void merge_block_with_prev(t_mutex_zone *zone, t_block **block, t_block *prev_block)
{
	if (prev_block && IS_BLOCK_FREE(prev_block))
	{
		delete_node_tree(&zone->root_free, prev_block, &zone->sentinel);
		merge_two_blocks(prev_block, *block);
		*block = prev_block;
	}
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

bool	find_block(t_page *pages, void *ptr, t_block **out)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr, NULL);
		if (block)
		{
			*out = block;
			return (true);
		}
		current_page = current_page->next;
	}
	out = NULL;
	return (false);
}

bool	is_gonna_overflow(size_t nmemb, size_t size)
{
	size_t	check_overflow;

	check_overflow = nmemb * size;
	if (nmemb != 0 && check_overflow / nmemb != size)
		return (true);
	return (false);
}
