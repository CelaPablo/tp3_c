all: ejercicio4 consultar consumir

ejercicio4:  ejercicio4.o
	gcc -o ejercicio4 ejercicio4.o -lpthread
ejercicio4.o:  ejercicio4.c
	gcc -c ejercicio4.c 

consultar: consultor.o
	gcc -o consultar consultor.o -lpthread
consultor.o: consultor.c 
	gcc -c consultor.c 

consumir: consumidor.o
	gcc -o consumir consumidor.o -lpthread
consumidor.o: consumidor.c 
	gcc -c consumidor.c 

clean:
	rm *.o