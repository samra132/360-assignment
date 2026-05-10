# Student Name: Gurleen Samra, Omar Ghzeil
# Student ID: 3144680, 3123197
# File: Makefile

CC = gcc-13
CFLAGS = -std=c11 -Wall -Wextra -pedantic

TARGET = userclean

all: $(TARGET)

$(TARGET): userclean.c
	$(CC) $(CFLAGS) userclean.c -o $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(TARGET)
