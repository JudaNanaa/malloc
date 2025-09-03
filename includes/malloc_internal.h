#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

# include "../libft/libft.h"
# include "../libft/printf_OK/ft_printf.h"
# include <stdbool.h>
# include <stddef.h>
# include <sys/mman.h>

# define n 64   // taille en bytes pour etre considerer comme tiny malloc
# define m 1024 // taille en bytes pour etre considerer comme small malloc
# define NB_BLOCK 100

typedef struct s_block
{
	void *ptr;            // ponteur retourne par malloc
	size_t size;          // taile du block
	bool is_free;         // es ce que le block est libre ou pas
	struct s_block *next; // pointeur vers le prochain block
	struct s_block *prev; // pointeur vers le precedent block
}				t_block;

typedef struct s_page
{
	size_t		length;
	t_block *blocks; // pointe vers le premier block de la page
	size_t		nb_block;
	size_t		nb_block_free;
	struct s_page *next; // pointe vers la prochaine page
}				t_page;

typedef struct s_malloc
{
	t_page *tiny;  // page liee au tiny malloc
	t_page *small; // page liee au small malloc
	t_page *large; // page liee au large malloc
}				t_malloc;

extern t_malloc	g_malloc;

t_block			*page_find_block_by_ptr(t_page *page, void *ptr);
void			initialize_blocks(t_block **block, size_t size);
int				split_block(t_block *block, size_t size);
t_page *find_page_by_block(t_page *pages, t_block *block);
t_block	*find_block(t_page *pages, void *ptr);

#endif