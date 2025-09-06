#ifndef LIB_MALLOC_H
# define LIB_MALLOC_H

# include <sys/mman.h>
# include <unistd.h>
#include <stddef.h>


void *malloc_wrapper(size_t size, const char *file, int line, const char *func);

#define malloc(size) malloc_wrapper(size, __FILE__, __LINE__, __func__)


void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void	*calloc(size_t nmemb, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
#endif