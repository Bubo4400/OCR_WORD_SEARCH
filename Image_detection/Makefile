CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image
SRC = src/main.c src/image_utils.c
OBJ = $(SRC:.c=.o)
BIN = wordsearch

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

