#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

# include "../printf_OK/ft_printf.h"
# include <stdbool.h>
# include <string.h>
# include <stdio.h>
# include <stddef.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <pthread.h>
# include <stdatomic.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdint.h>
# include <fcntl.h>

#  define MALLOC_NAME malloc
#  define FREE_NAME free
#  define REALLOC_NAME realloc
#  define REALLOCARRAY_NAME reallocarray
#  define CALLOC_NAME calloc
#  define STRDUP_NAME strdup

# define n 256   // taille en bytes pour etre considerer comme tiny malloc
# define m 4096 // taille en bytes pour etre considerer comme small malloc
# define NB_BLOCK 100
# define MEMORY_ALIGNMENT sizeof(size_t) // alignement de la memoire de 8bytes
# define BLOCK_HEADER_SIZE sizeof(t_block)
# define PAGE_HEADER_SIZE sizeof(t_page)

// arrondir un nombre au multiple de 8 superieur pour l'alignement
# define ALIGN(x) (((x) + (MEMORY_ALIGNMENT - 1)) & ~(MEMORY_ALIGNMENT - 1))

# define BLOCK_FLAG_FREE (1 << 0) // flag pour le free
# define BLOCK_FLAG_LAST (1 << 1) // flag pour le dernier block
# define BLOCK_FLAG_COLOR (1 << 2) // flag pour la couleur du block

# define SET_BLOCK_RED(block) ((block)->flags |= BLOCK_FLAG_COLOR)
# define SET_BLOCK_BLACK(block) ((block)->flags &= ~BLOCK_FLAG_COLOR)
# define IS_BLOCK_RED(block) (((block)->flags & BLOCK_FLAG_COLOR) != 0)
# define IS_BLOCK_BLACK(block) (((block)->flags & BLOCK_FLAG_COLOR) == 0)

#define COPY_BLOCK_COLOR(dst, src) \
	(dst)->flags = ((dst)->flags & ~BLOCK_FLAG_COLOR) | ((src)->flags & BLOCK_FLAG_COLOR)

# define SET_BLOCK_FREE(block) ((block)->flags |= BLOCK_FLAG_FREE)
// mettre le block comme free
# define SET_BLOCK_USE(block) ((block)->flags &= ~BLOCK_FLAG_FREE)
// mettre le block comme utilise
# define IS_BLOCK_FREE(block) (((block)->flags & BLOCK_FLAG_FREE) != 0)
// savoir si le block est free

# define SET_BLOCK_LAST(block) ((block)->flags |= BLOCK_FLAG_LAST)
// mettre le block comme le dernier block
# define SET_BLOCK_NOT_LAST(block) ((block)->flags &= ~BLOCK_FLAG_LAST)
// mettre le block comme n'est pas le dernier block
# define IS_BLOCK_LAST(block) (((block)->flags & BLOCK_FLAG_LAST) != 0)
// savoir si le block est le dernier block de la page

# define GET_BLOCK_SIZE(block) ((block)->size)
# define GET_BLOCK_TRUE_SIZE(block) ((block)->true_size)
// avoir la taille du block
# define SET_BLOCK_SIZE(block, sz) ((block)->size = sz)
// set la taille du block
# define SET_BLOCK_TRUE_SIZE(block, sz) ((block)->true_size = ALIGN(sz))

# define NEXT_BLOCK(block) \
	(IS_BLOCK_LAST(block) ? NULL : (void *)(block) + BLOCK_HEADER_SIZE \
			+ GET_BLOCK_TRUE_SIZE(block))

# define GET_BLOCK_PTR(block) ((void *)(block) + BLOCK_HEADER_SIZE)

typedef struct s_block
{
	size_t size;
	size_t	true_size;
	struct s_block *left;
	struct s_block *right;
	struct s_block *parent;
	struct s_block	*same_prev;
	struct s_block	*same_next;
	uint8_t		flags;
}				t_block;

typedef struct s_page
{
	size_t			length;
	t_block			*blocks; // pointe vers le premier block de la page
	struct s_page	*next; // pointe vers la prochaine page
	struct s_page	*prev; // pointe vers la prochaine page
}					t_page;

typedef struct s_page_block
{
    t_page  *page;
    t_block *block;
}   t_page_block;

typedef struct s_mutex_zone {
	size_t max_size_malloc;
	t_page *pages;
	t_page *last;
	t_block *root_free;
	t_block sentinel;
	pthread_mutex_t mutex;
} t_mutex_zone;

typedef struct s_malloc
{
	t_mutex_zone	tiny; // page liee au tiny malloc
	t_mutex_zone	small; // page liee au small malloc
	t_mutex_zone	large; // page liee au large malloc
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
void				initialize_blocks(t_block *block, size_t size_of_block);
void				split_block(t_block *block, size_t size, t_mutex_zone *zone);
bool				find_block(t_page *pages, void *ptr, t_block **out);
void				merge_block_with_next(t_mutex_zone *zone, t_block *block);
void				merge_block_with_prev(t_mutex_zone *zone, t_block **block, t_block *prev_block);
void				malloc_init(void);
void				*malloc_internal(size_t size);
void				free_internal(void *ptr);
void				*realloc_internal(void *ptr, size_t size);
bool				is_gonna_overflow(size_t nmemb, size_t size);
char				*strdup_internal(const char *s);
void				*lock_malloc(size_t size);
size_t				print_memory_zone(t_page *page_list, char *zone_name);
int					print_err(const char *msg);



t_block	*search_best_node(t_block *root, size_t size, t_block *NIL);
void				delete_node_tree(t_block **root, t_block *to_del, t_block *NIL);
void				insert_node_tree(t_block **root, t_block *to_add, t_block *NIL);

#endif