#include "../includes/malloc_internal.h"

void	*calloc_internal(size_t nmemb, size_t size)
{
	void	*ptr;

	ptr = lock_malloc(nmemb *size);
	if (ptr)
		bzero(ptr, nmemb * size);
	return (ptr);
}

void *calloc(size_t nmemb, size_t size) {
    void *ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);	
    if (is_gonna_overflow(nmemb, size)) {
        if (g_malloc.verbose) {
            dprintf(STDERR_FILENO,
                "[ERROR] calloc overflow detected\n"
                "\t-> nmemb: %zu, size: %zu (multiplication too large)\n", nmemb, size);
        }
        if (g_malloc.trace_file_fd != -1) {
            dprintf(g_malloc.trace_file_fd,
                "[ERROR] calloc overflow (nmemb: %zu, size: %zu)\n", nmemb, size);
        }
        return NULL;
    }

    ptr = calloc_internal(nmemb, size);

    if (g_malloc.verbose) {
        dprintf(STDERR_FILENO,
            "[DEBUG] calloc(nmemb: %zu, size: %zu) -> %p (%zu bytes)\n",
            nmemb, size, ptr, nmemb * size);

        dprintf(STDERR_FILENO, "Stack trace (most recent first):\n");
    }

    if (g_malloc.trace_file_fd != -1) {
        dprintf(g_malloc.trace_file_fd,
            "calloc(nmemb: %zu, size: %zu) -> %p\n",
            nmemb, size, ptr);
    }

    return ptr;
}

