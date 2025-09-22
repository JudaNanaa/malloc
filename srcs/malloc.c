#include "../includes/malloc_internal.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

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

t_page *create_page(size_t length, size_t max_size)
{
	t_page *new_page;

	new_page = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_page == MAP_FAILED)
	{
		errno = ENOMEM;
		return NULL;
	}
	new_page->length = length;
	memset(&new_page->free_lists, 0, sizeof(t_free_list));
	new_page->free_lists.max_size = max_size;
	new_page->next = NULL;
	new_page->prev = NULL;
	new_page->blocks = (void *)new_page + PAGE_HEADER_SIZE;
	initialize_blocks(new_page->blocks, length - PAGE_HEADER_SIZE - BLOCK_HEADER_SIZE);
	return new_page;
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

t_block *find_free_block_in_page(t_page *page, size_t size, size_t class)
{
	t_block *block;
	int i;

	i = class;
	while (i < NB_CLASS)
	{
		block = page->free_lists.head[i];
		while (block) {
			if (GET_BLOCK_TRUE_SIZE(block) >= size)
				return block;
			block = block->next_free;
		}
		i++;
	}
	return NULL;
}

bool find_free_block(t_page *page_list, size_t size, t_page_block *out)
{
	t_page *current_page;
	size_t class;
	t_block *block;

	current_page = page_list;
	memset(out, 0, sizeof(*out));
	if (current_page == NULL)
		return false;
	class = get_size_class(size, page_list->free_lists.max_size);
	while (current_page)
	{
		block = find_free_block_in_page(current_page, size, class);
		if (block)
		{
			remove_block_free_list(&current_page->free_lists, block);
			out->page = current_page;
			out->block = block;
			return true;
		}
		current_page = current_page->next;
	}
	return false;
}

void add_to_page_list(t_mutex_zone *zone, t_page *new)
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

void split_block(t_page_block *res, size_t size)
{
	t_block *new_block;
	size_t new_size;
	size_t aligned_size;

	SET_BLOCK_SIZE(res->block, size);
	if (GET_BLOCK_TRUE_SIZE(res->block) == size)
		return;
	if (size > GET_BLOCK_TRUE_SIZE(res->block) - MEMORY_ALIGNMENT)
		return;
	aligned_size = ALIGN(size);
	if (GET_BLOCK_TRUE_SIZE(res->block) <= aligned_size + BLOCK_HEADER_SIZE)
		return;
	new_size = GET_BLOCK_TRUE_SIZE(res->block) - aligned_size - BLOCK_HEADER_SIZE;
	SET_BLOCK_TRUE_SIZE(res->block, aligned_size);
	new_block = (t_block *)((char *)res->block 
                        + BLOCK_HEADER_SIZE 
                        + GET_BLOCK_TRUE_SIZE(res->block));
	initialize_blocks(new_block, new_size);
	if (IS_BLOCK_LAST(res->block))
		SET_BLOCK_LAST(new_block);
	else
		SET_BLOCK_NOT_LAST(new_block);
	SET_BLOCK_NOT_LAST(res->block);
	if (g_malloc.no_defrag == false)
		merge_block_with_next(&res->page->free_lists, new_block);
	add_block_to_free_list(&res->page->free_lists, new_block);
}

void *optimize_malloc(t_mutex_zone *zone, size_t size, size_t block_size)
{
	t_page_block res;

	if (find_free_block(zone->pages, size, &res))
	{
		split_block(&res, size);
		SET_BLOCK_USE(res.block);
		return GET_BLOCK_PTR(res.block);
	}
	res.page = create_page(page_allocation_size_for_zone(block_size), block_size);
	if (res.page == NULL)
		return NULL;
	res.block = res.page->blocks;
	split_block(&res, size);
	SET_BLOCK_USE(res.block);
	add_to_page_list(zone, res.page);
	return GET_BLOCK_PTR(res.block);
}

void *large_malloc(size_t size)
{
	t_page_block res;

	res.page = create_page(page_allocation_size_for_large(size), SIZE_MAX);
	if (res.page == NULL)
		return NULL;
	res.block = res.page->blocks;
	split_block(&res, size);
	SET_BLOCK_USE(res.block);
	add_to_page_list(&g_malloc.large, res.page);
	return GET_BLOCK_PTR(res.block);
}

void *malloc_internal(size_t size)
{
	if (size == 0)
		return NULL;
	else if (size <= g_malloc.tiny_malloc_size)
		return optimize_malloc(&g_malloc.tiny, size, g_malloc.tiny_malloc_size);
	else if (size <= g_malloc.small_malloc_size)
		return optimize_malloc(&g_malloc.small, size, g_malloc.small_malloc_size);
	return large_malloc(size);
}



void *lock_malloc(size_t size)
{
    void *ptr;
    pthread_mutex_t *mutex;

    if (size <= g_malloc.tiny_malloc_size)
        mutex = &g_malloc.tiny.mutex;
    else if (size <= g_malloc.small_malloc_size)
        mutex = &g_malloc.small.mutex;
    else
        mutex = &g_malloc.large.mutex;

    pthread_mutex_lock(mutex);
    ptr = malloc_internal(size);
    pthread_mutex_unlock(mutex);

    return ptr;
}

__attribute__((visibility("default")))
void *MALLOC_NAME(size_t size)
{
	void *ptr;

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
	return ptr;
}