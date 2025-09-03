#include "../includes/lib_malloc.h"
# include "../printf_OK/ft_printf.h"
#include <stdlib.h>

int	main(void)
{
	char *dest;

	(void)dest;
	int i = 0;
	while (i < 150) {
		dest = malloc(sizeof(char) * 10);
		ft_printf("dest == %p\n", dest);
		dest[0] = 'c';
		dest[1] = '\0';
		i++;
		ft_printf("dest == %s\n", dest);
		free(dest);
	}
	
	return (0);
}