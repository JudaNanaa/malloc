#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(void)
{
	char *dest;
	char *dest2;

	(void)dest;
	int i = 0;
	while (i < 150) {
		dest = malloc(sizeof(char) * 645);
		dest2 = malloc(sizeof(char) * 645);

		printf("dest == %p\n", dest);
		printf("dest2 == %p\n", dest2);
		
		dest[0] = 'c';
		dest[1] = '\0';

		dest2[0] = 'c';
		dest2[1] = '\0';
		i++;
		dest2 = realloc(dest2 - 10, 10);
		printf("dest == %s\n", dest);
		printf("dest2 == %s\n", dest2);
		free(dest);
		free(dest2);

	}
	
	return (0);
}