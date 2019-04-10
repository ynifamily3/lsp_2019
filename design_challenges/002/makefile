OBJECTS	= main.o lex.o pattern.o arg_read.o
TARGET	= ./javatoC
CC		= gcc
CFLAGS	= -std=c99 -W -Wall -DDEBUG

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm $(OBJECTS)