#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

t_malloc	g_malloc = {NULL, NULL, NULL};

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

t_block	*get_first_free_block(t_page *page)
{
	t_block	*current_block;

	current_block = page->blocks;
	while (current_block->is_free == false)
		current_block = current_block->next;
	return (current_block);
}

t_page	*search_page_with_space(t_page *pages)
{
	t_page	*current_page;

	current_page = pages;
	while (current_page)
	{
		if (current_page->nb_block_free > 0)
			return (current_page);
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
	block->next = new_block;
	initialize_blocks(&new_block, new_size);
	new_block->prev = block;
	return (1);
}

size_t	nb_memory_to_allocate(size_t block_size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = NB_BLOCK * (sizeof(t_block) + block_size);
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

void	*optimized_malloc(t_page **malloc_page, size_t block_size, size_t size)
{
	t_page	*page;
	t_block	*block;

	page = search_page_with_space(*malloc_page);
	if (page)
	{
		block = get_first_free_block(page);
		if (split_block(block, size) == 0)
		{
			page->nb_block_free--;
		}
		else
		{
			page->nb_block++;
		}
		block->is_free = false;
		block->size = size;
		return (block->ptr);
	}
	page = create_page((nb_memory_to_allocate(block_size)));
	if (page == NULL)
		return (NULL);
	if (split_block(page->blocks, size) == 0)
	{
		page->nb_block_free--;
	}
	else
	{
		page->nb_block++;
	}
	page->blocks->size = size;
	page->blocks->is_free = false;
	add_back_page_list(malloc_page, page);
	return (page->blocks->ptr);
}

void	*large_malloc(size_t size)
{
	ft_printf("je suis dans le large malloc\n");
	ft_printf("size == %u\n", size);
	return (NULL);
}

void	*malloc(size_t size)
{
	if (size == 0)
		return NULL;
	if (size <= n)
		return (optimized_malloc(&g_malloc.tiny, n, size));
	else if (size <= m)
		return (optimized_malloc(&g_malloc.small, n, size));
	else
		return (large_malloc(size));
	return (NULL);
}