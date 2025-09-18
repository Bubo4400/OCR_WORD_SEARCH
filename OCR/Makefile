CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter
SRC = main.c array_pointer.c
OBJ = $(SRC:.c=.o)
EXEC = solver

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(EXEC)

re: clean all
