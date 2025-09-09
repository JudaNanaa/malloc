#ifndef LIB_MALLOC_H
# define LIB_MALLOC_H

# include <stddef.h>
# include <sys/mman.h>
# include <unistd.h>

void	*my_malloc(size_t size);

void	my_free(void *ptr);

void	*realloc(void *ptr, size_t size);

void	*reallocarray(void *ptr, size_t nmemb, size_t size);

void	*my_calloc(size_t nmemb, size_t size);

void	show_alloc_mem(void);

void	show_alloc_mem_ex(void);

char	*strdup(const char *s);

#endif