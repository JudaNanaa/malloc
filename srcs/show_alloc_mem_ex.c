#include "../includes/malloc_internal.h"
#include <stdio.h>
#include <pthread.h>

void hex_dump_block(t_block *block)
{
    unsigned char *ptr;
    size_t size;

    ptr = (unsigned char *)GET_BLOCK_PTR(block);
    size = block->size;
    printf("  Hex dump: ");
    for (size_t i = 0; i < size && i < 16; i++)
        printf("%02X ", ptr[i]);
    if (size > 16)
        printf("...");
    printf("\n");
}

size_t print_block_info_ex(t_block *block, void *page_start)
{
    void *start;
    size_t size;
    size_t offset;

    start = GET_BLOCK_PTR(block);
    size = block->size;
    offset = (size_t)((char *)start - (char *)page_start);

    printf("%p - %p : %zu bytes", start, (char *)start + size, size);

    if (IS_BLOCK_free(block))
        printf(" [free]");
    else
        printf(" [USED]");

    printf(" | offset=%zu\n", offset);

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
    printf("   -> Page stats: %zu used, %zu free\n", used_blocks, free_blocks);
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
        printf("%s (page %d) : %p to %p\n", zone_name, page_id++, (void *)page, (void *)page + page->length);
        total_size += print_page_blocks_ex(page);
        page = page->next;
    }
    return total_size;
}

void show_alloc_mem_ex(void)
{
    size_t total_size;

    total_size = 0;
    printf("\n================= Extended Memory Report =================\n");
    pthread_mutex_lock(&g_malloc.tiny.mutex);
    total_size += print_memory_zone_ex(g_malloc.tiny.pages, "TINY");
    pthread_mutex_unlock(&g_malloc.tiny.mutex);
    pthread_mutex_lock(&g_malloc.small.mutex);
    total_size += print_memory_zone_ex(g_malloc.small.pages, "SMALL");
    pthread_mutex_unlock(&g_malloc.small.mutex);
    pthread_mutex_lock(&g_malloc.large.mutex);
    total_size += print_memory_zone_ex(g_malloc.large.pages, "LARGE");
    pthread_mutex_unlock(&g_malloc.large.mutex);
    printf("Total allocated (used only): %zu bytes\n", total_size);
    printf("==========================================================\n\n");
}
