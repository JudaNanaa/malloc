#include "../includes/malloc_internal.h"
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

t_page	*next_page(t_page *current_page)
{
	t_page	*next;

	pthread_mutex_lock(&g_malloc_lock);
	next = current_page->next;
	pthread_mutex_unlock(&g_malloc_lock);
	return (next);
}

size_t	get_nb_free_block_in_page(t_page *page)
{
	size_t	nb_block_free;

	pthread_mutex_lock(&page->lock);
	nb_block_free = page->nb_block_free;
	pthread_mutex_unlock(&page->lock);
	return (nb_block_free);
}

void	incr_nb_free_block_in_page(t_page *page)
{
	pthread_mutex_lock(&page->lock);
	page->nb_block_free++;
	pthread_mutex_unlock(&page->lock);
}

void	decr_nb_free_block_in_page(t_page *page)
{
	pthread_mutex_lock(&page->lock);
	page->nb_block_free--;
	pthread_mutex_unlock(&page->lock);
}

void block_page(t_page *page)
{
	pthread_mutex_lock(&page->lock);
}

void release_page(t_page *page)
{
	pthread_mutex_unlock(&page->lock);
}