#include "../includes/malloc_internal.h"

void	*calloc(size_t nmemb, size_t size)
{
	size_t aligned_size;
	void *ptr;

	aligned_size = ALIGN8((size + nmemb));
	ptr = malloc(aligned_size);
	if (ptr)
		ft_bzero(ptr, aligned_size);
	return (ptr);
}
