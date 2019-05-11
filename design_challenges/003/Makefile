OBJECTS	= main.o backup.o command_checker.o
TARGET	= ./ssu_convert
CC		= gcc
CFLAGS	= -std=c99 -W -Wall

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm $(OBJECTS)
