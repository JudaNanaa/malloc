#include "../includes/malloc_internal.h"

t_malloc g_malloc = {
	{ NULL, NULL, PTHREAD_MUTEX_INITIALIZER },
	{ NULL, NULL, PTHREAD_MUTEX_INITIALIZER },
	{ NULL, NULL, PTHREAD_MUTEX_INITIALIZER },
	n,
	m,
	-1,
	false,
	false,
	false,
	-1
};

t_page	*create_page(size_t length)
{
	t_page	*new_page;

	new_page = mmap(0, length, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->length = length;
	new_page->next = NULL;
	bzero(&new_page->free_lists, sizeof(new_page->free_lists));
	new_page->blocks = (void *)new_page + sizeof(t_page);
	initialize_blocks(&new_page->blocks, length - sizeof(t_page) - BLOCK_HEADER_SIZE);
	return (new_page);
}

t_block	*find_free_block_with_enough_spage(t_page *page, size_t size)
{
	t_block	*current_block;

	current_block = page->free_lists.head;
	while (current_block)
	{
		if (GET_BLOCK_SIZE(current_block) >= size) {
			remove_block_free_list(page, current_block);
			return (current_block);
		}
		current_block = NEXT_BLOCK(current_block);
	}
	return (NULL);
}

bool search_block_in_pages_for_alloc(t_page *pages, size_t size, t_page_block *out)
{
    t_page  *current_page;
    t_block *block;

    current_page = pages;
    while (current_page)
    {
		block = find_free_block_with_enough_spage(current_page, size);
		if (block)
		{
			out->page = current_page;
			out->block = block;
			return true;
		}
        current_page = current_page->next;
    }
    out->page = NULL;
    out->block = NULL;
    return false;
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
	t_page_block res;

	if (search_block_in_pages_for_alloc(zone->pages, size, &res))
	{
		split_block(res.page, res.block, size);
		SET_BLOCK_USE(res.block);
		return (GET_BLOCK_PTR(res.block));
	}
	res.page = create_page(page_allocation_size_for_zone(block_size));
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

	page = create_page(page_allocation_size_for_large(size));
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
						"[DEBUG] my_malloc(size: %u) forced to fail!\n",
						size);
		return (true);
	}
	return (false);
}

void	*malloc_internal(size_t size)
{
	void	*ptr;

	if (malloc_force_fail(size))
		return (NULL);
	if (size == 0)
		return (NULL);
	if (size <= g_malloc.tiny_malloc_size)
		ptr = optimized_malloc(&g_malloc.tiny, g_malloc.tiny_malloc_size, size);
	else if (size <= g_malloc.small_malloc_size)
		ptr = optimized_malloc(&g_malloc.small, g_malloc.small_malloc_size, size);
	else
		ptr = large_malloc(size);
	return (ptr);
}

void	*my_malloc(size_t size)
{
	void	*ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
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
	if (g_malloc.verbose)
	{
		ft_printf_fd(STDERR_FILENO, "[DEBUG] my_malloc(%u) -> %p\n",
				size, ptr);
	}
	if (g_malloc.trace_file_fd != -1)
	{
		ft_printf_fd(g_malloc.trace_file_fd,
						"my_malloc(%u) -> %p\n",
						size,
						ptr);
	}
	return (ptr);
}
