#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

# include "../libft/libft.h"
# include "../libft/printf_OK/ft_printf.h"
#include <bits/pthreadtypes.h>
# include <stdbool.h>
# include <stddef.h>
# include <sys/mman.h>
# include <pthread.h>
#include <stdatomic.h>

# define n 64   // taille en bytes pour etre considerer comme tiny malloc
# define m 1024 // taille en bytes pour etre considerer comme small malloc
# define NB_BLOCK 100
# define MEMORY_ALIGNMENT sizeof(size_t) // alignement de la memoire de 8bytes
# define BLOCK_HEADER_SIZE sizeof(t_block)
// arrondir un nombre au multiple de 8 superieur pour l'alignement
# define ALIGN(x) (((x) + (MEMORY_ALIGNMENT - 1)) & ~(MEMORY_ALIGNMENT - 1))

# define BLOCK_FREE 0 // flag pour le free
# define BLOCK_LAST 1 // flag pour le dernier block

# define SET_BLOCK_FREE(block) ((block)->metadata[BLOCK_FREE] = true)
// mettre le block comme free
# define SET_BLOCK_USE(block) ((block)->metadata[BLOCK_FREE] = false)
// mettre le block comme utilise
# define IS_BLOCK_FREE(block) ((block)->metadata[BLOCK_FREE])
// savoir si le block est free

# define SET_BLOCK_LAST(block) ((block)->metadata[BLOCK_LAST] = true)
// mettre le block comme le dernier block
# define SET_BLOCK_NOT_LAST(block) ((block)->metadata[BLOCK_LAST] = false)
// mettre le block comme n'est pas le dernier block
# define IS_BLOCK_LAST(block) ((block)->metadata[BLOCK_LAST])
// savoir si le block est le dernier block de la page

# define GET_BLOCK_SIZE(block) (ALIGN((block)->size))
// avoir la taille du block
// size need to be aligned !!
# define SET_BLOCK_SIZE(block, sz) ((block)->size = sz)
// set la taille du block

# define NEXT_BLOCK(block) \
	(IS_BLOCK_LAST(block) ? NULL : (void *)(block) + BLOCK_HEADER_SIZE \
			+ GET_BLOCK_SIZE(block))

# define GET_BLOCK_PTR(block) ((void *)(block) + BLOCK_HEADER_SIZE)

# define dbg(to_print) ft_printf_fd(STDERR_FILENO, "%s\n", to_print)

typedef struct s_block
{
	unsigned int size;
	char		metadata[4];
}				t_block;

typedef struct s_page
{
	size_t			length;
	t_block			*blocks; // pointe vers le premier block de la page
	size_t			nb_block_free;
	pthread_mutex_t lock;
	struct s_page	*next; // pointe vers la prochaine page
}					t_page;

typedef struct s_malloc
{
	t_page			*tiny;  // page liee au tiny malloc
	t_page			*small; // page liee au small malloc
	t_page			*large; // page liee au large malloc
	int				fail_size;
	atomic_bool			set;
	bool			verbose;
	bool			no_defrag;
	int				trace_file_fd;
}					t_malloc;

extern t_malloc	g_malloc;
extern pthread_mutex_t g_malloc_lock;

t_block				*page_find_block_by_ptr(t_page *page, void *ptr,
						t_block **prev_out);
void				initialize_blocks(t_block **block, size_t size);
int					split_block(t_block *block, size_t size);
t_page				*find_page_by_block(t_page *pages, t_block *block);
t_block				*find_block(t_page *pages, void *ptr);
int					merge_block(t_block *block, t_block *prev_block);
void				malloc_init(void);
void				*malloc_internal(size_t size);
void				free_internal(void *ptr);
void				*realloc_internal(void *ptr, size_t size);
bool				is_gonna_overflow(size_t nmemb, size_t size);
char				*strdup_internal(const char *s);

// t_page *next_page(t_page *current_page);
size_t	get_nb_free_block_in_page(t_page *page);
void block_page(t_page *page);
void release_page(t_page *page);


#endif