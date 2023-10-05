all: clean executable execute 

executable:lista.o p0.o listaficheros.o
	gcc -Wall -o p0 lista.o  p0.o listaficheros.o

p0.o:
	gcc -c -g p0.c 
lista.o:
	gcc -c -g lista.c
listaficheros.o:
	gcc -c -g listaficheros.c

execute: 
	./p0

valgrind: clean executable
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-report.txt ./p0

clean:
	rm -f *.o p0
