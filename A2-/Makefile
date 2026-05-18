CC = gcc-13
CFLAGS = -std=c11 -Wall -Wextra -pedantic

TARGET = sched

all: $(TARGET)

$(TARGET): sched.c
	$(CC) $(CFLAGS) -o $(TARGET) sched.c

clean:
	rm -f $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --policy=FCFS --in=workload.txt