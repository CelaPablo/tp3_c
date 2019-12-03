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
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>

typedef struct{
    char path[50];
    int num_cliente;
}t_info;

int servidor;
int cliente[128];
int clientes_activos[128] = {};

void controlar_salida(int sig);

void mostrarAyuda(int arg, char *args[]){
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0)) {
        printf("El servidor se encarga de atender las consultas de los clientes.\n");
        printf("Ejemplo de ejecución: ./servidor articulos.txt config.conf \n");
        printf("Parámetros: articulos.txt, es el archivo que contiene los artículos a buscar.\n");
        printf("config.conf, es el archivo que contiene el puerto a conectarse. Tambien puede ser el número.\n");
        printf("Para cerrar el servidor usar el comando kill -15 [PID server]\n");
        printf("En caso de necesitar ayuda sobre el cliente use el comando ./cliente -h \n");
        exit(1);
    }
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
    return 0;
}

int esNumero(char *arch_config){
    char cad_aux[50];
    strcpy(cad_aux,arch_config);
    for(int i = 0; i < strlen(cad_aux);i++){
        if((cad_aux[i] >= 'a' && cad_aux[i] <= 'z' ) || (cad_aux[i] >= 'A' && cad_aux[i] <= 'Z' ))
            return 0;
    }
    return 1;
}

int obtenerPuerto(char *arch_config){
    int puerto = 0;
    FILE*  fp = fopen(arch_config, "r");
    if(fp)
        fscanf(fp, "%d", &puerto);
    else if(esNumero(arch_config))
        puerto = atoi(arch_config);
    else{
        printf("Ingreso un puerto incorrecto\n");
        exit(1);
    }
    return puerto;
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

int buscar_cliente_valido() {
    int pos = 0;
    while(clientes_activos[pos] != 0 || pos == 127)
        pos++;
    return pos;
}

int validarPosicion(int i){
    return clientes_activos[i];
}

int filtrarArchivo(char *path, char *filtro, int registros, char *salida) {
    FILE *pf;
    int esId = strncmp("ID", filtro, 2);
    int esProducto = strncmp("PRODUCTO", filtro, 8);
    int esMarca = strncmp("MARCA", filtro, 5);
    int noHayCoincidencias = 0;
    int noAgrego = 0;
    char id[100];
    char articulo[100];
    char producto[100];
    char marca[100];
    char *igual = strchr(filtro, '=');
    char *buscado = igual;
    
    if(!strchr(filtro, '=')){
        return 1;
    }
    strcpy(salida,"");
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
            noHayCoincidencias = 0;
            noAgrego = 1;
        } else if (esMarca == 0 && strcmp(marca, buscado) == 0) {
            agregarSalida(salida, id, articulo, producto, marca);
            noHayCoincidencias = 0;
            noAgrego = 1;
        } else if (esProducto == 0 && strcmp(producto, buscado) == 0) {
            agregarSalida(salida, id, articulo, producto, marca);
            noHayCoincidencias = 0;
            noAgrego = 1;
        } else if(esId != 0 && esMarca != 0 && esProducto != 0){
            noHayCoincidencias = 1;
            noAgrego = 0;
        }
        if(noAgrego == 0)
            noHayCoincidencias = 1;
        strcpy(id, " ");
        strcpy(articulo, " ");
        strcpy(producto, " ");
        strcpy(marca, " ");
    }

    strcat(salida, "\0");
    fclose(pf);
    return noHayCoincidencias;
}

void* atender_cliente(void *info){
    int num = ((t_info*)info)->num_cliente;
    char path[50]; 
    strcpy(path,((t_info*)info)->path);
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
    clientes_activos[num] = 0;
}

int main(int arg,char *args[]) {
	mostrarAyuda(arg,args);
    validarParametros(arg,args);

    char arch_conf[50];
    pid_t server_demonio;
    strcpy(arch_conf,args[2]);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(obtenerPuerto(arch_conf));
    
    server_demonio = fork();
    if(server_demonio > 0)
        exit(0);
    signal(SIGINT, controlar_salida);
    signal(SIGTERM, controlar_salida);
    
    
	servidor = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		return 1;
	}
    
	listen(servidor, SOMAXCONN);
	pthread_t hilo;
    int pos;
    t_info info[128];

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion;
    while(1){
        pos = buscar_cliente_valido();
        strcpy(info[pos].path,args[1]);
        info[pos].num_cliente = pos;
        cliente[pos] = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
        clientes_activos[pos] = 1;
        pthread_create(&hilo,NULL, atender_cliente,&info[pos]);
        printf("Bienvenido cliente\n");
    }
	return 0;
}

void controlar_salida(int signal) {
    int i = 0;
    char cadena_salida[5] = "QUIT";
    int number_to_send;
	int converted_number;
    number_to_send = strlen(cadena_salida);
    converted_number = htonl(number_to_send);
    char salida_[number_to_send];
    printf("Cerrando el servidor...\n");
    while(i<128){
        if(validarPosicion(i) == 1){
	        strcpy(salida_,cadena_salida);
	        write(cliente[i], &converted_number, sizeof(converted_number));
	        send(cliente[i], salida_, sizeof(salida_), 0);
            close(cliente[i]);
        }
        i++;
    }
    close(servidor);
    exit(1);
}