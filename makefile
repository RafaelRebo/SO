#"make" para ejecutar, "make valgrind" para ejecutar y reportar posibles fugas de memoria en valgrind-report.txt

all: clean executable execute 

executable:lista.o listaficheros.o p0.o p1.o p2.o generalfunctions.o listamemoria.o
	gcc -Wall -o p2 lista.o listaficheros.o p1.o generalfunctions.o p0.o p2.o listamemoria.o

p0.o:
	gcc -c -g p0.c 
lista.o:
	gcc -c -g lista.c
listaficheros.o:
	gcc -c -g listaficheros.c
listamemoria.o:
	gcc -c -g listamemoria.c
p1.o:
	gcc -c -g p1.c
p2.o:
	gcc -c -g p2.c
generalfunctions.o:
	gcc -c -g generalfunctions.c

execute: 
	./p2

valgrind: clean executable
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-report.txt ./p2

clean:
	rm -f *.o p2
