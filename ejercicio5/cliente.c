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
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


int validarParametros(int arg, char *args[]){
    if (arg != 2){
        return 1;
    }
    return 0;
}

void mostrarAyuda(){
    printf("\n Ejemplo de ejecucion: \n ./servidor ./archivoProductos ./config ");
    printf("\n Ejemplo de ejecucion consulta: \n ./cliente \n");
}
int obtenerPuerto(){
    int puerto=0;
    FILE*  fp = fopen("config.conf", "r");
    fscanf(fp, "%d", &puerto);
    return puerto;

}

int main(int arg, char *args[]) {
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0)) {
        mostrarAyuda();
        return 0;
    }
    const char * END_REQUEST = "QUIT";
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(obtenerPuerto());
	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
	    return 1;
	}
    char mensaje[1000];	
	do{     
        printf("Ingrese su consulta:\n");
        scanf("%s", mensaje);
        int cantCaracteres = strlen(mensaje);
        char *aMayuscula = mensaje;
        for (int i = 0; i < cantCaracteres; i++){
            *aMayuscula = toupper(*aMayuscula);
            aMayuscula++;
        }
        send(cliente, mensaje, strlen(mensaje), 0);
        printf("Enviando la consulta...\n");
        int received_int = 0;
        int return_status = read(cliente, &received_int, sizeof(received_int));
        char respuesta[ntohl(received_int)];
        int bytesRecibidos = recv(cliente, respuesta, ntohl(received_int), 0);
		respuesta[bytesRecibidos] = '\0';
        if(strcmp(mensaje, END_REQUEST) != 0){
            printf("Resultados:\n");
		    printf("%s\n", respuesta);
        }
	}while (strcmp(mensaje, END_REQUEST) != 0); 

	return 0;
}
