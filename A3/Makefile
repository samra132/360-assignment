# Student Name: Omar Ghzeil, Gurleen Samra
# Student ID:3123197, 3144680
# Submission Date: June 7, 2026
# File: Makefile
# Description: Builds, tests, and checks vmsim

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g
TARGET = vmsim

all: $(TARGET)

$(TARGET): vmsim.c vmsim.h
	$(CC) $(CFLAGS) vmsim.c -o $(TARGET)

test: $(TARGET)
	./$(TARGET) --mode=bb --base=4096 --limit=64 --trace=tests/bb/t1.txt
	./$(TARGET) --mode=bb --base=4096 --limit=64 --trace=tests/bb/bad.txt
	./$(TARGET) --mode=seg --config=tests/seg/three-seg.ini --trace=tests/seg/t2.txt
	./$(TARGET) --mode=seg --config=tests/seg/stack-demo.ini --trace=tests/seg/stack-down.txt
	./$(TARGET) --mode=seg --config=tests/seg/three-seg.ini --trace=tests/seg/bad-trace.txt
	./$(TARGET) --mode=seg --config=tests/seg/bad-config.ini --trace=tests/seg/t2.txt

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --mode=bb --base=4096 --limit=64 --trace=tests/bb/t1.txt
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --mode=seg --config=tests/seg/three-seg.ini --trace=tests/seg/t2.txt

clean:
	rm -f $(TARGET) *.o
