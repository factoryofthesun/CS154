CC = gcc 
CFLAGS = -Wall
OBJS = bar.o foo.o 
RMFLAGS = rm 
NAMEFLAGS = -o 

main: bar.o foo.o
	$(CC) $(CFLAGS) $(OBJS) $(NAMEFLAGS) main

bar.o: bar.c bar.h
	$(CC) $(CFLAGS) -c bar.c $(NAMEFLAGS) bar.o

foo.o: foo.c foo.h bar.h
	gcc -Wall -c foo.c -o foo.o

clean:
	rm -f foo.o bar.o main


