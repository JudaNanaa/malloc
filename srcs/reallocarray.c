#include "../includes/malloc_internal.h"

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    void *new_ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
    if (is_gonna_overflow(nmemb, size)) {
        if (g_malloc.verbose) {
            dprintf(STDERR_FILENO,
                "[ERROR] reallocarray overflow detected\n\t-> nmemb: %zu, size: %zu (multiplication too large)\n", nmemb, size);
        }
        if (g_malloc.trace_file_fd != -1) {
			dprintf(g_malloc.trace_file_fd,
                "[ERROR] reallocarray overflow (nmemb: %zu, size: %zu)\n", nmemb, size);
        }
        return NULL;
    }

    new_ptr = realloc_internal(ptr, nmemb * size);

    if (g_malloc.verbose) {
        dprintf(STDERR_FILENO,
            "[DEBUG] reallocarray(ptr: %p, nmemb: %zu, size: %zu)\n\t-> allocated %zu bytes at %p\n",
            ptr, nmemb, size, nmemb * size, new_ptr);
    }

    if (g_malloc.trace_file_fd != -1) {
        dprintf(g_malloc.trace_file_fd,
            "reallocarray(ptr: %p, nmemb: %zu, size: %zu) -> %p\n",
            ptr, nmemb, size, new_ptr);
    }

    return new_ptr;
}
