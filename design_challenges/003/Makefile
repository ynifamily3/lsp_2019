OBJECTS	= main.o backup.o command_checker.o
TARGET	= ./ssu_backup
CC		= gcc
CFLAGS	= -W -Wall -lpthread

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm $(OBJECTS) $(TARGET)
