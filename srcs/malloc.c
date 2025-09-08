#include "../includes/malloc_internal.h"

t_malloc	g_malloc = {NULL, NULL, NULL, -1, false, false, false, -1};

t_page	*create_page(size_t length)
{
	t_page	*new_page;

	new_page = mmap(0, length, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->length = length;
	new_page->next = NULL;
	new_page->nb_block_free = 1;
	new_page->blocks = (void *)new_page + sizeof(t_page);
	initialize_blocks(&new_page->blocks, length - sizeof(t_page));
	return (new_page);
}

t_block	*find_free_block_with_enough_spage(t_page *page, size_t size)
{
	t_block	*current_block;

	current_block = page->blocks;
	while (current_block)
	{
		if (IS_BLOCK_FREE(current_block)
			&& GET_BLOCK_SIZE(current_block) >= size)
			return (current_block);
		current_block = NEXT_BLOCK(current_block);
	}
	return (NULL);
}

t_block	*search_block_in_pages_for_alloc(t_page *pages, size_t size)
{
	t_page	*current_page;
	t_block	*block;

	current_page = pages;
	while (current_page)
	{
		if (current_page->nb_block_free > 0)
		{
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
	needed = sizeof(t_page) + size;
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
		SET_BLOCK_USE(block);
		return (GET_BLOCK_PTR(block));
	}
	page = create_page((page_allocation_size_for_zone(block_size)));
	if (page == NULL)
		return (NULL);
	if (split_block(page->blocks, size) == 0)
		page->nb_block_free--;
	SET_BLOCK_USE(page->blocks);
	add_back_page_list(malloc_page, page);
	return (GET_BLOCK_PTR(page->blocks));
}

void	*large_malloc(size_t size)
{
	t_page	*page;

	page = create_page(page_allocation_size_for_large(size));
	if (page == NULL)
		return (NULL);
	split_block(page->blocks, size);
	SET_BLOCK_USE(page->blocks);
	add_back_page_list(&g_malloc.large, page);
	return (GET_BLOCK_PTR(page->blocks));
}

bool	malloc_force_fail(size_t size)
{
	if (g_malloc_fail_size() != 1 && (int)size == g_malloc_fail_size())
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

	if (malloc_force_fail(size))
		return (NULL);
	if (size == 0)
		return (NULL);
	if (size <= n)
		ptr = optimized_malloc(&g_malloc.tiny, n, size);
	else if (size <= m)
		ptr = optimized_malloc(&g_malloc.small, m, size);
	else
		ptr = large_malloc(size);
	return (ptr);
}

void	*malloc(size_t size)
{
	void	*ptr;

	if (!g_malloc_is_set())
		malloc_init();
	ptr = malloc_internal(size);
	if (g_malloc_verbose())
	{
		ft_printf_fd(STDERR_FILENO, "[DEBUG] malloc(%u) -> %p\n",
				size, ptr);
	}
	if (g_malloc_trace_file_fd() != -1)
	{
		ft_printf_fd(g_malloc_trace_file_fd(),
						"malloc(%u) -> %p\n",
						size,
						ptr);
	}
	return (ptr);
}
