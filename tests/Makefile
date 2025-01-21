NAME = a
MLX = -Lminilibx-linux minilibx-linux/libmlx_Linux.a -L/usr/lib -Iminilibx-linux -lXext -lX11 -lm -lz #-Iinclude
CC = cc #-Wall -Werror -Wextra
SRCS = prova.c
INCLUDES = -Iincludes

all: $(NAME)

$(NAME):
	make -C minilibx-linux 2>/dev/null 1>/dev/null
	make -C libft
	$(CC) $(SRCS) $(INCLUDES) $(MLX)  libft/libft.a   -o $(NAME)

clean:
	make clean -C minilibx-linux 2>/dev/null 1>/dev/null
	make clean -C libft

fclean: clean
	make fclean -C libft
	rm -f $(NAME)

re: fclean all
.PHONY: all clean fclean re
.SILENT:
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
NO_COLOR = \033[0m
BOLD = \033[1m
