#"make" para ejecutar, "make valgrind" para ejecutar y reportar posibles fugas de memoria en valgrind-report.txt

all: clean executable execute 

executable:lista.o listaficheros.o p0.o p1.o p2.o p3.o generalfunctions.o listamemoria.o listaprocesos.o
	gcc -Wall -o p3 lista.o listaficheros.o p1.o generalfunctions.o p0.o p2.o p3.o listamemoria.o listaprocesos.o

p0.o:
	gcc -c -g p0.c 
lista.o:
	gcc -c -g lista.c
listaficheros.o:
	gcc -c -g listaficheros.c
listamemoria.o:
	gcc -c -g listamemoria.c
listaprocesos.o:
	gcc -c -g listaprocesos.c
p1.o:
	gcc -c -g p1.c
p2.o:
	gcc -c -g p2.c
p3.o:
	gcc -c -g p3.c
generalfunctions.o:
	gcc -c -g generalfunctions.c

execute: 
	./p3

valgrind: clean executable
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-report.txt ./p3

clean:
	rm -f *.o p3
