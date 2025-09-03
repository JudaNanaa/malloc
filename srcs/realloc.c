#include "../includes/malloc_internal.h"
#include <stdlib.h>



void	*realloc(void *ptr, size_t size) {
	if (ptr == NULL)
		return malloc(size);
	else if (size == 0)
	{
		free(ptr);
		return NULL;
	}
	
}