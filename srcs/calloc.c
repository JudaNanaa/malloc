#include "../includes/malloc_internal.h"
#include <stdio.h>
#include <unistd.h>

void	*calloc_internal(size_t nmemb, size_t size)
{
	void	*ptr;

	ptr = malloc_internal(nmemb * size);
	if (ptr)
		bzero(ptr, size);
	return (ptr);
}

void *calloc_wrapper(size_t nmemb, size_t size, const char *file, int line, const char *func) {
	void *ptr;

	ptr = calloc_internal(nmemb, size);
    if (g_malloc.verbose) {
        ft_printf_fd(STDERR_FILENO, "[DEBUG] calloc(nmemb: %u, size: %u) called at %s:%d in %s\n"
            "\t-> allocated %u bytes at %p\n",
            nmemb, size, file, line, func, nmemb * size, ptr);
    }

    if (g_malloc.trace_file_fd != -1) {
        ft_printf_fd(g_malloc.trace_file_fd,
            "calloc(nmemb: %u, size: %u) at %s:%d in %s -> %p\n",
            nmemb, size, file, line, func, ptr);
    }

    return ptr;
}
