
#include "../includes/malloc_internal.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

t_malloc	g_malloc = {NULL, NULL, NULL};

void	*malloc(size_t size)
{
	(void)size;
	int sz = sysconf(_SC_PAGESIZE);

	ft_putstr("size == ");
	ft_putnbr(sz);
	return (NULL);
}
