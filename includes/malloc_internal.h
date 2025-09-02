#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

# include "../libft/libft.h"

# define n 64   // taille en bytes pour etre considerer comme tiny malloc
# define m 1024 // taille en bytes pour etre considerer comme small malloc

typedef struct s_block
{
	size_t size;          // taile du block
	int free;             // es ce que le block est libre ou pas
	struct s_block *next; // pointe vers le prochain block
}				t_block;

typedef struct s_malloc_page
{
	size_t size;                // taile de la page
	t_block *first;             // pointe vers le premier block de la page
	struct s_malloc_page *next; // pointe vers la prochaine page
}				t_malloc_page;

typedef struct s_malloc
{
	t_malloc_page *tiny;  // page liee au tiny malloc
	t_malloc_page *small; // page liee au small malloc
	t_malloc_page *large; // page liee au large malloc
}				t_malloc;

extern t_malloc	g_malloc;

#endif