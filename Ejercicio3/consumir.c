/***********************************
 Trabajo práctico N3 Ejercicio 3
 Integrantes:
 Cabral, David           39757782
 Cela, Pablo             36166857
 Pessolani, Agustin      39670584
 Sullca, Fernando        37841788
 Zabala, Gaston          34614948
 ***********************************/
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>

int validarParametros(int arg, char *args[]) {
    if (arg != 3)
        return 1;

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\n No se encontro el archivo: %s\n", args[1]);
        return 1;
    }

    if (stat(args[2], &myFile) < 0) {
        printf("\n No se encontro el fifo: %s \n", args[2]);
        printf("\n Debe ejecutar el proceso ejercicio3 primero\n");
        return 1;
    }
    return 0;
}

void mostrarAyuda(){
    printf("\n Ejemplo de ejecucion:\n");
    printf("\n Primero crear el demonio\n");
    printf("\t ./ejercicio3  ./articulos.txt ./fifoConsulta ./fifoResultado");
    printf("\n Para realizar consultas segun ID - PRODUCTO - MARCA \n");
    printf("\t ./consultar producto=P.DULCE ./fifoConsulta");
    printf("\n Para consumir: \n");
    printf("\t ./consumir ./articulos.txt ./fifoResultado \n");
    return ;
}

int obtenerCantidadDeRegistros(char *path[]) {
    FILE *pf;
    int cantfilas = 0;
    pf = fopen(*path, "r");
    if (!pf) {
        printf("\n No se encuentra el archivo.");
        exit(0);
    }
    char fila[100];
    while (!feof(pf)) {
        fscanf(pf, " %[^\n]", fila);
        cantfilas++;
    }
    fclose(pf);
    return cantfilas;
}

int main(int arg, char *args[]) {
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0)) {
        mostrarAyuda();
        return 0;
    }

    if (validarParametros(arg, args) == 1)
        return 1;

    int fd = open(args[2], O_RDONLY); // abrir fifo para lectura
    int registros = obtenerCantidadDeRegistros(&args[1]);
    char entrada[registros * 40];
    
    int bytes = -1;
    bytes = read(fd, entrada, sizeof(entrada) + 1); // leer fifo
    printf("\n %s\n ", entrada);
    
    return 0;
}
