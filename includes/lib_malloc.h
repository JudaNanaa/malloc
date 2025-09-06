#ifndef LIB_MALLOC_H
# define LIB_MALLOC_H

# include <sys/mman.h>
# include <unistd.h>
#include <stddef.h>


void *malloc_wrapper(size_t size, const char *file, int line, const char *func);

#define malloc(size) malloc_wrapper(size, __FILE__, __LINE__, __func__)

void free_wrapper(void *ptr, const char *file, int line, const char *func);

#define free(ptr) free_wrapper(ptr, __FILE__, __LINE__, __func__)

void *realloc_wrapper(void *ptr, size_t size, const char *file, int line, const char *func);

# define realloc(ptr, size) realloc_wrapper(ptr, size, __FILE__, __LINE__, __func__)

void *calloc_wrapper(size_t nmemb, size_t size, const char *file, int line, const char *func);

# define calloc(nmemb, size) calloc_wrapper(nmemb, size, __FILE__, __LINE__, __func__)

void	show_alloc_mem(void);

void show_alloc_mem_ex_wrapper(const char *file, int line, const char *func);

# define show_alloc_mem_ex() show_alloc_mem_ex_wrapper(__FILE__, __LINE__, __func__) 
#endif