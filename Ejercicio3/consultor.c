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
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

void validarParametros(int arg, char *args[]) {
    if (arg != 3) {
      printf("CANTIDAD DE PARAMETROS INCORRECTOS,VERIFIQUE LA AYUDA ( ./consultar -h )\n");
      exit(1);
    }

    struct stat myFile;

    if (stat(args[2], &myFile) < 0) {
      printf("\n No se encontro el fifo: %s \n", args[2]);
      printf("\n Debe ejecutar el proceso ejercicio3 primero\n");
      exit(1);
    }

    return;
}

void mostrarAyuda(){
  printf("Ejemplo de ejecucion:\n");
  printf("Primero crear el demonio\n");
  printf("\t ./ejercicio3 ./articulos.txt ./fifoConsulta ./fifoResultado");
  printf("\n Para realizar consultas segun ID - PRODUCTO - MARCA \n");
  printf("\t ./consultar producto=P.DULCE ./fifoConsulta\n");
  printf("Para visualizar la salida usar: cat ./fifoResultado\n");
  exit(0);
}

int main(int arg, char *args[]) {

    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-help") == 0 || strcmp(args[1], "-?") == 0))
        mostrarAyuda();

    validarParametros(arg, args);

    char *entrada = args[1];
    int cantCaracteres = strlen(entrada);
    int fd = open(args[2], O_WRONLY | O_TRUNC | O_CREAT);
    write(fd, args[1], cantCaracteres + 1);

    return 0;
}
