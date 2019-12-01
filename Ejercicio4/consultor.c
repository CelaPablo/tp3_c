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
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>

int validarParametros(int arg, char *args[]) {
    if (arg != 2)
        return 1;

    return 0;
}

void mostrarAyuda() {
    printf("\n Ejemplo de ejecucion:\n");
    printf("\t ./ejercicio4 ./articulos.txt  \n");
    printf("\n Ejemplo de ejecucion consumidor:\n");
    printf("\t ./consumir ./articulos.txt \n");
    printf("\n Ejemplo de ejecucion consultor:\n");
    printf("\t ./consultar producto=P.DULCE \n");
    exit(0);
}

int main(int arg, char *args[]) {

    char *entrada = args[1];
    int tam = sizeof(char) * 100;

    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-help") == 0 || strcmp(args[1], "-?") == 0))
        mostrarAyuda();

    printf("antes de los semaforos\n");

    sem_t *sem1 = sem_open("semaforo1", O_CREAT); // para que se bloquee hasta que llegue una consulta
    sem_t *sem2 = sem_open("semaforo2", O_CREAT); // para bloquear cuando arranca consultor
    sem_t *sem3 = sem_open("semaforo3", O_CREAT); // porq si

    sem_wait(sem2);
    printf("espera\n");

    int fd;
    char *memC;                 // crea segundo semaforo
    fd = shm_open("/consultas", O_CREAT | O_RDWR, 0600);                        // crear memoria compartida
    ftruncate(fd, tam);                                                       // definir tamaño
    memC = (char *)mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // asociar espacio a variable
    strcpy(memC, entrada); // escribo la consulta en la memoria compartida

    sem_post(sem1);
    exit(0);
    // una vez que envio el mensaje y recibo la confirmacion cierro todo
    // sem_post(sem);  // V(); // libero el semaforo para que el proceso "ejercicio3" pueda leer de la memoria
    // sem_wait(sem2); //P() // me quedo bloqueado hasta que se lea de la memoria
    // close(fd);
    // munmap(mem, tam); // elino la asociacion de la variable con la memoria
    // shm_unlink("/nombre1");// elimino la memoria compartida
    // sem_close(sem);
    // sem_unlink("semaforo"); // elimino el semaforo
    // sem_close(sem2);
    // sem_unlink("semaforo2");
    //
    // return 0;
}
