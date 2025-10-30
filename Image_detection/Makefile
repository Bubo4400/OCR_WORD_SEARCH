CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

OBJS = main.o image_processing.o image_utils.o contour_detection.o grid_detection.o
BIN = ocr_detect

all: $(BIN)

$(BIN): $(OBJS)
	@echo "🧩 Édition des liens..."
	$(CC) $(OBJS) -o $(BIN) $(LDFLAGS) -lm
	@echo "✅ Compilation terminée : $(BIN)"

%.o: %.c
	@echo "🧩 Compilation de $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

run: $(BIN)
	./$(BIN) input.png output

