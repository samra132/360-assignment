# Student Name: 
# Student ID: 
# Submission Date: June 1, 2026
# File: Makefile
# Description: 

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = vmsim

all: $(TARGET)

$(TARGET): vmsim.c
	$(CC) $(CFLAGS) vmsim.c -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

test: $(TARGET)./$(TARGET) --mode=bb --base=4096 --limit=64 --trace=tests/bb/t1.txt

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) \--mode=bb --base=4096 --limit=64 \--trace=tests/bb/t1.txt
