bmptool:	bmplib.o asgn2.o
	$(CC) bmplib.o asgn2.o -o $@

bmplib.o:	bmplib.c bmplib.h
	$(CC) -Wall -c $< -o $@

asgn2.o:	asgn2.c bmplib.h 
	$(CC) -Wall -c $< -o $@

clean:
	rm -f bmptool *.o core *~
