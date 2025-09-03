#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	char *dest;

	dest = malloc(sizeof(char) * 10);
	dest[0] = '\0';
	strcpy(dest, "bonjour");
	printf("dest == %p\n", dest);
	
	dest = realloc(dest, 11);
	printf("return de realloc %p \n", dest);
	// printf("dest == %s\n", dest);
}