all: bpgame.o bpop 

bpgame.o: bpgame.c
	gcc bpgame.c -c -o bpgame.o 

bpop: bpop.c simpleio.c bpgame.o 
	gcc bpop.c simpleio.c bpgame.o -o bpop 

clean:
	rm -f *.o bpop