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

# define n 64   // taille en bytes pour etre considerer comme tiny my_malloc
# define m 1024 // taille en bytes pour etre considerer comme small my_malloc
# define NB_BLOCK 100
# define MEMORY_ALIGNMENT sizeof(size_t) // alignement de la memoire de 8bytes
# define BLOCK_HEADER_SIZE sizeof(t_block)
// arrondir un nombre au multiple de 8 superieur pour l'alignement
# define ALIGN(x) (((x) + (MEMORY_ALIGNMENT - 1)) & ~(MEMORY_ALIGNMENT - 1))

# define BLOCK_FLAG_FREE (1 << 0) // flag pour le my_free
# define BLOCK_FLAG_LAST (1 << 1) // flag pour le dernier block

# define SET_BLOCK_FREE(block) ((block)->flags |= BLOCK_FLAG_FREE)
// mettre le block comme my_free
# define SET_BLOCK_USE(block) ((block)->flags &= ~BLOCK_FLAG_FREE)
// mettre le block comme utilise
# define IS_BLOCK_FREE(block) (((block)->flags & BLOCK_FLAG_FREE) != 0)
// savoir si le block est my_free

# define SET_BLOCK_LAST(block) ((block)->flags |= BLOCK_FLAG_LAST)
// mettre le block comme le dernier block
# define SET_BLOCK_NOT_LAST(block) ((block)->flags &= ~BLOCK_FLAG_LAST)
// mettre le block comme n'est pas le dernier block
# define IS_BLOCK_LAST(block) (((block)->flags & BLOCK_FLAG_LAST) != 0)
// savoir si le block est le dernier block de la page

# define GET_BLOCK_SIZE(block) (ALIGN((block)->size))
// avoir la taille du block
# define SET_BLOCK_SIZE(block, sz) ((block)->size = sz)
// set la taille du block

# define NEXT_BLOCK(block) \
	(IS_BLOCK_LAST(block) ? NULL : (void *)(block) + BLOCK_HEADER_SIZE \
			+ GET_BLOCK_SIZE(block))

# define GET_BLOCK_PTR(block) ((void *)(block) + BLOCK_HEADER_SIZE)

typedef struct s_block
{
	size_t size; // changer ca en size_t
	uint8_t		flags;
	struct s_block *next_free;
	struct s_block *prev_free;
}				t_block;

typedef struct s_free_list {
    t_block *head;  // pointeur vers premier block libre
	t_block *last;
} t_free_list;

typedef struct s_page
{
	size_t			length;
	t_block			*blocks; // pointe vers le premier block de la page
	t_free_list		free_lists;
	struct s_page	*next; // pointe vers la prochaine page
}					t_page;

typedef struct s_page_block
{
    t_page  *page;
    t_block *block;
}   t_page_block;

typedef struct s_mutex_zone {
	t_page *pages;
	t_page *last;
	pthread_mutex_t mutex;
} t_mutex_zone;

typedef struct s_malloc
{
	t_mutex_zone	tiny; // page liee au tiny malloc
	t_mutex_zone	small; // page liee au small malloc
	t_mutex_zone	large; // page liee au large malloc
	size_t			tiny_malloc_size;
	size_t			small_malloc_size;
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
void				split_block(t_page *page, t_block *block, size_t size);
bool				find_block(t_page *pages, void *ptr, t_page_block *out);
void				merge_block(t_page *page, t_block *block, t_block *prev_block);
void				malloc_init(void);
void				*malloc_internal(size_t size);
void				free_internal(void *ptr);
void				*realloc_internal(void *ptr, size_t size);
bool				is_gonna_overflow(size_t nmemb, size_t size);
char				*strdup_internal(const char *s);
void				add_block_to_free_list(t_page *page, t_block *block);
void				remove_block_free_list(t_page *page, t_block *block);

#endif