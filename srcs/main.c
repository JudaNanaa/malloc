#include "../includes/lib_malloc.h"
# include "../libft/printf_OK/ft_printf.h"

int main(void)
{
    printf("=== TEST MALLOC ===\n");
    char *msg = (char *)malloc(20);
    if (!msg) {
        perror("malloc failed");
        return 1;
    }
	show_alloc_mem();
    strcpy(msg, "Hello malloc!");
    printf("msg = %s\n", msg);

    printf("\n=== TEST REALLOC (expand) ===\n");
    char *bigger = (char *)realloc(msg, 40); // agrandir
    if (!bigger) {
        perror("realloc failed");
        free(msg); // free old if realloc failed
        return 1;
    }
	show_alloc_mem();

    strcat(bigger, " + realloc works!");
    printf("bigger = %s\n", bigger);

    printf("\n=== TEST REALLOC (shrink) ===\n");
    char *smaller = (char *)realloc(bigger, 10); // réduire
    if (!smaller) {
        perror("realloc failed");
        free(bigger);
        return 1;
    }
	show_alloc_mem();

    printf("smaller (truncated) = %.*s\n", 9, smaller);

    printf("\n=== TEST FREE ===\n");
    free(smaller);
    printf("Memory freed successfully!\n");

	show_alloc_mem();

    printf("=== TEST LARGE MALLOCS (> 1024 bytes) ===\n");

    // 1. Allocation simple d’un large bloc
    size_t big_size = 2048; // > 1024 donc doit aller dans large malloc
    char *large1 = (char *)malloc(big_size);
    if (!large1) {
        perror("malloc large1 failed");
        return 1;
    }
	show_alloc_mem();
	
    memset(large1, 'A', big_size - 1);
    large1[big_size - 1] = '\0';
    printf("large1 allocated (%zu bytes): first char = %c, last char = %c\n",
           big_size, large1[0], large1[big_size - 2]);

    // 2. Realloc to a larger size (should trigger a new mmap)
    size_t bigger_size = 4096;
    char *large2 = (char *)realloc(large1, bigger_size);
    if (!large2) {
        perror("realloc large2 failed");
        free(large1);
        return 1;
    }
    memset(large2 + big_size, 'B', bigger_size - big_size - 1);
    large2[bigger_size - 1] = '\0';
    printf("large2 reallocated (%zu bytes): mid char = %c, last char = %c\n",
           bigger_size, large2[big_size], large2[bigger_size - 2]);

    // 3. Realloc to a smaller size (should shrink)
    size_t smaller_size = 1500; // still > 1024, still large
    char *large3 = (char *)realloc(large2, smaller_size);
    if (!large3) {
        perror("realloc large3 failed");
        free(large2);
        return 1;
    }
    printf("large3 reallocated smaller (%zu bytes): first char = %c\n",
           smaller_size, large3[0]);

    // 4. Free the large block
    free(large3);
    printf("large3 freed successfully.\n");

    // 5. Edge case: allocation juste au-dessus du seuil
    char *edge = (char *)malloc(1025);
    if (!edge) {
        perror("malloc edge failed");
        return 1;
    }
    printf("edge allocated (1025 bytes): OK\n");
    free(edge);

    printf("\n=== TEST FREE INVALID ===\n");
    // Attention : ceci doit provoquer ton abort() custom
    // free(smaller); // décommenter pour tester le "double free detected"

    return 0;
}