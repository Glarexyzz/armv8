CC = gcc
CFLAGS = -Wall
LDFLAGS = -lncurses
OBJFILES = food.o main.o game_manager.o game.o render.o snake.o utils.o
TARGET = snake_game
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~