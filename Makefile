##
## EPITECH PROJECT, 2023
## Paradigm-day04am
## File description:
## ex00
##

CC	:=	gcc -std=gnu17

CFLAGS	=	-W
CFLAGS	+=	-Wall
CFLAGS	+=	-Wextra
CFLAGS	+=	-Wundef
CFLAGS	+=	-Wshadow
CFLAGS	+=	-Wunreachable-code
CFLAGS	+=	-Wmissing-prototypes
CFLAGS	+=	-pedantic
CFLAGS	+=	-I./include/ -g3 -O1

BDIR	=	.build

NAME	=	myftp

SRC	=	$(shell find src -name "*.c")

OBJ	=	$(SRC:%.c=$(BDIR)/%.o)

.PHONY: all
all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(NAME)

$(BDIR)/%.o:	%.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: asan
asan:	CFLAGS += -fsanitize=address,leak,undefined -g3
asan: re

.PHONY: clean
clean:
	@ rm -f $(OBJ)
	@ rm -rf .build

.PHONY: fclean
fclean:	clean
	@ rm -f $(NAME)

.PHONY: re
.NOTPARALLEL: re
re:	fclean all
