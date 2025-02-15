NAME = minishell
CC = cc
# CFLAGS = -Wall -Wextra -Werror -g
CFLAGS  = -Wall -Wextra -Werror \

READLINE = -lreadline

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a
# READLINE = -lreadline -lncurses

SRC_DIR = ./srcs
OBJ_DIR = ./objs

SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/signals.c \
	$(SRC_DIR)/env_utils.c \
	$(SRC_DIR)/builtins.c \
	$(SRC_DIR)/expansions.c \
	$(SRC_DIR)/parse.c \
	$(SRC_DIR)/execute.c \
	$(SRC_DIR)/redirections.c \
	$(SRC_DIR)/append_char.c \
	$(SRC_DIR)/lexer.c \

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	@echo "Making libft..."
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(READLINE) -o $(NAME)
	@echo "minishell compiled."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I./includes -I$(LIBFT_DIR) -c $< -o $@

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

test: all
	./combination_test/run_test.sh

.PHONY: all clean fclean re test
