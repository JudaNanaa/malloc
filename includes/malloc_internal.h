#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

# include "../printf_OK/ft_printf.h"
#include <stdbool.h>

# define n 64   // taille en bytes pour etre considerer comme tiny malloc
# define m 1024 // taille en bytes pour etre considerer comme small malloc
# define NB_BLOCK 100

typedef struct s_block
{
	void *ptr;
	size_t size;          // taile du block
	bool is_free;             // es ce que le block est libre ou pas
}				t_block;

typedef struct s_page
{
	size_t nb_alloc;                // taile de la page
	t_block blocks[NB_BLOCK];             // tableau de blocks
	struct s_page *next; // pointe vers la prochaine page
}				t_page;

typedef struct s_malloc
{
	t_page *tiny;  // page liee au tiny malloc
	t_page *small; // page liee au small malloc
	t_page *large; // page liee au large malloc
}				t_malloc;

extern t_malloc	g_malloc;

#endif