all: ejercicio4 consultor

ejercicio4:  ejercicio4.o
	gcc -o ejercicio4 ejercicio4.o -lpthread
ejercicio4.o:  ejercicio4.c
	gcc -c ejercicio4.c

consultar: consultor.o
	gcc -o consultor consultor.o -lpthread
consultor.o: consultor.c
	gcc -c consultor.c

clean:
	rm *.o
