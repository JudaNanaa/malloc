CC = cc
CFLAGS = -Wall -Wextra -g3 -pg -Werror -fPIC

SRCS_DIR = srcs/
SRCS = malloc.c free.c utils.c realloc.c calloc.c show_alloc_mem.c \
       show_alloc_mem_ex.c env_var.c reallocarray.c strdup.c

SRCS := $(addprefix $(SRCS_DIR), $(SRCS))

OBJS_DIR = .objs/
OBJS = $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

all:$(NAME) $(LINK)

# --- Création de la lib malloc ---
$(NAME): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -L $(LIBFT_DIR) -lft -pthread
	@echo "✅ $@ has been built"

# --- Lien symbolique ---
$(LINK): $(NAME)
	ln -sf $(NAME) $(LINK)
	@echo "🔗 Created symlink $(LINK) -> $(NAME)"

# --- Compilation des objets ---
$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Nettoyage ---
clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(LINK)

re: fclean all

.PHONY: all clean fclean re
