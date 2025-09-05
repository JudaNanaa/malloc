#include "../includes/malloc_internal.h"
#include <stddef.h>

size_t	show_block_infos(t_block *block)
{
	void	*begin;
	size_t	size;

	begin = GET_BLOCK_PTR(block);
	size = GET_BLOCK_SIZE(block);
	print_mem((unsigned long long int)begin);
	ft_putstr(" - ");
	print_mem((unsigned long long int)begin + size);
	ft_putstr(" : ");
	ft_putnbr(size);
	if (IS_BLOCK_FREE(block))
		ft_putstr(" free");
	ft_putendl(" bytes");
	return (size);
}

size_t	block_infos(t_page *page)
{
	t_block	*current_block;
	size_t	total;

	current_block = page->blocks;
	total = 0;
	while (current_block)
	{
		total += show_block_infos(current_block);
		current_block = NEXT_BLOCK(current_block);
	}
	return (total);
}

size_t	show_mem(t_page *malloc_page, char *mem_zone)
{
	t_page	*current_page;
	size_t	total;

	total = 0;
	current_page = malloc_page;
	while (current_page)
	{
		ft_putstr(mem_zone);
		ft_putstr(" : ");
		print_mem((unsigned long long int)GET_BLOCK_PTR(current_page->blocks));
		ft_putstr("\n");
		total += block_infos(current_page);
		current_page = current_page->next;
	}
	return (total);
}

void	show_alloc_mem(void)
{
	size_t	total;

	total = 0;
	ft_putendl("-----------------show_alloc_mem-----------------");
	total += show_mem(g_malloc.tiny, "TINY");
	total += show_mem(g_malloc.small, "SMALL");
	total += show_mem(g_malloc.large, "LARGE");
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putendl(" bytes");
	ft_putendl("------------------------------------------------");
}
