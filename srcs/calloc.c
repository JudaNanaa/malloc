#include "../includes/malloc_internal.h"

void	*calloc_internal(size_t nmemb, size_t size)
{
	void	*ptr;

	ptr = malloc_internal(nmemb * size);
	if (ptr)
		bzero(ptr, nmemb * size);
	return (ptr);
}

void *calloc(size_t nmemb, size_t size) {
    void *ptr;

    if (is_gonna_overflow(nmemb, size)) {
        if (g_malloc.verbose) {
            ft_printf_fd(STDERR_FILENO,
                "[ERROR] calloc overflow detected\n"
                "\t-> nmemb: %u, size: %u (multiplication too large)\n", nmemb, size);
        }
        if (g_malloc.trace_file_fd != -1) {
            ft_printf_fd(g_malloc.trace_file_fd,
                "[ERROR] calloc overflow (nmemb: %u, size: %u)\n", nmemb, size);
        }
        return NULL;
    }

    ptr = calloc_internal(nmemb, size);

    if (g_malloc.verbose) {
        ft_printf_fd(STDERR_FILENO,
            "[DEBUG] calloc(nmemb: %u, size: %u) -> %p (%zu bytes)\n",
            nmemb, size, ptr, nmemb * size);

        ft_printf_fd(STDERR_FILENO, "Stack trace (most recent first):\n");
    }

    if (g_malloc.trace_file_fd != -1) {
        ft_printf_fd(g_malloc.trace_file_fd,
            "calloc(nmemb: %u, size: %u) -> %p\n",
            nmemb, size, ptr);
    }

    return ptr;
}

