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

int validarParametros(int arg, char *args[])
{
    if (arg != 2) {
        return 1;
    }

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\nno se encontro el archivo: %s\n", args[1]);
        return 1;
    }

    return 0;
}

void mostrarAyuda() {
    printf("\n Ejemplo de ejecucion:\n");
    printf("\t ./ejercicio4 ./articulos.txt  \n");
    printf("\n Ejemplo de ejecucion consumidor:\n");
    printf("\t ./consumir ./articulos.txt \n");
    printf("\n Ejemplo de ejecucion consultor:\n");
    printf("\t ./consultar producto=P.DULCE \n");
    return;
}

int obtenerCantidadDeRegistros(char *path[])
{
    FILE *pf;
    int cantfilas = 0;
    pf = fopen(*path, "r");
    if (!pf)
    {
        printf("no se encuentra el archivo");
        exit(0);
    }
    char fila[100];
    while (!feof(pf))
    {
        fscanf(pf, " %[^\n]", fila);
        cantfilas++;
    }
    fclose(pf);
    return cantfilas;
}

int main(int arg, char *args[])
{
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0))
    {

        mostrarAyuda();

        return 0;
    }
     if (validarParametros(arg, args) == 1)
       return 1;

    int registros = obtenerCantidadDeRegistros(&args[1]);

    char entrada[registros * 40];

    int fds;
    char *mems;   
    sem_t *sem3 = sem_open("semaforo3", O_CREAT, 0600, 0); // crear semaforo
    sem_t *sem4 = sem_open("semaforo4", O_CREAT, 0600, 0);
    size_t tam2= sizeof(char) * 30 * registros;
    fds = shm_open("/mc2", O_CREAT | O_RDWR, 0600); // crear memoria compartida

    ftruncate(fds, tam2);                                                       // definir tamaño
    mems= (char *)mmap(NULL, tam2, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0); // asociar espacio a variable

    sem_wait(sem3); // P()

    strcpy(entrada,mems);
    //printf("%s\n", mem);
    sem_post(sem4); // V()

    close(fds);

    munmap(mems, tam2);
    
    sem_close(sem3);
    
    sem_close(sem4);
    printf("\n*****SALIDA****\n");

    printf("\n %s\n ", entrada);

    return 0;
}
