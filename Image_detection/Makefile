CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lm

SRC = main.c image_processing.c image_utils.c contour_detection.c grid_detection.c
OBJ = $(SRC:.c=.o)
BIN = ocr

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

clean:
	rm -f $(OBJ) $(BIN)

