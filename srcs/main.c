#include "../includes/lib_malloc.h"
#include "../includes/malloc_internal.h"
# include "../libft/printf_OK/ft_printf.h"
#include <stdalign.h>
#include <stddef.h>
#include <unistd.h>

int main(void)
{
    printf("=== TEST MALLOC ===\n");
    char *msg = (char *)malloc(20);
    if (!msg) {
        perror("malloc failed");
        return 1;
    }
    strcpy(msg, "Hello malloc!");
    printf("msg = %s\n", msg);

    printf("\n=== TEST REALLOC (expand) ===\n");
    char *bigger = (char *)realloc(msg, 40); // agrandir
    if (!bigger) {
        perror("realloc failed");
        free(msg); // free old if realloc failed
        return 1;
    }
    strcat(bigger, " + realloc works!");
    printf("bigger = %s\n", bigger);

    printf("\n=== TEST REALLOC (shrink) ===\n");
    char *smaller = (char *)realloc(bigger, 10); // réduire
    if (!smaller) {
        perror("realloc failed");
        free(bigger);
        return 1;
    }
    printf("smaller (truncated) = %.*s\n", 9, smaller);

    printf("\n=== TEST FREE ===\n");
    free(smaller);
    printf("Memory freed successfully!\n");

    printf("\n=== TEST FREE INVALID ===\n");
    // Attention : ceci doit provoquer ton abort() custom
    // free(smaller); // décommenter pour tester le "double free detected"

    return 0;
}