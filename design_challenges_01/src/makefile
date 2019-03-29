OBJECTS = ssu_score.o print_helps.o arg_read.o assert_answer.o mark_student.o ssu_runtime.o
TARGET = ../ssu_score
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

assert_answer.o : assert_answer.c
	$(CC) $(CFLAGS) -c assert_answer.c

mark_student.o : mark_student.c
	$(CC) $(CFLAGS) -c mark_student.c

ssu_runtime.o : ssu_runtime.c
	$(CC) $(CFLAGS) -c ssu_runtime.c

clean : 
	rm $(TARGET)
run :
	../ssu_score ${args}
rund : 
	../ssu_score ../STD_DIR ../ANS_DIR
