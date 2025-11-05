CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

SRC = main.c image_processing.c image_utils.c contour_detection.c grid_detection.c
OBJ = $(SRC:.c=.o)
EXEC = ocr_detect

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f *.o
	@rm -f $(EXEC)
	@rm -f $(EXEC).exe
	@rm -f *.bmp
	@rm -rf out/

run: $(EXEC)
	./$(EXEC) input.png output

re: clean all

