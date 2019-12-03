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
//La agregué para poder capturar la señal
#include<signal.h>

// funcion para eliminar los recursos cuando se hace kill del demonio
void sig_handler(int signal) {
  puts("unlink");
  shm_unlink(MEM_R);
  shm_unlink(MEM_C);
  sem_unlink(SEM_A);
  sem_unlink(SEM_B);
  sem_unlink(SEM_C);
  sem_unlink(SEM_D);
  exit(1);
}

void enviarArchivoFiltrado(char *salida, int siguiente) {
    int fds;
    message *mem;
    // crear semaforos
    sem_t *sem3 = sem_open(SEM_C, O_CREAT, 0600, 0);
    sem_t *sem4 = sem_open(SEM_D, O_CREAT, 0600, 0);
    size_t tam = sizeof(message);
    // crear memoria compartida para el resultado
    fds = shm_open(MEM_R, O_CREAT | O_RDWR, 0600);
    // definir tamaño
    ftruncate(fds, tam);
    // asociar espacio a variable
    mem = mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    mem->siguiente = siguiente;
    strcpy(mem->valor, salida);

    sem_post(sem3); // V(sem3);
    sem_wait(sem4); //P(sem4)

    close(fds);
    munmap(mem, tam);
    sem_close(sem3);
    sem_close(sem4);
}

void validarParametros(int arg, char *args[]) {

    if (arg != 2) {
        printf("\nCANTIDAD DE PARAMETROS INCORRECTOS,VERIFIQUE LA AYUDA\n");
        exit(1);
    }

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\nNo se encontro el archivo\n");
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

void agregarSalida(char out[], char id[], char articulo[], char producto[], char marca[]) {
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

void agregarSalidaNula(char out[]) {
    strcat(out, "No se encontraron resultados.\n");
}

void agregarSalidaIncorrecta(char out[]) {
    strcat(out, "VERIFIQUE LA AYUDA - ./ejercicio3 -h\n");
}

int obtenerCantidadDeRegistros(char *path[]) {
    FILE *pf;
    int cantfilas = 0;
    pf = fopen(*path, "r");
    if (!pf) {
        printf("No se encuentra el archivo\n");
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

void filtrarArchivo(char *path[], char *filtro, char *salida) {

    FILE *pf;
    int esId = strncmp("ID", filtro, 2);
    int esProducto = strncmp("PRODUCTO", filtro, 8);
    int esMarca = strncmp("MARCA", filtro, 5);
    int agregados = 0;
    char id[60];
    char articulo[60];
    char producto[60];
    char marca[60];
    char *igual = strchr(filtro, '=');
    char *buscado = igual;

    strcpy(salida, " ");
    buscado++;

    if(!igual) {
      agregarSalidaIncorrecta(salida);
      enviarArchivoFiltrado(salida, 1);
    } else {

      pf = fopen(*path, "r");
      if (!pf) {
          printf("\nNo se  encontro el archivo\n");
          exit(0);
      }
      strcpy(id, " ");
      strcpy(articulo, " ");
      strcpy(producto, " ");
      strcpy(marca, " ");

      while (!feof(pf)) {

          fflush(stdin);
          fscanf(pf, " %[^;]", id);
          fflush(stdin);
          fscanf(pf, " ;%[^;]", articulo);
          fflush(stdin);
          fscanf(pf, " ;%[^;]", producto);
          fflush(stdin);
          fscanf(pf, " ;%[^\r|\n]", marca);

          if (esId == 0 && strcmp(id, buscado) == 0) {
              agregarSalida(salida, id, articulo, producto, marca);
              enviarArchivoFiltrado(salida, 1);
              agregados++;
          } else if (esMarca == 0 && strcmp(marca, buscado) == 0) {
              agregarSalida(salida, id, articulo, producto, marca);
              enviarArchivoFiltrado(salida, 1);
              agregados++;
          } else if (esProducto == 0 && strcmp(producto, buscado) == 0) {
              agregarSalida(salida, id, articulo, producto, marca);
              enviarArchivoFiltrado(salida, 1);
              agregados++;
          }

          strcpy(id, " ");
          strcpy(articulo, " ");
          strcpy(producto, " ");
          strcpy(marca, " ");
          strcpy(salida, " ");
      }

      if(agregados == 0) {
        agregarSalidaNula(salida);
        enviarArchivoFiltrado(salida, 1);
      }
    }

    enviarArchivoFiltrado("", 0);
    fclose(pf);
}

void recibirConsulta(char *filtro) {
    int fd;
    char *mem;
    // crear semaforo
    sem_t *sem1 = sem_open(SEM_A, O_CREAT, 0600, 0);
    sem_t *sem2 = sem_open(SEM_B, O_CREAT, 0600, 0);
    size_t tam = sizeof(char) * 100;
    // crear memoria compartida
    fd = shm_open(MEM_C, O_CREAT | O_RDWR, 0600);
    ftruncate(fd, tam);
    mem = (char *) mmap(NULL, tam, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // P() // me quedo bloquedo hasta poder leer de la memoria
    sem_wait(sem1);

    strcpy(filtro, mem);
    // V(sem2)
    sem_post(sem2);
    close(fd);
    munmap(mem, tam);
    sem_close(sem1);
    sem_close(sem2);
}

int main(int arg, char *args[]) {
    //Capturo las señales
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

   // por las dudas eliminamos todo antes de arrancar
    shm_unlink(MEM_R);
    sem_unlink(SEM_A);
    sem_unlink(SEM_B);
    sem_unlink(SEM_C);
    sem_unlink(SEM_D);

    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0))
        mostrarAyuda();

    validarParametros(arg, args);
    //creo un proceso hijo
    int pid = fork();
    if(pid > 0) {
      printf("kill %d\n", pid);
      exit(0);
    }
    //si es el hijo se queda ejecutando
    while (1) {
        int fd;
        int fds;

        char filtro[100] = "";
        char *aMayuscula = filtro;

        recibirConsulta(filtro);

        int cantCaracteres = strlen(filtro);

        for (int i = 0; i < cantCaracteres; i++) {
            *aMayuscula = toupper(*aMayuscula);
            aMayuscula++;
        }
        char salida[100];
        filtrarArchivo(&args[1], filtro, salida);

        close(fds);
        close(fd);
    }
    return (0);
}
