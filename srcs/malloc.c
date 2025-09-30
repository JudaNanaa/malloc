#include "../includes/malloc_internal.h"

t_malloc	g_malloc = {
	.tiny = {.max_size_malloc = n, .pages = NULL, .last = NULL,
		.root_free = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER},
	.small = {.max_size_malloc = m, .pages = NULL, .last = NULL,
		.root_free = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER},
	.large = {.max_size_malloc = SIZE_MAX, .pages = NULL, .last = NULL,
		.root_free = NULL, .mutex = PTHREAD_MUTEX_INITIALIZER},
	.fail_size = -1,
	.set = ATOMIC_VAR_INIT(false),
	.verbose = false,
	.no_defrag = false,
	.trace_file_fd = -1,
	};

t_page	*create_page(size_t length)
{
	t_page	*new_page;

	new_page = mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->length = length;
	new_page->next = NULL;
	new_page->prev = NULL;
	new_page->blocks = (void *)new_page + PAGE_HEADER_SIZE;
	initialize_blocks(new_page->blocks, length - PAGE_HEADER_SIZE
			- BLOCK_HEADER_SIZE);
	return (new_page);
}

size_t	page_allocation_size_for_zone(size_t block_size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = PAGE_HEADER_SIZE + NB_BLOCK * (BLOCK_HEADER_SIZE + block_size);
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

size_t	page_allocation_size_for_large(size_t size)
{
	size_t	needed;
	size_t	total;
	int		page_size;

	page_size = sysconf(_SC_PAGESIZE);
	needed = PAGE_HEADER_SIZE + BLOCK_HEADER_SIZE + size;
	total = ((needed + page_size - 1) / page_size) * page_size;
	return (total);
}

bool	find_free_block(t_mutex_zone *zone, size_t size, t_block **block_out)
{
	t_block	*block;

	block = search_best_node(zone->root_free, size, &zone->sentinel);
	if (block)
	{
		delete_node_tree(&zone->root_free, block, &zone->sentinel);
		*block_out = block;
		return (true);
	}
	return (false);
}

void	add_to_page_list(t_mutex_zone *zone, t_page *new)
{
	if (zone->pages == NULL)
		zone->pages = new;
	else
	{
		zone->last->next = new;
		new->prev = zone->last;
	}
	zone->last = new;
}

void	split_block(t_block *block, size_t size, t_mutex_zone *zone)
{
	t_block	*new_block;
	size_t	new_size;
	size_t	aligned_size;

	SET_BLOCK_SIZE(block, size);
	if (GET_BLOCK_TRUE_SIZE(block) == size)
		return ;
	if (size > GET_BLOCK_TRUE_SIZE(block) - MEMORY_ALIGNMENT)
		return ;
	aligned_size = ALIGN(size);
	if (GET_BLOCK_TRUE_SIZE(block) <= aligned_size + BLOCK_HEADER_SIZE)
		return ;
	new_size = GET_BLOCK_TRUE_SIZE(block) - aligned_size - BLOCK_HEADER_SIZE;
	SET_BLOCK_TRUE_SIZE(block, aligned_size);
	new_block = (t_block *)((char *)block + BLOCK_HEADER_SIZE
			+ GET_BLOCK_TRUE_SIZE(block));
	initialize_blocks(new_block, new_size);
	if (IS_BLOCK_LAST(block))
		SET_BLOCK_LAST(new_block);
	else
		SET_BLOCK_NOT_LAST(new_block);
	SET_BLOCK_NOT_LAST(block);
	if (g_malloc.no_defrag == false)
		merge_block_with_next(zone, new_block);
	if (zone != &g_malloc.large)
		insert_node_tree(&zone->root_free, new_block, &zone->sentinel);
}

void	*optimize_malloc(t_mutex_zone *zone, size_t size)
{
	t_page	*page;
	t_block	*block;

	if (find_free_block(zone, size, &block))
	{
		split_block(block, size, zone);
		SET_BLOCK_USE(block);
		return (GET_BLOCK_PTR(block));
	}
	page = create_page(page_allocation_size_for_zone(zone->max_size_malloc));
	if (page == NULL)
		return (NULL);
	block = page->blocks;
	split_block(block, size, zone);
	SET_BLOCK_USE(block);
	add_to_page_list(zone, page);
	return (GET_BLOCK_PTR(block));
}

void	*large_malloc(size_t size)
{
	t_page	*page;
	t_block	*block;

	page = create_page(page_allocation_size_for_large(size));
	if (page == NULL)
		return (NULL);
	block = page->blocks;
	split_block(block, size, &g_malloc.large);
	SET_BLOCK_USE(block);
	add_to_page_list(&g_malloc.large, page);
	return (GET_BLOCK_PTR(block));
}

void	*malloc_internal(size_t size)
{
	if (size == 0)
		return (NULL);
	else if (size <= g_malloc.tiny.max_size_malloc)
		return (optimize_malloc(&g_malloc.tiny, size));
	else if (size <= g_malloc.small.max_size_malloc)
		return (optimize_malloc(&g_malloc.small, size));
	return (large_malloc(size));
}

void	*lock_malloc(size_t size)
{
	void			*ptr;
	pthread_mutex_t	*mutex;

	if (size <= g_malloc.tiny.max_size_malloc)
		mutex = &g_malloc.tiny.mutex;
	else if (size <= g_malloc.small.max_size_malloc)
		mutex = &g_malloc.small.mutex;
	else
		mutex = &g_malloc.large.mutex;
	pthread_mutex_lock(mutex);
	ptr = malloc_internal(size);
	pthread_mutex_unlock(mutex);
	return (ptr);
}

__attribute__((visibility("default"))) void *MALLOC_NAME(size_t size)
{
	void	*ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
	ptr = lock_malloc(size);
	if (g_malloc.verbose)
		ft_printf_fd(STDERR_FILENO, "[DEBUG] malloc(%u) -> %p by pid == %d\n",
				size, ptr, getpid());
	if (g_malloc.trace_file_fd != -1)
		ft_printf_fd(g_malloc.trace_file_fd,
						"malloc(%u) -> %p\n",
						size,
						ptr);
	return (ptr);
}
