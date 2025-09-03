#include "../includes/lib_malloc.h"
# include "../libft/printf_OK/ft_printf.h"

int	main(void)
{
	char *dest;

	(void)dest;
	int i = 0;
	while (i < 150) {
		dest = malloc(sizeof(char) * 64);
		ft_printf("dest == %p\n", dest);
		dest[0] = 'c';
		dest[1] = '\0';
		i++;
		ft_printf("dest == %s\n", dest);
		free(dest);
	}
	
	return (0);
}