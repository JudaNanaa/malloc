# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: madamou <madamou@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/25 17:05:20 by itahri            #+#    #+#              #
#    Updated: 2025/09/16 17:36:08 by madamou          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC      = cc
CFLAGS  = -Wall -Wextra -Werror  -fPIC

SRCS    = car_fr.c ft_printf.c hex_fr.c int_fr.c mem_fr.c monitoring.c \
          putnbr.c str_fr.c uns_fr.c len_functions.c

OBJS_DIR = .objs
OBJS     = $(SRCS:%.c=$(OBJS_DIR)/%.o)

NAME    = libftprintf.a

all: $(NAME)

$(NAME): $(OBJS)
	ar crs $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
