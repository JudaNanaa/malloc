#include "../includes/malloc_internal.h"
#include <stddef.h>

size_t	print_block_info(t_block *block)
{
	void	*start;
	size_t	size;

	start = GET_BLOCK_PTR(block);
	size = GET_BLOCK_SIZE(block);
	print_mem((unsigned long long int)start);
	ft_putstr(" - ");
	print_mem((unsigned long long int)start + size);
	ft_putstr(" : ");
	ft_putnbr(size);
	if (IS_BLOCK_FREE(block)) {
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
		print_mem((unsigned long long int)GET_BLOCK_PTR(page->blocks));
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
	ft_putendl("----------------- Memory Allocation Report -----------------");
	total_size += print_memory_zone(g_malloc.tiny, "TINY");
	total_size += print_memory_zone(g_malloc.small, "SMALL");
	total_size += print_memory_zone(g_malloc.large, "LARGE");
	ft_putstr("Total : ");
	ft_putnbr(total_size);
	ft_putendl(" bytes");
	ft_putendl("------------------------------------------------------------");
}
