CC = gcc
CFLAGS = -Wall -g
OBJS = main.o parser.o executor.o signals.o jobs.o utils.o

myshell: $(OBJS)
	$(CC) -o myshell $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o myshell