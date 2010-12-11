CC=gcc
TARGET=elfcheck

all: test.o elfhack.c
	@$(CC) -o $(TARGET) elfhack.c
	@./$(TARGET)
	@rm $(TARGET)
