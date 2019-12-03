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
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>

int validarPathFifo(char *path) {
    char finalPath[strlen(path)];
    char *barra = strrchr(path, '/');

    if (barra != NULL) {
        int cantidad = (strlen(path) - strlen(barra));
        strncpy(finalPath, path, cantidad);
        finalPath[cantidad] = '\0';
    } else
        strcpy(finalPath, path);

    struct stat myFile;
    if (stat(finalPath, &myFile) < 0) {
        printf("\n El directorio %s no existe \n", finalPath);
        return 1;
    }

    return 0;
}

void validarParametros(int arg, char *args[]) {
    if (arg != 4) {
        printf("CANTIDAD DE PARAMETROS INCORRECTOS,VERIFIQUE LA AYUDA ( ./ejercicio3 -h )\n");
        exit(1);
    }

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\nNo se encontro el archivo %s\n", args[1]);
        exit(1);
    }

    if (validarPathFifo(args[2]) == 1 || validarPathFifo(args[3]) == 1)
        exit(1);

    if(strcmp(args[2], args[3]) == 0) {
      printf("\nLOS NOMBRES DE LOS FIFOS DEBEN SER DISTINTOS.\n" );
      exit(1);
    }
    return;
}

void mostrarAyuda(){
  printf("Ejemplo de ejecucion:\n");
  printf("Primero crear el demonio\n");
  printf("\t ./ejercicio3 ./articulos.txt ./fifoConsulta ./fifoResultado");
  printf("\n Para realizar consultas segun ID - PRODUCTO - MARCA - ejemplos:\n");
  printf("\t echo id=9717 > ./fifoConsulta\n");
  printf("\t echo producto=P.DULCE > ./fifoConsulta\n");
  printf("\t echo marca=GEORGALOS > ./fifoConsulta\n");
  printf("Para visualizar la salida usar: cat ./fifoResultado\n");
  exit(0);
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
    if(!pf) {
      printf("No se encuentra el archivo.\n");
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

void filtrarArchivo(char *path[], char *filtro, int registros, char *salida) {
    FILE *pf;
    int esId = strncmp("ID", filtro, 2);
    int esProducto = strncmp("PRODUCTO", filtro, 8);
    int esMarca = strncmp("MARCA", filtro, 5);
    int agregados = 0;
    char id[100];
    char articulo[100];
    char producto[100];
    char marca[100];
    char *igual = strchr(filtro, '=');
    char *buscado = igual;

    strcpy(salida, "\0");
    buscado++;

    if(!igual) {
      agregarSalidaIncorrecta(salida);
      return;
    }

    pf = fopen(*path, "r");
    if (!pf) {
        printf("No se  encontro el archivo %s\n", *path);
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
            agregados++;
            agregarSalida(salida, id, articulo, producto, marca);
        } else if (esMarca == 0 && strcmp(marca, buscado) == 0) {
            agregados++;
            agregarSalida(salida, id, articulo, producto, marca);
        } else if (esProducto == 0 && strcmp(producto, buscado) == 0) {
            agregados++;
            agregarSalida(salida, id, articulo, producto, marca);
        }

        strcpy(id, " ");
        strcpy(articulo, " ");
        strcpy(producto, " ");
        strcpy(marca, " ");
    }

    if(agregados == 0)
        agregarSalidaNula(salida);

    fclose(pf);

    return;
}

int main(int arg, char *args[]) {
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0))
        mostrarAyuda();

    validarParametros(arg, args);
    char filtro[100];
    char *fifo1 = args[2];
    char *fifo2 = args[3];
    // crear demonio
    int pid = fork();
    if(pid > 0) {
      printf("kill %d\n", pid);
      exit(0);
    }

    // crear fifos
    mkfifo(fifo1, 0666); //fifo consulta
    mkfifo(fifo2, 0666); //fifo resultado

    //si es el hijo se queda ejecutando
    while (1) {
        // abrir fifos
        int fConsulta = open(fifo1, O_RDONLY);
        for(int i=0; i<100; i++) { filtro[i] = '\0'; } // limpio filtro
        int bytes = -1;

        bytes = read(fConsulta, filtro, sizeof(filtro)); // leer fifo
        close(fConsulta);

        int cantCaracteres = strlen(filtro);
        char *aMayuscula = filtro;

        // pongo en mayuscula el filtro
        for (int i = 0; i < cantCaracteres; i++) {
            *aMayuscula = toupper(*aMayuscula);
            aMayuscula++;
        }

        filtro[cantCaracteres-1] = '\0';

        int registros = obtenerCantidadDeRegistros(&args[1]);
        char salida[registros];

        filtrarArchivo(&args[1], filtro, registros, salida);

        int fRespuesta = open(args[3], O_WRONLY);

        write(fRespuesta, salida, strlen(salida));

        close(fRespuesta);
    }

    return (0);
}
