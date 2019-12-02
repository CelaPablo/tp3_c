/***********************************
 Trabajo práctico N3 Ejercicio 4
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
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "constantes.h"

void validarParametros(int arg) {
  if (arg != 2) {
      printf("\nCantidad de parametros incorrecta, verifique la ayuda (./ejercicio4 -h)\n");
      exit(1);
  }

  return;
}

void mostrarAyuda(){
  printf("Ejemplo de ejecucion:\n");
  printf("Primero crear el demonio\n");
  printf("\t./ejercicio4 ./articulos.txt\n");
  printf("Para realizar consultas segun ID - PRODUCTO - MARCA \n");
  printf("\t./consultar producto=P.DULCE\n");
  exit(1);
}

void recibirResultado(message *resultado) {
    int fds;
    message *mem;
    // crear semaforos
    sem_t *sem3 = sem_open(SEM_C, O_CREAT, 0600, 0);
    sem_t *sem4 = sem_open(SEM_D, O_CREAT, 0600, 0);
    // crear memoria compartida para la respuesta
    fds = shm_open(MEM_R, O_CREAT | O_RDWR, 0600);
    size_t tam = sizeof(message);
    // definir tamaño
    ftruncate(fds, tam);
    // asociar espacio a variable
    // direccion a mapear - tamanio - modo - MAP_SHARED hace visible la memoria para otros - offset
    mem = mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    // P(sem3)
    sem_wait(sem3);

    resultado->siguiente = mem->siguiente;
    strcpy(resultado->valor, mem->valor);
    // V(sem4)
    sem_post(sem4);
    // cierra memoria compartida
    close(fds);
    // eliminar las asignaciones a la memoria compartida
    munmap(mem, tam);
    //cierro semaforos 3 y 4
    sem_close(sem3);
    sem_close(sem4);
}

void enviarConsulta(char *consulta, int tam) {
    int fd;
    char *mem;
    // crear semaforo 1 y 2
    sem_t *sem1 = sem_open(SEM_A, O_CREAT, 0600, 0);
    sem_t *sem2 = sem_open(SEM_B, O_CREAT, 0600, 0);
    // crear memoria compartida para la consulta
    fd = shm_open(MEM_C, O_CREAT | O_RDWR, 0600);
    // definir tamaño
    ftruncate(fd, tam);
    // asociar espacio a variable
    mem = (char *) mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // escribo la consulta en la memoria compartida
    strcpy(mem, consulta);
    // una vez que envio el mensaje y recibo la confirmacion cierro todo
    // libero el semaforo para que el proceso "ejercicio4" pueda leer de la memoria
    // me quedo bloqueado hasta que se lea de la memoria
    sem_post(sem1);  // V(sem1);
    sem_wait(sem2); //P(sem2)
    close(fd);
    // elino la asociacion de la variable con la memoria
    munmap(mem, tam);
    // elimino la memoria compartida
    shm_unlink(MEM_C);
    // cierro y elimino los semaforos
    sem_close(sem1);
    sem_unlink(SEM_A);
    sem_close(sem2);
    sem_unlink(SEM_B);
}

int main(int arg, char *args[]) {

    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-help") == 0 || strcmp(args[1], "-?") == 0))
        mostrarAyuda();

    validarParametros(arg);

    char *entrada = args[1];
    int cantCaracteres = strlen(entrada);

    enviarConsulta(args[1], cantCaracteres + 1);

    message men;
    men.siguiente = 1;
    while (men.siguiente) {
        recibirResultado(&men);
        if (strlen(men.valor) != 0)
            printf("%s", men.valor);
    }
    // elimino los recursos creados
    sem_unlink(SEM_A);
    sem_unlink(SEM_B);
    sem_unlink(SEM_C);
    sem_unlink(SEM_D);

    return 0;
}
