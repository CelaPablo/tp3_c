all: ejercicio5 consultar 

ejercicio5:  ejercicio5.o
	gcc -o ejercicio5 ejercicio5.o 
ejercicio5.o:  ejercicio5.c
	gcc -c ejercicio5.c 

consultar: consultor.o
	gcc -o consultar consultor.o 
consultor.o: consultor.c 
	gcc -c consultor.c 

clean:
	rm *.o