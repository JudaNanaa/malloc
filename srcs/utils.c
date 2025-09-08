#include "../includes/malloc_internal.h"

void	initialize_blocks(t_block **block, size_t size)
{
	t_block	*current_block;

	current_block = *block;
	bzero(current_block->metadata, sizeof(current_block->metadata));
	SET_BLOCK_SIZE(current_block, size);
	SET_BLOCK_FREE(current_block);
	SET_BLOCK_LAST(current_block);
}

void	block_merge_with_next(t_block *first, t_block *second)
{
	size_t	total_size;

	total_size = GET_BLOCK_SIZE(first) + BLOCK_HEADER_SIZE
		+ GET_BLOCK_SIZE(second);
	SET_BLOCK_SIZE(first, total_size);
	if (IS_BLOCK_LAST(second))
		SET_BLOCK_LAST(first);
}

int	merge_block(t_block *block, t_block *prev_block)
{
	int		nb_merge;
	t_block	*next_block;

	if (g_malloc_no_defrag() == true)
		return 0;
	nb_merge = 0;
	next_block = NEXT_BLOCK(block);
	if (next_block && IS_BLOCK_FREE(next_block) == true)
	{
		block_merge_with_next(block, next_block);
		nb_merge++;
	}
	if (prev_block && IS_BLOCK_FREE(prev_block) == true)
	{
		block_merge_with_next(prev_block, block);
		nb_merge++;
	}
	return (nb_merge);
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

int	split_block(t_block *block, size_t size)
{
	t_block	*new_block;
	long	new_size;
	size_t aligned_size;

	if (GET_BLOCK_SIZE(block) == size)
		return (0);
	if (size > ALIGN(block->size) - MEMORY_ALIGNMENT) {
		SET_BLOCK_SIZE(block, size);
		return (1);
	}
	aligned_size = ALIGN(size);
	new_size = GET_BLOCK_SIZE(block) - aligned_size - BLOCK_HEADER_SIZE;
	SET_BLOCK_SIZE(block, size);
	new_block = (void *)block + sizeof(t_block) + aligned_size;
	initialize_blocks(&new_block, new_size);
	if (IS_BLOCK_LAST(block))
		SET_BLOCK_LAST(new_block);
	else
		SET_BLOCK_NOT_LAST(new_block);
	merge_block(new_block, NULL);
	SET_BLOCK_NOT_LAST(block);
	return (1);
}

t_page	*find_page_by_block(t_page *pages, t_block *block)
{
	t_page	*current_page;

	current_page = pages;
	while (current_page)
	{
		if (page_find_block_by_ptr(current_page, GET_BLOCK_PTR(block),
				NULL) != NULL)
			return (current_page);
		current_page = current_page->next;
	}
	return (NULL);
}

t_block	*find_block(t_page *pages, void *ptr)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		block = page_find_block_by_ptr(current_page, ptr, NULL);
		if (block)
			return (block);
		current_page = current_page->next;
	}
	return (NULL);
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
