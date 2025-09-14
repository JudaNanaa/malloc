CC = cc
CFLAGS = -Wall -Wextra -g3 #-Werror -fPIC

SRCS_DIR = srcs/
SRCS = malloc.c free.c utils.c realloc.c calloc.c show_alloc_mem.c \
       show_alloc_mem_ex.c env_var.c reallocarray.c strdup.c main2.c # main.c

SRCS := $(addprefix $(SRCS_DIR), $(SRCS))

OBJS_DIR = .objs/
OBJS = $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

# all: $(LIBFT) $(NAME) $(LINK)
all : test

# --- Création de la lib malloc ---
$(NAME): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -L $(LIBFT_DIR) -lft -lpthread
	@echo "✅ $@ has been built"

# --- Lien symbolique ---
$(LINK): $(NAME)
	ln -sf $(NAME) $(LINK)
	@echo "🔗 Created symlink $(LINK) -> $(NAME)"

# --- Compilation des objets ---
$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Compilation de la libft ---
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

test: $(LIBFT) $(OBJS)
	$(CC) $(OBJS) -o test_malloc -lpthread  -L $(LIBFT_DIR) -lft

# --- Nettoyage ---
clean:
	rm -rf $(OBJS_DIR)
	@$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(LINK)
	@$(MAKE) fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
