OBJECTS	= main.o lex.o pattern.o arg_read.o converter.o header_table.o ssu_runtime.o
TARGET	= ./ssu_convert
CC		= gcc
CFLAGS	= -std=c99 -W -Wall

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm $(OBJECTS)
