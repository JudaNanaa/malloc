#include "../includes/malloc_internal.h"

size_t	print_block_info(t_block *block)
{
	void	*start;
	size_t	size;

	start = GET_BLOCK_PTR(block);
	size = block->size;
	printf("%p - %p : %zu", start, (char *)start + size, size);
	if (IS_BLOCK_free(block))
	{
		printf(" free");
		size = 0;
	}
	printf(" bytes\n");
	return (size);
}

size_t	print_page_blocks(t_page *page)
{
	t_block	*block;
	size_t	total_size;

	block = page->blocks;
	total_size = 0;
	while (block)
	{
		total_size += print_block_info(block);
		block = NEXT_BLOCK(block);
	}
	return (total_size);
}

size_t	print_memory_zone(t_page *page_list, char *zone_name)
{
	t_page	*page;
	size_t	total_size;

	page = page_list;
	total_size = 0;
	while (page)
	{
		printf("%s : %p\n", zone_name, (void *)page);
		total_size += print_page_blocks(page);
		page = page->next;
	}
	return (total_size);
}

void	show_alloc_mem(void)
{
	size_t	total_size;

	total_size = 0;
	printf("\n----------------- Memory Allocation Report -----------------\n");
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	total_size += print_memory_zone(g_malloc.tiny.pages, "TINY");
	pthread_mutex_unlock(&g_malloc.tiny.mutex);
	pthread_mutex_lock(&g_malloc.small.mutex);
	total_size += print_memory_zone(g_malloc.small.pages, "SMALL");
	pthread_mutex_unlock(&g_malloc.small.mutex);
	pthread_mutex_lock(&g_malloc.large.mutex);
	total_size += print_memory_zone(g_malloc.large.pages, "LARGE");
	pthread_mutex_unlock(&g_malloc.large.mutex);
	printf("Total : %zu bytes\n", total_size);
	printf("------------------------------------------------------------\n\n");
}
