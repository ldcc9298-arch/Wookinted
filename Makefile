CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
# Adicionamos -lm caso uses funções de math.h no futuro
LDFLAGS = -lm

SRC = src/main.c src/utils.c src/files.c src/users.c src/books.c src/transactions.c src/interface.c
OBJ = $(SRC:.c=.o)
EXEC = wookinted

.PHONY: all clean clean-data run

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpa apenas os binários (Recomendado para o dia-a-dia)
clean:
	rm -f src/*.o $(EXEC)

# Comando extra para quando quiseres resetar a base de dados
clean-data:
	rm -f data/*.dat data/log_sistema.txt

run: all
	./$(EXEC)