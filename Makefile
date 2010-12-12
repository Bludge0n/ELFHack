CC=gcc
TARGET=elfcheck

all: test.o elfhack.c
	@$(CC) -o $(TARGET) elfhack.c
	@./$(TARGET) a.out
	@rm $(TARGET)

test.o: test.c
	@$(CC) -c test.c

clean: test.o
	@rm test.o
