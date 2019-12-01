/***********************************
 Trabajo práctico N3 Ejercicio 5
 Integrantes:
 Cabral, David           39757782
 Cela, Pablo             36166857
 Pessolani, Agustin      39670584
 Sullca, Fernando        37841788
 Zabala, Gaston          34614948
 ***********************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

typedef struct{
    char path[50];
    int num_cliente;
}t_info;

int cliente[10];

void mostrarAyuda(){
    printf("\n Ejemplo de ejecucion: \n ./servidor ./archivoProductos ./config ");
    printf("\n Ejemplo de ejecucion consulta: \n ./cliente \n");
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

int validarParametros(int arg, char *args[]) {

    if (arg < 2 || arg >3) {
        printf("\nCantidad de parámetros incorrecta. Utilize la ayuda.\n");
        return 1;
    }

    struct stat myFile;
    if (stat(args[1], &myFile) < 0) {
        printf("\nNo se encontro el archivo %s\n", args[1]);
        return 1;
    }

    if(arg == 3){
	    if (stat(args[2], &myFile) < 0) {
            printf("\nNo se encontro el archivo %s\n", args[2]);
        return 1;
        }
    }
    return 0;

}

int obtenerCantidadDeRegistros(char *path) {
    FILE *pf;
    int cantfilas = 0;
    pf = fopen(path, "r");
    if (!pf) {
        printf("No se pudo abrir el archivo.\n");
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

int filtrarArchivo(char *path, char *filtro, int registros, char *salida) {
    FILE *pf;
    int esId = strncmp("ID", filtro, 2);
    int esProducto = strncmp("PRODUCTO", filtro, 8);
    int esMarca = strncmp("MARCA", filtro, 5);
    int j = 0;
    char id[100];
    char articulo[100];
    char producto[100];
    char marca[100];
    char *igual = strchr(filtro, '=');
    char *buscado = igual;

    if(!strchr(filtro, '=')){
        return 1;
    }
    buscado++;
    pf = fopen(path, "r");
    if (!pf) {
        printf("\nno se  encontro el archivo %s\n", path);
        exit(0);
    }
    strcpy(id, " ");
    strcpy(articulo, " ");
    strcpy(producto, " ");
    strcpy(marca, " ");
    while (!feof(pf)) {
        j++;
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
        } else if (esMarca == 0 && strcmp(marca, buscado) == 0) {
            agregarSalida(salida, id, articulo, producto, marca);
        } else if (esProducto == 0 && strcmp(producto, buscado) == 0) {
            agregarSalida(salida, id, articulo, producto, marca);
        }
        strcpy(id, " ");
        strcpy(articulo, " ");
        strcpy(producto, " ");
        strcpy(marca, " ");
    }

    strcat(salida, "\0");
    fclose(pf);
    return 0;
}

int obtenerPuerto(){
    int puerto=0;
    FILE*  fp = fopen("config.conf", "r");
    fscanf(fp, "%d", &puerto);
    return puerto;
}

void* atender_cliente(void *info){
    int num = ((t_info*)info)->num_cliente;
    char path[50];
    strcpy(path,(char *)info);
    char *buffer = malloc(1000);
    while (1) {

		int bytesRecibidos = recv(cliente[num], buffer, 1000, 0);
		if (bytesRecibidos <= 0) {
			continue;
		}
		buffer[bytesRecibidos] = '\0';
        if(strcmp(buffer,"QUIT") == 0){
            printf("Hasta luego, vuelva pronto.\n");
            close(cliente[num]);
            break;
        }
        int registros = obtenerCantidadDeRegistros(path);
        char salida[registros * 40];

        int res = filtrarArchivo(path, buffer, registros, salida);
        if(res == 1){
            strcpy(salida,"No se encontraron resultados.");
        }
        int number_to_send = strlen(salida);
	    //Put your value
	    char salida_[number_to_send];
		int converted_number = htonl(number_to_send);
		strcpy(salida_,salida);
        // Write the number to the opened socket
		write(cliente[num], &converted_number, sizeof(converted_number));
		send(cliente[num], salida_, sizeof(salida_), 0);
	}
    free(buffer);
}

int main(int arg,char *args[]) {
	pid_t server_demonio;
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0)) {
        mostrarAyuda();
        return 0;
    }

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(obtenerPuerto());

    server_demonio = fork();
    if(server_demonio > 0)
        exit(0);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		return 1;
	}

	listen(servidor, SOMAXCONN);
	pthread_t hilo;
    int i = 0;
    t_info info[10];

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion;
    while(1){
        strcpy(info[i].path,args[1]);
        info[i].num_cliente = i;
        cliente[i] = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
        pthread_create(&hilo,NULL, atender_cliente,&info[i]);
        printf("Bienvenido cliente\n");
        i++;
    }

    //despues del accept se crea el hilos
	return 0;
}
