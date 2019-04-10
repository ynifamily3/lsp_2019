OBJECTS = ssu_score.o print_helps.o arg_read.o ssu_runtime.o parse_tree.o
TARGET = ./ssu_score
CC = gcc
CFLAGS = -Wall -W -std=c99 -lpthread -pthread

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

ssu_score.o : ssu_score.c
	$(CC) $(CFLAGS) -c ssu_score.c

print_helps.o : print_helps.c
	$(CC) $(CFLAGS) -c print_helps.c

arg_read.o : arg_read.c
	$(CC) $(CFLAGS) -c arg_read.c

ssu_runtime.o : ssu_runtime.c
	$(CC) $(CFLAGS) -c ssu_runtime.c

parse_tree.o : parse_tree.c
	$(CC) $(CFLAGS) -c parse_tree.c

clean : 
	rm $(TARGET)
run :
	./ssu_score ${args}
