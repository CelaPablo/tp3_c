all: ejercicio3 consultar #consumir

ejercicio3:  ejercicio3.o
	gcc -o ejercicio3 ejercicio3.o
ejercicio3.o:  ejercicio3.c
	gcc -c ejercicio3.c

consultar: consultor.o
	gcc -o consultar consultor.o
consultor.o: consultor.c 
	gcc -c consultor.c

#consumir: consumir.o
#	gcc -o consumir consumir.o
#consumir.o: consumir.c 
#	gcc -c consumir.c

clean:
	rm *.o