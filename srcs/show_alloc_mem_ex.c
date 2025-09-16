#include "../includes/malloc_internal.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void hex_dump_block(t_block *block)
{
    unsigned char *ptr;
    size_t size;

    ptr = (unsigned char *)GET_BLOCK_PTR(block);
    size = block->size;
    ft_printf_fd(STDERR_FILENO, "  Hex dump: ");
    for (size_t i = 0; i < size && i < 16; i++)
        ft_printf_fd(STDERR_FILENO, "%02X ", ptr[i]);
    if (size > 16)
        ft_printf_fd(STDERR_FILENO, "...");
    ft_printf_fd(STDERR_FILENO, "\n");
}

size_t print_block_info_ex(t_block *block, void *page_start)
{
    void *start;
    size_t size;
    size_t offset;

    start = GET_BLOCK_PTR(block);
    size = block->size;
    offset = (size_t)((char *)start - (char *)page_start);

    ft_printf_fd(STDERR_FILENO, "%p - %p : %u bytes", start, (char *)start + size, size);

    if (IS_BLOCK_free(block))
        ft_printf_fd(STDERR_FILENO, " [free]");
    else
        ft_printf_fd(STDERR_FILENO, " [USED]");

    ft_printf_fd(STDERR_FILENO, " | offset=%u\n", offset);

    if (!IS_BLOCK_free(block))
        hex_dump_block(block);

    return (IS_BLOCK_free(block) ? 0 : size);
}

size_t print_page_blocks_ex(t_page *page)
{
    t_block *block;
    size_t total_size;
    size_t free_blocks;
    size_t used_blocks;

    free_blocks = 0;
    used_blocks = 0;
    block = page->blocks;
    total_size = 0;
    while (block)
    {
        total_size += print_block_info_ex(block, page);
        if (IS_BLOCK_free(block))
            free_blocks++;
        else
            used_blocks++;
        block = NEXT_BLOCK(block);
    }
    ft_printf_fd(STDERR_FILENO, "   -> Page stats: %u used, %u free\n", used_blocks, free_blocks);
    return total_size;
}

size_t print_memory_zone_ex(t_page *page_list, char *zone_name)
{
    t_page *page;
    size_t total_size;
    int page_id;

    page_id = 0;
    page = page_list;
    total_size = 0;
    while (page)
    {
        ft_printf_fd(STDERR_FILENO, "%s (page %d) : %p to %p\n", zone_name, page_id++, (void *)page, (void *)page + page->length);
        total_size += print_page_blocks_ex(page);
        page = page->next;
    }
    return total_size;
}

void show_alloc_mem_ex(void)
{
    size_t total_size;

    total_size = 0;
    ft_printf_fd(STDERR_FILENO, "\n================= Extended Memory Report =================\n");
    pthread_mutex_lock(&g_malloc.tiny.mutex);
    total_size += print_memory_zone_ex(g_malloc.tiny.pages, "TINY");
    pthread_mutex_unlock(&g_malloc.tiny.mutex);
    pthread_mutex_lock(&g_malloc.small.mutex);
    total_size += print_memory_zone_ex(g_malloc.small.pages, "SMALL");
    pthread_mutex_unlock(&g_malloc.small.mutex);
    pthread_mutex_lock(&g_malloc.large.mutex);
    total_size += print_memory_zone_ex(g_malloc.large.pages, "LARGE");
    pthread_mutex_unlock(&g_malloc.large.mutex);
    ft_printf_fd(STDERR_FILENO, "Total allocated (used only): %u bytes\n", total_size);
    ft_printf_fd(STDERR_FILENO, "==========================================================\n\n");
}
