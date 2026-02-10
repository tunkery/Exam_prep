NAME = vbc
TESTER = test_vbc
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

# Choose your implementation file (vbc0.c or given.c)
VBC_SRC = vbc0.c
# VBC_SRC = given.c  # Uncomment this to test given.c instead

all: $(NAME) $(TESTER)

$(NAME): $(VBC_SRC)
	$(CC) $(CFLAGS) $(VBC_SRC) -o $(NAME)

$(TESTER): test_main.c $(NAME)
	$(CC) $(CFLAGS) test_main.c -o $(TESTER)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME) $(TESTER)

re: fclean all

run: $(TESTER)
	./$(TESTER)

test: run

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TESTER)

.PHONY: all clean fclean re run test valgrind
