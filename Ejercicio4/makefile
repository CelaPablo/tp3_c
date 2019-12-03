all: ejercicio4 consultar

ejercicio4:  ejercicio4.o
	gcc -o ejercicio4 ejercicio4.o -lpthread -lrt
ejercicio4.o:  ejercicio4.c
	gcc -c ejercicio4.c

consultar: consultor.o
	gcc -o consultar consultor.o -lpthread -lrt
consultor.o: consultor.c
	gcc -c consultor.c

clean:
	rm *.o
