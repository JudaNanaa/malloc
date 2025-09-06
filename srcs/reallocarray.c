#include "../includes/malloc_internal.h"

void *reallocarray_wrapper(void *ptr, size_t nmemb, size_t size,
                           const char *file, int line, const char *func) {
    void *new_ptr;

    if (is_gonna_overflow(nmemb, size)) {
        if (g_malloc.verbose) {
            ft_printf_fd(STDERR_FILENO,
                "[ERROR] reallocarray overflow detected at %s:%d in %s\n"
                "\t-> nmemb: %u, size: %u (multiplication too large)\n",
                file, line, func, nmemb, size);
        }
        if (g_malloc.trace_file_fd != -1) {
            ft_printf_fd(g_malloc.trace_file_fd,
                "[ERROR] reallocarray overflow at %s:%d in %s "
                "(nmemb: %u, size: %u)\n",
                file, line, func, nmemb, size);
        }
        return NULL;
    }

    // Appel à ton realloc interne

	// ft_printf_fd(2, "ptr == %p\n", ptr);

    new_ptr = realloc_internal(ptr, nmemb * size);

    if (g_malloc.verbose) {
        ft_printf_fd(STDERR_FILENO,
            "[DEBUG] reallocarray(ptr: %p, nmemb: %u, size: %u) called at %s:%d in %s\n"
            "\t-> allocated %u bytes at %p\n",
            ptr, nmemb, size, file, line, func, nmemb * size, new_ptr);
    }

    if (g_malloc.trace_file_fd != -1) {
        ft_printf_fd(g_malloc.trace_file_fd,
            "reallocarray(ptr: %p, nmemb: %u, size: %u) at %s:%d in %s -> %p\n",
            ptr, nmemb, size, file, line, func, new_ptr);
    }

    return new_ptr;
}
