#include "../includes/malloc_internal.h"

__attribute__((visibility("default")))
void *REALLOCARRAY_NAME(void *ptr, size_t nmemb, size_t size) {
    void *new_ptr;

	pthread_mutex_lock(&g_malloc_lock);
	if (!g_malloc.set)
		malloc_init();
	pthread_mutex_unlock(&g_malloc_lock);
    if (is_gonna_overflow(nmemb, size)) {
        if (g_malloc.verbose) {
            ft_printf_fd(STDERR_FILENO,
                "[ERROR] reallocarray overflow detected\n\t-> nmemb: %u, size: %u (multiplication too large)\n", nmemb, size);
        }
        if (g_malloc.trace_file_fd != -1) {
			ft_printf_fd(g_malloc.trace_file_fd,
                "[ERROR] reallocarray overflow (nmemb: %u, size: %u)\n", nmemb, size);
        }
        return NULL;
    }

    new_ptr = realloc_internal(ptr, nmemb * size);

    if (g_malloc.verbose) {
        ft_printf_fd(STDERR_FILENO,
            "[DEBUG] reallocarray(ptr: %p, nmemb: %u, size: %u)\n\t-> allocated %u bytes at %p\n",
            ptr, nmemb, size, nmemb * size, new_ptr);
    }

    if (g_malloc.trace_file_fd != -1) {
        ft_printf_fd(g_malloc.trace_file_fd,
            "reallocarray(ptr: %p, nmemb: %u, size: %u) -> %p\n",
            ptr, nmemb, size, new_ptr);
    }

    return new_ptr;
}
