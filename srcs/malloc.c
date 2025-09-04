#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

t_malloc	g_malloc = {NULL, NULL, NULL};

t_page	*create_page(size_t size)
{
	t_page	*new_page;

	new_page = mmap(0, sizeof(t_page) + size, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->length = sizeof(t_page) + size;
	new_page->next = NULL;
	new_page->nb_block = 1;
	new_page->nb_block_free = 1;
	new_page->blocks = (void *)new_page + sizeof(t_page);
	initialize_blocks(&new_page->blocks, size);
	return (new_page);
}

t_block *find_free_block_with_enough_spage(t_page *page, size_t size) {
	t_block *current_block;

	current_block = page->blocks;
	while (current_block) {
		if (IS_BLOCK_FREE(current_block) && GET_BLOCK_SIZE(current_block) >= size)
			return current_block;
		current_block = NEXT_BLOCK(current_block);
	}
	return NULL;
}

t_block	*search_block_in_pages_for_alloc(t_page *pages, size_t size)
{
	t_page	*current_page;
	t_block *block;

	current_page = pages;
	while (current_page)
	{
		if (current_page->nb_block_free > 0) {
			block = find_free_block_with_enough_spage(current_page, size);
			return (block);
		}
		current_page = current_page->next;
	}
	return (NULL);
}

void	add_back_page_list(t_page **first, t_page *new)
{
	t_page	*current;

	if (*first == NULL)
	{
		*first = new;
	}
	else
	{
		current = *first;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new;
	}
}

size_t	nb_memory_to_allocate(size_t block_size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = NB_BLOCK * (BLOCK_HEADER_SIZE + block_size);
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

void	*optimized_malloc(t_page **malloc_page, size_t block_size, size_t size)
{
	t_page	*page;
	t_block	*block;

	block = search_block_in_pages_for_alloc(*malloc_page, size);
	if (block)
	{
		page = find_page_by_block(*malloc_page, block);
		if (split_block(block, size) == 0)
			page->nb_block_free--;
		else
			page->nb_block++;
		SET_BLOCK_USE(block);
		return (GET_BLOCK_PTR(block));
	}
	page = create_page((nb_memory_to_allocate(block_size)));
	if (page == NULL)
		return (NULL);
	if (split_block(page->blocks, size) == 0)
		page->nb_block_free--;
	else
		page->nb_block++;
	SET_BLOCK_USE(page->blocks);
	add_back_page_list(malloc_page, page);
	return (GET_BLOCK_PTR(page->blocks));
}

void	*large_malloc(size_t size)
{
	ft_printf("je suis dans le large malloc\n");
	ft_printf("size == %u\n", size);
	return (NULL);
}

void	*malloc(size_t size)
{
	size_t size_aligned;

	ft_printf("je suis dans mon mallloc\n");
	size_aligned = ALIGN8(size);
	if (size_aligned == 0)
		return NULL;
	if (size_aligned <= n)
		return (optimized_malloc(&g_malloc.tiny, n, size_aligned));
	else if (size_aligned <= m)
		return (optimized_malloc(&g_malloc.small, n, size_aligned));
	else
		return (large_malloc(size_aligned));
	return (NULL);
}