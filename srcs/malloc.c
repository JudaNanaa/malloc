#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

t_malloc g_malloc = {
    .tiny = { .pages = NULL, .last = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER },
    .small = { .pages = NULL, .last = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER },
    .large = { .pages = NULL, .last = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER },
    .tiny_malloc_size = n,
    .small_malloc_size = m,
    .fail_size = -1,
    .set = ATOMIC_VAR_INIT(false),
    .verbose = false,
    .no_defrag = false,
    .trace_file_fd = -1
};


t_page	*create_page(size_t length, size_t max_size)
{
	t_page	*new_page;

	new_page = mmap(0, length, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->length = length;
	new_page->next = NULL;
	memset(&new_page->free_lists, 0, sizeof(new_page->free_lists));
	new_page->free_lists.max_size = max_size;
	new_page->blocks = (void *)new_page + sizeof(t_page);
	initialize_blocks(&new_page->blocks, length - sizeof(t_page)
			- BLOCK_HEADER_SIZE);
	return (new_page);
}
t_block	*find_free_block_with_enough_spage(t_free_list *free_lists, size_t size,
		size_t class)
{
	t_block	*block;

	for (size_t i = class; i < NB_CLASS; i++)
	{
		block = free_lists->head[i];
		while (block)
		{
			if (GET_BLOCK_SIZE(block) >= size)
			{
				remove_block_free_list(free_lists, block);
				return (block);
			}
			block = block->next_free;
		}
	}
	return (NULL);
}

bool	search_block_in_pages_for_alloc(t_page *pages, size_t size,
		t_page_block *out)
{
	t_page	*current_page;
	t_block	*block;
	size_t	class;

	out->page = NULL;
	out->block = NULL;
	if (pages == NULL)
		return (false);
	current_page = pages;
	class = get_size_class(size, current_page->free_lists.max_size);
	while (current_page)
	{
		block = find_free_block_with_enough_spage(&current_page->free_lists,
													size,
													class);
		if (block)
		{
			out->page = current_page;
			out->block = block;
			return (true);
		}
		current_page = current_page->next;
	}
	return (false);
}

void	add_back_page_list(t_mutex_zone *zone, t_page *new)
{
	if (zone->pages == NULL)
		zone->pages = new;
	else
		zone->last->next = new;
	zone->last = new;
}

size_t	page_allocation_size_for_zone(size_t block_size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = sizeof(t_page) + NB_BLOCK * (BLOCK_HEADER_SIZE + block_size);
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

size_t	page_allocation_size_for_large(size_t size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = sizeof(t_page) + size + BLOCK_HEADER_SIZE;
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

void	*optimized_malloc(t_mutex_zone *zone, size_t block_size, size_t size)
{
	t_page_block	res;

	if (search_block_in_pages_for_alloc(zone->pages, size, &res))
	{
		split_block(res.page, res.block, size);
		SET_BLOCK_USE(res.block);
		return (GET_BLOCK_PTR(res.block));
	}
	res.page = create_page(page_allocation_size_for_zone(block_size),
							block_size);
	if (res.page == NULL)
		return (NULL);
	split_block(res.page, res.page->blocks, size);
	SET_BLOCK_USE(res.page->blocks);
	add_back_page_list(zone, res.page);
	return (GET_BLOCK_PTR(res.page->blocks));
}

void	*large_malloc(size_t size)
{
	t_page	*page;

	page = create_page(page_allocation_size_for_large(size), SIZE_MAX);
	if (page == NULL)
		return (NULL);
	split_block(page, page->blocks, size);
	SET_BLOCK_USE(page->blocks);
	add_back_page_list(&g_malloc.large, page);
	return (GET_BLOCK_PTR(page->blocks));
}

bool	malloc_force_fail(size_t size)
{
	if (g_malloc.fail_size != 1 && (int)size == g_malloc.fail_size)
	{
		ft_printf_fd(STDERR_FILENO,
						"[DEBUG] malloc(size: %u) forced to fail!\n",
						size);
		return (true);
	}
	return (false);
}

void	*malloc_internal(size_t size)
{
	void	*ptr;

	if (size == 0)
		return (NULL);
	if (malloc_force_fail(size))
		return (NULL);
	if (size <= g_malloc.tiny_malloc_size)
		ptr = optimized_malloc(&g_malloc.tiny, g_malloc.tiny_malloc_size, size);
	else if (size <= g_malloc.small_malloc_size)
		ptr = optimized_malloc(&g_malloc.small, g_malloc.small_malloc_size,
				size);
	else
		ptr = large_malloc(size);
	return (ptr);
}

void	*lock_malloc(size_t size)
{
	void	*ptr;

	if (size <= g_malloc.tiny_malloc_size)
		pthread_mutex_lock(&g_malloc.tiny.mutex);
	else if (size <= g_malloc.small_malloc_size)
		pthread_mutex_lock(&g_malloc.small.mutex);
	else
		pthread_mutex_lock(&g_malloc.large.mutex);
	ptr = malloc_internal(size);
	if (size <= g_malloc.tiny_malloc_size)
		pthread_mutex_unlock(&g_malloc.tiny.mutex);
	else if (size <= g_malloc.small_malloc_size)
		pthread_mutex_unlock(&g_malloc.small.mutex);
	else
		pthread_mutex_unlock(&g_malloc.large.mutex);
	return (ptr);
}

void	*my_malloc(size_t size)
{
	void	*ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
	ptr = lock_malloc(size);
	if (g_malloc.verbose)
		ft_printf_fd(STDERR_FILENO, "[DEBUG] malloc(%u) -> %p by pid == %d\n", size, ptr, getpid());
	if (g_malloc.trace_file_fd != -1)
		ft_printf_fd(g_malloc.trace_file_fd,
						"malloc(%u) -> %p\n",
						size,
						ptr);
	return (ptr);
}
