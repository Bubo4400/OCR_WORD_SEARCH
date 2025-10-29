CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image
OBJ = main.o image_processing.o image_utils.o
BIN = ocr_detect

all: $(BIN)

$(BIN): $(OBJ)
	@echo "🧩 Édition des liens..."
	$(CC) $(OBJ) -o $(BIN) $(LDFLAGS)
	@echo "✅ Compilation terminée : $(BIN)"

%.o: %.c
	@echo "🧩 Compilation de $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	./$(BIN) input.png output.bmp

