#include "../includes/malloc_internal.h"
#include <pthread.h>

t_page *next_page(t_page *current_page)
{
	t_page *next;

	pthread_mutex_lock(&g_malloc_lock);
	next = current_page->next;
	pthread_mutex_unlock(&g_malloc_lock);
	return next;
}
