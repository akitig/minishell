# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/04 19:21:54 by akunimot          #+#    #+#              #
#    Updated: 2025/02/08 17:27:18 by akunimot         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = splitstr
LIBNAME = libft.a
SRC = split.c \
      doller.c \
      utils.c \
      correct.c \
      correct_special.c \
      ## parser.c \

OBJ = $(SRC:.c=.o)

CC = gcc
#CFLAGS =
LDFLAGS = -pie

NONE='\033[0m'
GREEN='\033[32m'
GRAY='\033[2;37m'
CURSIVE='\033[3m'

all: $(NAME)

$(NAME): $(OBJ) $(LIBNAME)
	@echo $(CURSIVE)$(GRAY) "     - Compiling $(NAME)..." $(NONE)
	@$(CC) $(CFLAGS) $(OBJ) $(LIBNAME) -o $(NAME) $(LDFLAGS)
	@echo $(GREEN)"- Compiled -"$(NONE)

$(LIBNAME):
	@echo $(CURSIVE)$(GRAY) "     - Compiling LIBFT $(LIBNAME)..." $(NONE)
	@$(MAKE) -C ../../libft all
	@cp ../../libft/libft.a $(LIBNAME)
	@echo $(GREEN)"- Compiled -"$(NONE)

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo $(CURSIVE)$(GRAY) "     - Removing object files..." $(NONE)
	@$(MAKE) -C ../../libft clean
	@rm -rf $(OBJ)

fclean: clean
	@echo $(CURSIVE)$(GRAY) "     - Removing $(NAME) And $(LIBNAME)..." $(NONE)
	@$(MAKE) -C ../../libft fclean
	@rm -rf $(NAME) $(LIBNAME)

re: fclean all

