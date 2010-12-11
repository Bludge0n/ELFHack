CC=gcc
TARGET=elfcheck

all: main elfhack.c
	@$(CC) -o $(TARGET) elfhack.c
	@./$(TARGET)
	@rm $(TARGET)
