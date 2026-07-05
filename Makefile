CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

BUILD_DIR = build

TARGET = main

SRC = \
	src/main.c \
	src/game.c \
	src/render.c

OBJ = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/game.o \
	$(BUILD_DIR)/render.o


TEST_SRC = \
	test/functions_test.c \
	src/game.c \
	src/render.c

TEST_TARGET = tests

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: src/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/game.o: src/game.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/render.o: src/render.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

play: $(TARGET)
	./$(TARGET) 



clean:
	rm -rf $(BUILD_DIR) main

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re play test