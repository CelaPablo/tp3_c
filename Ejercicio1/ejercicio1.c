/***********************************
 Trabajo práctico N3 Ejercicio 1
 Integrantes:
 Cabral, David           39757782
 Cela, Pablo             36166857
 Pessolani, Agustin      39670584
 Sullca, Fernando        37841788
 Zabala, Gaston          34614948
 ***********************************/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TIPO(y) ((y == 'Z') ? "Zombie" : (y == 'D') ? "Demonio" : ("Normal"))
#define PARENTESCO(Y) ((Y==2)?"hijo": (Y==3) ? "nieto" : "bisnieto")

int generacion = 0;        // generación del proceso
int cantidad_hijos = 2;    // cantidad de hijos que tiene el proceso
int cantidad_zombies = 2;  // cantidad de zombies que tiene el proceso
int cantidad_demonios = 0; // cantidad de demonios que tiene el proceso
int numero_hermanos = 1;   // cantidad de hermanos que son en total en esta generación(1,2,4,8)
int ppid = -1;             // ppid del proceso que muestra sus datos
int entro = 0;              //Si debo crear los zombies de manera aleatoria

// mostrar datos del proceso en curso
void mostrarDatoProceso(char tipo);

// crear procesos hijos
void crearHijos();

// crear procesos demonios (solo pueden ser hijos de zombies)
// Retorna:
//  0: si es el proceso demonio creado
//  mayor a 0: si es el proceso que creó los demonios
int crearDemonios();

void crearZombies();

void ayuda();

int main(int arg,char** arg2 )
{
    printf("Para cerrar cada proceso se debe presionar ENTER  \n");

    /***Verfficacion********/

    if(arg>1)
    {
        if (strcmp(arg2[1],"help") || strcmp(arg2[1],"h")|| strcmp(arg2[1],"?"))
        {
            ayuda();
            return 0;
        }
        else
        {
            printf("OPTIONS DE AYUDA\n");
            printf("\t --help \t -h \t -?: muestra La  ayuda y discripcion del ejercicio\n");
            printf("EXAMPLES: muestra esta ayuda\n");
            printf("\t./ejercicio1 -h\n");
            printf("LA EJECUCCION DEL PROCESOS NO NECESITA PARAMETROS\n");
            return 0;
        }
    }
    /*******Fin de Verificacion****/
    srand(time(NULL));

    //printf("PID: %d PPID: %d Proceso Padre \n",getpid(),getppid()); No se pudo dividir...
    mostrarDatoProceso('G');

    // iniciar procreación
    crearHijos();

    // esperar una tecla (se ejecuta en todos los hijos ) y mantenerlos en memoria , poder usar "ps" o "pstree"
    getchar();
    printf("proceso [%d] finalizado\n", getpid());

    return 0;
}

void mostrarDatoProceso(char tipo)
{
    // siempre se muestran los procesos apenas son creados
    // por lo que podemos aumentar la generación antes de mostrarlos
    generacion++;

    // si es un demonio
    if (tipo == 'D')
    {
        // esperamos a que muera su padre (que se haga zombie)
        // y luego mostramos sus datos
        // para que el ppid del demonio se muestre bien
        while (ppid == getppid())
        {
        }

    }

    if(generacion==1)
    {
        printf("PID: %d PPID: %d Padre \n",getpid(),getppid());
    }
    else
        printf("PID: %d PPID: %d Parentesco-Tipo: [%s]-[%s]\n",
               getpid(),getppid(),PARENTESCO(generacion),TIPO(tipo));
}

void crearHijos()
{
    int hijo_pid;

    numero_hermanos = cantidad_hijos * generacion- 1;

    if(entro>0)
    cantidad_zombies = 0;
    /**********Procesos de crear hijos***************/
    while (cantidad_hijos)
    {
        // crear nuevo hijo
        hijo_pid = fork();

        // si estamos en el proceso hijo
        if (hijo_pid == 0)
        {

            mostrarDatoProceso('G');

            // solamente el primer proceso tiene hijos zombie(cualqueir nivel)
            if(entro>0)
                cantidad_zombies=0;

            // si aún no es la última generación
            if (generacion < 4)
            {
                // la cantidad de hijos que tiene, depende del número de
                // hermanos que tenga el proceso
                cantidad_hijos = (cantidad_hijos == numero_hermanos)? 1 : 2;

                /// crear hijos del proceso hijo ,Zombies /demonios
                /*******Valor aleatorio para zombies en diferentes niveles****/

                if(entro<1)
                {
                    int c=rand()%2;
                    /// printf("%d valor random,\n",c);

                    if(c==1 && cantidad_zombies>=1)
                    {
                        ///  printf("Creando zombies\n");
                        ///  printf("%d entro\n",entro);
                        entro++;
                /****************Zona Proceso Zombie*************************/
                        while (cantidad_zombies)
                        {
                            // crear nuevo zombie
                            hijo_pid = fork();

                            // si es el proceso hijo
                            if (hijo_pid == 0)
                            {

                                mostrarDatoProceso('Z');

                                /** el primer zombie tiene 2 demonios
                                 el segundo zombie tiene 1 demonio
                                 Total 3 demonios
                                */
                                cantidad_demonios = cantidad_zombies;

                                // si es el proceso demonio
                                if (crearDemonios() == 0)
                                {
                                    // salimos del while que fue creado en su padre
                                break;
                                }
                                // finalizar proceso para que sea zombie
                                exit(0);
                            }

                            // indicar que el zombie fue creado y crear el siguiente si lo hay
                            cantidad_zombies--;
                        }
                    }
                }
                /*******************FIN de Zombie******************************************************/
                crearHijos();
            }

            // como es el hijo, salimos del while
            // porque el while fue creado en el padre
            break;
        }
        /**************************FIN HIJO******************/

        // indicar que el hijo fue creado y crear el siguiente si lo hay
        cantidad_hijos--;

    }

    /**********Procesos de Padre ***************/

/// crearZombies(); Los crea para el primero unicamente
/***

if(entro==0 && cantidad_zombies>0 && generacion <4)
crearZombies();***/

//printf("%d %d\n",cantidad_demonios,cantidad_zombies);

/**if(cantidad_zombies>0 && entro ==0)
    crearZombies();
*/
}

void crearZombies()
{
    int hijo_pid;
// crear zombies
    while (cantidad_zombies)
    {
        // crear nuevo zombie
        hijo_pid = fork();

        // si es el proceso hijo
        if (hijo_pid == 0)
        {
            // mostrar datos del proceso zombie
            mostrarDatoProceso('Z');

            // si es zombie, puede tener hijos demonio
            // el primer zombie tiene 2 demonios
            // el segundo zombie tiene 1 demonio

            cantidad_demonios = cantidad_zombies;

            // si es el proceso demonio
            if (crearDemonios() == 0)
            {
                // salimos del while que fue creado en su padre
                break;
            }

            // finalizar proceso para que sea zombie
            exit(0);
        }

        // indicar que el zombie fue creado y crear el siguiente si lo hay
        cantidad_zombies--;
    }

}

int crearDemonios()
{
    int hijo_pid;

    // los demonios muestran sus datos, luego de que cambie su ppid
    ppid = getpid();

    while (cantidad_demonios)
    {
        // crear nuevo demonio
        hijo_pid = fork();

        // si estamos dentro del proceso demonio
        if (hijo_pid == 0)
        {
            // mostrar datos del proceso demonio
            mostrarDatoProceso('D');

            while (1)
            {
                // mantener proceso demonio vivo
            }
            // salir del while porque fue creado
            // para el proceso padre
            return 0;
        }
        // indicar que el demonio fue creado y crear el siguiente si lo hay
        cantidad_demonios--;
    }

    return getpid();
}

void ayuda()
{
    printf("NAME\n");
    printf("\tejercicio1\n");
    printf("DESCRIPTION\n");
    printf("\tCreacion de procesos hijos en diferente niveles y tipos de procesos(Normales ,demonios,zombies)\n");
    printf(" NOTA:\n Los zombie finalizan junto con el padre \n Los demonios quedan corriendo  \n");
    printf("SYNOPSIS\n");
    printf("\t./ejercicio1 OPTION\n");
    printf("OPTIONS\n");
    printf("\t --help: muestra esta ayuda\n");
    printf("\t (Sin opciones): muestra los datos de los procesos generados\n");
    printf("EXAMPLES:\n");
    printf("\t./ejercicio1 -h\n : Ejecucion de ayuda");
    printf("\t./ejercicio1 : Ejecucion normal\n");

}
