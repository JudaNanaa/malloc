CC      = cc
CFLAGS  = -Wall -Wextra -Werror -fPIC

SRCS    = car_fr.c ft_printf.c hex_fr.c int_fr.c mem_fr.c monitoring.c \
          putnbr.c str_fr.c uns_fr.c len_functions.c

OBJS_DIR = .objs
OBJS     = $(SRCS:%.c=$(OBJS_DIR)/%.o)

NAME    = libftprintf.a

# --- Colors ---
YELLOW=	$(shell tput -Txterm setaf 3)
GREEN= 	$(shell tput -Txterm setaf 2)
BLUE=	$(shell tput -Txterm setaf 6)
END= 	$(shell tput -Txterm sgr0)

all: $(NAME)

$(NAME): $(OBJS)
	ar crs $(NAME) $(OBJS)
	@echo "$(GREEN)✅ $(NAME) has been built$(END)"

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(OBJS_DIR)
	@echo "$(YELLOW)Compiling $(notdir $<) ...$(END)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJS_DIR)
	@echo "$(BLUE)🧹 Objects removed$(END)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(BLUE)🗑️  $(NAME) removed$(END)"

re: fclean all

.PHONY: all clean fclean re
