OBJECTS = ssu_main.o ssu_os.o ssu_esp.o ssu_lsp.o
TARGET = ssu_prog
CC = gcc
CFLAGS = -Wall -W

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

ssu_main.o : ssu_main.c
	$(CC) $(CFLAGS) -c ssu_main.c

ssu_os.o : ssu_os.c
	$(CC) $(CFLAGS) -c ssu_os.c

ssu_esp.o : ssu_esp.c
	$(CC) $(CFLAGS) -c ssu_esp.c

ssu_lsp.o : ssu_lsp.c
	$(CC) $(CFLAGS) -c ssu_lsp.c

