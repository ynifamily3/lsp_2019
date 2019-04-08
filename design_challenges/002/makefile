OBJECTS	= main.o lex.o
TARGET	= ./javatoC
CC		= gcc
CFLAGS	= -std=c99 -W -Wall

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main.o : main.c
	$(CC) $(CFLAGS) -c main.c

lex.o : lex.c
	$(CC) $(CFLAGS) -c lex.c
