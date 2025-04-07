CC = gcc
CFLAGS = -Wall -g
OBJS = main.o parser.o executor.o signals.o jobs.o history.o

mysh: $(OBJS)
	$(CC) -o mysh $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o mysh