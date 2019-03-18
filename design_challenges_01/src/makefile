OBJECTS = ssu_score.o print_helps.o arg_read.o assert_answer.o
TARGET = ../ssu_score
CC = gcc
CFLAGS = -Wall -W -std=c99

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

ssu_score.o : ssu_score.c
	$(CC) $(CFLAGS) -c ssu_score.c

print_helps.o : print_helps.c
	$(CC) $(CFLAGS) -c print_helps.c

arg_read.o : arg_read.c
	$(CC) $(CFLAGS) -c arg_read.c

assert_answer.o : assert_answer.c
	$(CC) $(CFLAGS) -c assert_answer.c

clean : 
	rm $(TARGET)
run :
	../ssu_score ${args}
rund : 
	../ssu_score ../STD_DIR ../ANS_DIR
