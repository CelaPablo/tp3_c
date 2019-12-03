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

void mostrarAyuda(int arg, char *args[]){
    if (arg == 2 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "-?") == 0 || strcmp(args[1], "-help") == 0)) {
        printf("El cliente se encarga de envíar una consulta al servidor.\nSolo funciona si el servidor está activo.\n");
        printf("Ejemplo de ejecución: ./cliente dirección_IP config.conf \n");
        printf("Parámetros: direcció:IP, es la dirección IP del servidor. ");
        printf("Tambien se puede usar la palabra 'local', en el caso de que el server se encuentre en la misma PC.\n");
        printf("config.conf, es el archivo que contiene el puerto a conectarse. Tambien puede ser el número.\n");
        printf("En caso de necesitar ayuda sobre el servidor use el comando ./servidor -h \n");
        exit(1);
    }
}

void validarParametros(int arg, char *args[]){
    if(arg != 2 && arg != 3){
        puts("Ingreso una cantidad incorrecta de parámetros. Deben ser dos o tres");
        exit(1);
    }
    
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

void obtenerIP(char *ip){
    char cad_aux[30];
    strcpy(cad_aux,ip);
    for(int i = 0; i < 30;i++)
        cad_aux[i] = toupper(cad_aux[i]);
    if(strcmp(cad_aux,"LOCAL") == 0)
        strcpy(ip,"127.0.0.1");

}

int main(int arg, char *args[]) {
    
    const char * END_REQUEST = "QUIT";
    char arch_conf[50];
    char dir_ip[30];
    validarParametros(arg,args);
    mostrarAyuda(arg,args);
    
    strcpy(dir_ip,args[1]);
    strcpy(arch_conf,args[2]); 
    obtenerIP(dir_ip);
    
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(dir_ip);
	direccionServidor.sin_port = htons(obtenerPuerto(arch_conf));

	int socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(socket_server, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar al servidor");
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
        send(socket_server, mensaje, strlen(mensaje), 0);
        printf("Enviando la consulta...\n");
        int received_int = 0;
        int return_status = read(socket_server, &received_int, sizeof(received_int));
        char respuesta[ntohl(received_int)];
        int bytesRecibidos = recv(socket_server, respuesta, ntohl(received_int), 0);
		respuesta[bytesRecibidos] = '\0';
        if(strcmp(mensaje, END_REQUEST) != 0 && strcmp(respuesta,"QUIT") != 0){
            printf("Resultados:\n");
		    printf("%s\n", respuesta);
        }
        if(strcmp(respuesta,"QUIT") == 0){
            printf("Hubo un error con el servidor, se perdió la conección.\n");
            break;
        }
	}while (strcmp(mensaje, END_REQUEST) != 0); 
    close(socket_server);
	return 0;
}
