/*********************************** 
Trabajo práctico N3 ejercicio 2
Script: ejercicio2.c
Integrantes:
Cabral, David           39757782
Cela, Pablo             36166857
Pessolani, Agustin	    39670584
Sullca, Fernando        37841788
Zabala, Gaston          34614948 
***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct valores{
    double  valor1,
            valor2;
    int     es_valido;
}t_valor; //Estructura para los valores obtenidos de un archivo 

typedef struct infoThread{
    int num_hilo,
        cantidad,//cantidad de elementos a analizar
        es_primero,
        primera_pos;
}t_infoThread; //Estructura para los datos de un hilo

//***************Prototipos de funciones *****************
void ayuda(char *argv[]);
void validaciones(int argc, char *argv[]);

void leer_archivo(char *path_entrada);
int obtener_cantidad_de_elementos();
void manejar_hilos(int cant_elementos, int cant_hilos);
void elementos_por_hilo(t_infoThread *infoThread, int cant_elementos, int cant_hilos);
int obtener_primera_pos(t_infoThread *infoThread, int num_hilo);
void crear_salida(int cant_elementos);
void* suma_de_vectores(void *infoThread);

t_valor vector_valores[10000];//Contiene la información del archivo
double vector_resultado[10000];//Contiene el resultado de la suma

int main(int argc, char *argv[]){
    int i = 0, cant_hilos, cant_elementos;
    validaciones(argc,argv);
    ayuda(argv);

    leer_archivo(argv[1]);
    cant_elementos = obtener_cantidad_de_elementos();
    cant_hilos = atoi(argv[2]);
    manejar_hilos(cant_elementos,cant_hilos);
    crear_salida(cant_elementos);
    return 0;
}

//*************** Funciones *****************
void ayuda(char *argv[]){
    if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0 || strcmp(argv[1],"-help") == 0){
        printf("El programa se encarga de sumar dos vectores, usando hilos.\nEl resultado lo guarda en un archivo.\n");
        printf("Los parámetros que recibe son:\n");
        printf("Param1: Entrada.txt, ruta del archivo donde se encuentran los datos, del tipo String.\n");
        printf("Param2: N, cantidad de hilos a utilizar, del tipo Entero.\n");
        printf("\nEjemplo: ./ejercicio2 entrada.txt 3\n");
        exit(1);
    }
}

void validaciones(int argc, char *argv[]){
    FILE *archivo;
    if(argc != 2 && argc != 3){
        puts("Ingreso una cantidad incorrecta de parámetros.");
        exit(2);
    }    
    if(!strcmp(argv[1],"-h") == 0 && !strcmp(argv[1],"-?") == 0 && !strcmp(argv[1],"-help") == 0){
        archivo = fopen(argv[1],"r");
        fseek(archivo, 0, SEEK_END);
        if(ftell(archivo) == 0){
            puts("El archivo está vacío. El archivo debe contener como mínimo un elemento.");
            fclose(archivo);
            exit(3);
        }
        fclose(archivo);
    }
}

//Lee desde el archivo la información y la guarda en un vector
void leer_archivo(char *path_entrada){
    int i = 0;
    FILE *arch_entrada = fopen(path_entrada,"r");
    if (!arch_entrada){
        printf("Error al abrir el archivo de entrada.\n");
        exit(4);
    }
    while (!feof(arch_entrada)){
        fscanf(arch_entrada,"%lf,%lf",&vector_valores[i].valor1,&vector_valores[i].valor2);        
        if(feof(arch_entrada))
            break;
        vector_valores[i].es_valido = 1;
        i++;
    }
    fclose(arch_entrada);
}

//Cuenta cuanta posiciones poseen datos válidos
int obtener_cantidad_de_elementos(){
    int cant = 0,i;
    int tam_array = (sizeof(vector_valores)) / sizeof((vector_valores)[0]);
    for (i = 0; i < tam_array; i++){
        if(vector_valores[i].es_valido == 1)
            cant++;
    }
    return cant;
}

//Se crean y luego se esperan a los hilos
void manejar_hilos(int cant_elementos, int cant_hilos){
    pthread_t vector_hilos[cant_hilos];
    t_infoThread infoThread[cant_hilos];
    int i;
    elementos_por_hilo(infoThread,cant_elementos,cant_hilos);
    for(i = 0; i < cant_hilos; i++){
        printf("El hilo %d va a trabajar con %d elementos.\n",infoThread[i].num_hilo,infoThread[i].cantidad);
        pthread_create(&(vector_hilos[i]), NULL, suma_de_vectores, &infoThread[i]);
    }
    for(i = 0; i < cant_hilos; i++){
        pthread_join(vector_hilos[i],NULL);
    }
}

//Calculo con cuantos elementos va a trabajar cada hilo
void elementos_por_hilo(t_infoThread *infoThread, int cant_elementos, int cant_hilos){
    int i, cant_restante;
    cant_restante = cant_elementos;
    for(i = 0; i < cant_hilos; i++){
        infoThread[i].num_hilo = i+1;
        infoThread[i].cantidad = 0;
        if(i == 0)
            infoThread[i].es_primero = 1;
        else
            infoThread[i].es_primero = 0;
    }
    while(cant_restante > 0){
        for(i = 0; i < cant_hilos; i++){
            if(cant_restante > 0){
                infoThread[i].cantidad++;
                cant_restante --;
            }
            else
                break;
        }
    }
    for(i = 0; i < cant_hilos; i++)
        infoThread[i].primera_pos = obtener_primera_pos(infoThread,i);
}

//Busco la primera posición para cada hilo dentro del vector
int obtener_primera_pos(t_infoThread *infoThread, int num_hilo){
    int pos = 0, i;
    for(i = 0; i < num_hilo; i++)
        pos += infoThread[i].cantidad;
    return pos;
}

//La suma de los dos datos, esta es la función con la que trabajan los hilos
void* suma_de_vectores(void *infoThread){
    int num_hilo = ((t_infoThread*)infoThread)->num_hilo;
    int cant_elem = ((t_infoThread*)infoThread)->cantidad;
    int es_primero = ((t_infoThread*)infoThread)->es_primero;
    int pos = ((t_infoThread*)infoThread)->primera_pos;
    while(cant_elem > 0){
        vector_resultado[pos] = vector_valores[pos].valor1 + vector_valores[pos].valor2;
        pos++;
        cant_elem--;
    }
}

//Creo el archivo de salida
void crear_salida(int cant_elementos){
    int i;
    FILE *arch_salida = fopen("./salida.txt","w");
    if(!arch_salida){
        puts("No se pudo crear el archivo de salida.");
        exit(5);
    }
    for (i = 0; i < cant_elementos; i++)
        fprintf(arch_salida,"%lf\n",vector_resultado[i]);
    fclose(arch_salida);
}

//***************Fin del script *****************