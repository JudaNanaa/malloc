#include "../includes/malloc_internal.h"

void	hex_dump_block(t_block *block)
{
	unsigned char	*ptr;
	size_t			size;

	ptr = (unsigned char *)GET_BLOCK_PTR(block);
	size = block->size;
	ft_putstr("  Hex dump: ");
	for (size_t i = 0; i < size && i < 16; i++)
		ft_printf_fd(STDERR_FILENO, "%X ", ptr[i]);
	if (size > 16)
		ft_putstr("...");
	ft_putendl("");
}

size_t	print_block_info_ex(t_block *block, void *page_start)
{
	void	*start;
	size_t	size;
	size_t	offset;

	start = GET_BLOCK_PTR(block);
	size = block->size;
	offset = (size_t)((char *)start - (char *)page_start);
	print_mem((unsigned long long int)start, STDERR_FILENO);
	ft_putstr(" - ");
	print_mem((unsigned long long int)start + size, STDERR_FILENO);
	ft_putstr(" : ");
	ft_putnbr(size);
	ft_putstr(" bytes");
	if (IS_BLOCK_FREE(block))
		ft_putstr(" [FREE]");
	else
		ft_putstr(" [USED]");
	ft_putstr(" | offset=");
	ft_putnbr(offset);
	ft_putendl("");
	if (IS_BLOCK_FREE(block) == false)
		hex_dump_block(block);
	return (IS_BLOCK_FREE(block) ? 0 : size);
}

size_t	print_page_blocks_ex(t_page *page)
{
	t_block	*block;
	size_t	total_size;
	size_t	free_blocks;
	size_t	used_blocks;

	free_blocks = 0;
	used_blocks = 0;
	block = page->blocks;
	total_size = 0;
	while (block)
	{
		total_size += print_block_info_ex(block, page);
		if (IS_BLOCK_FREE(block))
			free_blocks++;
		else
			used_blocks++;
		block = NEXT_BLOCK(block);
	}
	ft_printf_fd(STDERR_FILENO, "   -> Page stats: %u used, %u my_free\n",
			used_blocks, free_blocks);
	return (total_size);
}

size_t	print_memory_zone_ex(t_page *page_list, char *zone_name)
{
	t_page	*page;
	size_t	total_size;
	int		page_id;

	page_id = 0;
	page = page_list;
	total_size = 0;
	while (page)
	{
		ft_printf_fd(STDERR_FILENO, "%s (page %d) : ", zone_name, page_id++);
		print_mem((unsigned long long int)page,
					STDERR_FILENO);
		ft_putstr("\n");
		total_size += print_page_blocks_ex(page);
		page = page->next;
	}
	return (total_size);
}

void	show_alloc_mem_ex(void)
{
	size_t	total_size;

	total_size = 0;
	ft_putendl("\n================= Extended Memory Report =================");
	pthread_mutex_lock(&g_malloc.tiny.mutex);
	total_size += print_memory_zone_ex(g_malloc.tiny.pages, "TINY");
	pthread_mutex_unlock(&g_malloc.tiny.mutex);
	pthread_mutex_lock(&g_malloc.small.mutex);
	total_size += print_memory_zone_ex(g_malloc.small.pages, "SMALL");
	pthread_mutex_unlock(&g_malloc.small.mutex);
	pthread_mutex_lock(&g_malloc.large.mutex);
	total_size += print_memory_zone_ex(g_malloc.large.pages, "LARGE");
	pthread_mutex_unlock(&g_malloc.large.mutex);
	ft_putstr("Total allocated (used only): ");
	ft_putnbr(total_size);
	ft_putendl(" bytes");
	ft_putendl("==========================================================\n");
}
