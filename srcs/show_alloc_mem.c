#include "../includes/malloc_internal.h"

size_t	print_block_info(t_block *block)
{
	void	*start;
	size_t	size;

	start = GET_BLOCK_PTR(block);
	size = block->size;
	print_mem((unsigned long long int)start, STDERR_FILENO);
	ft_putstr(" - ");
	print_mem((unsigned long long int)start + size, STDERR_FILENO);
	ft_putstr(" : ");
	ft_putnbr(size);
	if (IS_BLOCK_FREE(block))
	{
		ft_putstr(" free");
		size = 0;
	}
	ft_putendl(" bytes");
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
		ft_putstr(zone_name);
		ft_putstr(" : ");
		print_mem((unsigned long long int)GET_BLOCK_PTR(page->blocks),
				STDERR_FILENO);
		ft_putstr("\n");
		total_size += print_page_blocks(page);
		page = page->next;
	}
	return (total_size);
}

void	show_alloc_mem(void)
{
	size_t	total_size;

	total_size = 0;
	ft_putendl("\n----------------- Memory Allocation Report -----------------");
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	total_size += print_memory_zone(g_malloc.tiny.pages, "TINY");
	pthread_mutex_unlock(&g_malloc.tiny.mutex);
	pthread_mutex_lock(&g_malloc.small.mutex);
	total_size += print_memory_zone(g_malloc.small.pages, "SMALL");
	pthread_mutex_unlock(&g_malloc.small.mutex);
	pthread_mutex_lock(&g_malloc.large.mutex);
	total_size += print_memory_zone(g_malloc.large.pages, "LARGE");
	pthread_mutex_unlock(&g_malloc.large.mutex);
	ft_putstr("Total : ");
	ft_putnbr(total_size);
	ft_putendl(" bytes");
	ft_putendl("------------------------------------------------------------\n");
}
