CC = cc
CFLAGS = -Wall -Wextra -Werror -fPIC -fvisibility=hidden -g3

RBTREE = $(addprefix rbTree/, deletion.c insertion.c rotation.c search.c)

SRCS_DIR = srcs/
SRCS = malloc.c free.c utils.c realloc.c calloc.c show_alloc_mem.c \
       show_alloc_mem_ex.c env_var.c reallocarray.c strdup.c $(RBTREE)
SRCS := $(addprefix $(SRCS_DIR), $(SRCS))

OBJS_DIR = .objs/
OBJS = $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)

PRINTF_DIR = printf_OK/
PRINTF = libftprintf.a

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

# --- Colors ---
YELLOW=	$(shell tput -Txterm setaf 3)
GREEN= 	$(shell tput -Txterm setaf 2)
BLUE=	$(shell tput -Txterm setaf 6)
END= 	$(shell tput -Txterm sgr0)

all: $(PRINTF) $(NAME) $(LINK)

# --- Création de la lib malloc ---
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -shared -o $@ $(OBJS) -L $(PRINTF_DIR) -lftprintf -pthread
	@echo "$(BLUE)✅ $@ has been built$(END)"

# --- Lien symbolique ---
$(LINK): $(NAME)
	@ln -sf $(NAME) $(LINK)
	@echo "$(GREEN)🔗 Created symlink $(LINK) -> $(NAME)$(END)"

# --- Compilation des objets ---
$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	@echo "$(YELLOW)Compiling $(notdir $<) ...$(END)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(PRINTF) :
	@make -sC $(PRINTF_DIR)

# --- Nettoyage ---
clean:
	@rm -rf $(OBJS_DIR)
	@make clean -sC $(PRINTF_DIR)
	@echo "$(BLUE)🧹 Objects removed (libft_malloc + printf)$(END)"

fclean: clean
	@rm -f $(NAME) $(LINK)
	@make fclean -sC $(PRINTF_DIR)
	@echo "$(BLUE)🗑️  $(NAME) and symlink removed$(END)"

re: fclean
	@echo "$(BLUE)♻️  Rebuilding project...$(END)"
	@$(MAKE) -s all

.PHONY: all clean fclean re
