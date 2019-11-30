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

int validarParametros(int arg, char *args[])
{

    if (arg != 2) {
        printf("\nCANTIDAD DE PARAMETROS INCORRECTOS,VERIFIQUE LA AYUDA\n");
        return 1;
    }

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\nNo se encontro el archivo %s\n", args[1]);
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

void agregarSalida(char out[], char id[], char articulo[], char producto[], char marca[])
{
    strcat(out, id);
    strcat(out, ";");
    strcat(out, articulo);
    strcat(out, ";");
    strcat(out, producto);
    strcat(out, ";");
    strcat(out, marca);
    strcat(out, "\n");
    return;
}

int obtenerCantidadDeRegistros(char *path[])
{
    FILE *pf;
    int cantfilas = 0;
    pf = fopen(*path, "r");
    if (!pf)
    {
        printf("No se encuentra el archivo");
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

void filtrarArchivo(char *path[], char *filtro, int registros, char *salida)
{

    FILE *pf;
    int esId = strncmp("ID", filtro, 2);
    int esProducto = strncmp("PRODUCTO", filtro, 8);
    int esMarca = strncmp("MARCA", filtro, 5);
    // printf("esMarca: %d",esMarca);
    int j = 0;
    printf("valor j=%d\n", j);
    char id[100];
    char articulo[100];

    char producto[100];
    char marca[100];
    char *igual = strchr(filtro, '=');
    char *buscado = igual;

    strcpy(salida, " ");
    buscado++;
    printf("buscado: %s\n", buscado);

    printf("\nFILTRO: %s\n", filtro);
    pf = fopen(*path, "r");
    if (!pf)
    {

        printf("\nNo se  encontro el archivo %s\n", *path);
        exit(0);
    }
    strcpy(id, " ");
    strcpy(articulo, " ");
    strcpy(producto, " ");
    strcpy(marca, " ");

    while (!feof(pf))
    {

        j++;
        fflush(stdin);
        fscanf(pf, " %[^;]", id);

        fflush(stdin);
        fscanf(pf, " ;%[^;]", articulo);

        fflush(stdin);
        fscanf(pf, " ;%[^;]", producto);

        fflush(stdin);
        fscanf(pf, " ;%[^\r|\n]", marca);
        printf("id: %s articulo: %s producto:%s marca: %s\n", id, articulo, producto, marca);

        if (esId == 0 && strcmp(id, buscado) == 0)
        {

            agregarSalida(salida, id, articulo, producto, marca);
        }
        else if (esMarca == 0 && strcmp(marca, buscado) == 0)
        {

            agregarSalida(salida, id, articulo, producto, marca);
        }
        else if (esProducto == 0 && strcmp(producto, buscado) == 0)
        {

            agregarSalida(salida, id, articulo, producto, marca);
        }

        strcpy(id, " ");
        strcpy(articulo, " ");
        strcpy(producto, " ");
        strcpy(marca, " ");
    }
    fclose(pf);

    return;
}

int main(int arg, char *args[])
{
    int x;
    int num = arg;

    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0))
    {

        mostrarAyuda();

        return 0;
    }
    int res = validarParametros(arg, args);
    if (res == 1)
    {
        return 0;
    }

    //si es el hijo se queda ejecutando
    while (1)
    {

        char filtro[100];
        int registros = obtenerCantidadDeRegistros(&args[1]);

       // **** MEMORIA COMPARTIDA PARA RECIBIR CONSULTA********
        int fd;
        char *mem;
        sem_t *sem = sem_open("semaforo", O_CREAT, 0600, 0); // crear semaforo
        sem_t *sem2 = sem_open("semaforo2", O_CREAT, 0600, 0);
        size_t tam = sizeof(char) * 100;
        fd = shm_open("/nombre1", O_CREAT | O_RDWR, 0600);                        // crear memoria compartida
        mem = (char *)mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // asociar espacio a variable
        sem_wait(sem); // P() // me quedo bloquedo hasta poder leer de la memoria
        printf("%s\n", mem);
        strcpy(filtro, mem);
        sem_post(sem2); // V()
        close(fd);
        munmap(mem, tam);
        sem_close(sem);
        sem_close(sem2);

        char salida[registros * 40];       
        int cantCaracteres = strlen(filtro);
        char *aMayuscula = filtro;

        // pongo en mayuscula el filtro
        for (int i = 0; i < cantCaracteres; i++)
        {
            *aMayuscula = toupper(*aMayuscula);
            aMayuscula++;
        }

        filtrarArchivo(&args[1], filtro, registros, salida);

        printf("\n******ARCHIVO FILTRADO*******\n");
        ///**** MEMORIA PARA ENVIAR EL RESULTADO DE LA CONSULTA******
        int fds;
        char *mems;
        sem_t *sem3 = sem_open("semaforo3", O_CREAT, 0600, 0); // crear semaforo
        sem_t *sem4 = sem_open("semaforo4", O_CREAT, 0600, 0); // crea segundo semaforo
        size_t tam2 = sizeof(char) * 30 * registros;
        fds = shm_open("/mc2", O_CREAT | O_RDWR, 0600);                              // crear memoria compartida
        ftruncate(fds, tam2);                                                        // definir tamaño
        mems = (char *)mmap(NULL, tam2, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0); // asociar espacio a variable
        strcpy(mems, salida);                                                        // copio lo que tenga salida a la memoria compartida;
        sem_post(sem3);                                                              // V();
        sem_wait(sem4);                                                              //P()
        close(fds);
        munmap(mems, tam2);
        shm_unlink("/mc2");
        sem_close(sem3);
        sem_unlink("semaforo3");
        sem_close(sem4);
        sem_unlink("semaforo4");
    }

    return (0);
}
