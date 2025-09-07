
#include "../includes/malloc_internal.h"

char	*strdup_internal(const char *s)
{
	char	*dest;
	size_t	len;

	len = strlen(s);
	dest = malloc_internal(len + 1);
	if (dest == NULL)
		return (dest);
	return (memmove(dest, s, len + 1));
}

char *strdup(const char *s)
{
    char *dest;

    dest = strdup_internal(s);

    if (g_malloc.verbose) {
        ft_printf_fd(STDERR_FILENO,
            "[DEBUG] strdup(\"%s\") -> %p\n", s, dest);
    }

    if (g_malloc.trace_file_fd != -1) {
        ft_printf_fd(g_malloc.trace_file_fd,
            "strdup(\"%s\") -> %p\n", s, dest);
    }

    return dest;
}
