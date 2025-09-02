CC = cc

CFLAGS = -Wall -Wextra -Werror -g3

SRCS_DIR = srcs/

SRCS = $(addprefix srcs/, main.c malloc.c)

OBJS_DIR = .objs/

OBJS = $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)

NAME = libft_malloc_$(HOSTTYPE).so

LIBFT = libft.a
LIBFT_DIR = libft

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif


# changer le all car test
all: $(LIBFT) test

test: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o test_malloc -L $(LIBFT_DIR) -lft

$(NAME): $(OBJS)
	ar -rcs $(NAME) $(OBJS)
	@echo la lib a bien ete cree

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(LIBFT) :
	make -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJS_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -rf $(NAME)
	make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re