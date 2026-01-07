CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/utils.c src/files.c src/users.c src/books.c src/loans.c src/interface.c
OBJ = $(SRC:.c=.o)
EXEC = wookinted

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)
	rm -f data/*.dat

run: all
	./$(EXEC)
