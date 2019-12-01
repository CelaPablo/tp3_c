/***********************************
Trabajo práctico N3 ejercicio 1
Script: ejercicio1.c
Integrantes:
Cabral, David           39757782
Cela, Pablo             36166857
Pessolani, Agustin	    39670584
Zabala, Gaston          34614948
***********************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void ayuda(char *argv[]);
void padre_hijo(int);

pid_t pid_hijo[2];
pid_t pid_nieto[3];
pid_t pid_bisnieto[5];

int main(int argc, char *argv[]){
    int cant_hijos = 2;
    int i,j,k;
    signal(SIGUSR1,padre_hijo);
    if(argc == 2){
        ayuda(argv);
    }
    else if(argc > 2){
        printf("Ingreso demasiados parámetros.\n");
        exit(2);
    }


    printf("PID: %d PPID: %d Parentesco-Tipo: Padre\n",getpid(),getppid());
    for(i = 0; i < 2; i++){
        pid_hijo[i] = fork();
        if(pid_hijo[i] == 0){
            printf("PID: %d PPID: %d Parentesco-Tipo: [hijo]-[normal]\n",getpid(),getppid());
            //HIJO 1
            if(i == 0){
                for(j = 0; j < 2; j++){
                    pid_nieto[j] = fork();
                    //NIETO 1 Y 2
                    if(pid_nieto[j] == 0){
                        printf("PID: %d PPID: %d Parentesco-Tipo: [nieto]-[normal]\n",getpid(),getppid());
                        //BISNIETO 1 Y 2 -- ZOMBIES
                        if(j == 0){
                            for(k = 0; k < 2; k++){
                                pid_bisnieto[k] = fork();
                                if(pid_bisnieto[k] == 0){
                                    printf("PID: %d PPID: %d Parentesco-Tipo: [bisnieto]-[zombie]\n",getpid(),getppid());
                                    while (1){
                                        pause();
                                        exit(0);
                                    }
                                }
                            }
                        }
                        //BISNIETO 3 Y 4 -- DEMONIOS
                        if(j == 1){
                            for(k = 2; k < 4; k++){
                                pid_bisnieto[k] = fork();
                                if(pid_bisnieto[k] == 0){
                                    printf("PID: %d PPID: %d Parentesco-Tipo: [bisnieto]-[demonio]\n",getpid(),getppid());
                                    while (1){}
                                }
                                //printf("pid [%d], k %d\n",pid_bisnieto[k],k);
                            }
                        }
                        pause();
                        if(j == 0){
                            sleep(15);
                            kill(pid_bisnieto[0],SIGUSR1);
                            kill(pid_bisnieto[1],SIGUSR1);
                        }
                        exit(2);
                        while (1){}
                    }
                }
            }
            //HIJO 2
            else if (i == 1){
                j = 2;
                pid_nieto[j] = fork();
                //NIETO 3
                if(pid_nieto[j] == 0){
                    printf("PID: %d PPID: %d Parentesco-Tipo: [nieto]-[normal]\n",getpid(),getppid());
                    k = 4;
                    pid_bisnieto[k] = fork();
                    //BISNIETO 5 -- DEMONIO
                    if(pid_bisnieto[k] == 0){
                        printf("PID: %d PPID: %d Parentesco-Tipo: [bisnieto]-[demonio]\n",getpid(),getppid());
                        while (1){}
                    }
                    //printf("pid [%d], k %d\n",pid_bisnieto[k],k);
                    while (1){}
                }
            }
            pause();
            if(i == 0){
                kill(pid_nieto[0],SIGUSR1);
                kill(pid_nieto[1],SIGKILL);
            }
            else
                kill(pid_nieto[2],SIGKILL);
            exit(2);
        }
    }
    getchar();
    kill(pid_hijo[0],SIGUSR1);
    kill(pid_hijo[1],SIGUSR1);
    sleep(10);
    return 0;
}

void ayuda(char *argv[]){
    if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0 || strcmp(argv[1],"-help") == 0){
        printf("El programa se encarga de crear una jerarquía familiar:\n");
        printf("\t* 1 padre, 2 hijos, 3 nietos y 5 bisnietos.\nAdemás cada padre solo puede tener 2 hijos como máximo.\n");
        printf("Dos procesos serán zombies y 3 serán demonios.\n");
        printf("No recibe ningún parámetro.\n");
        printf("\t ejemplo de ejecución: ./ejercicio1\n");
        exit(1);
    }
}

void padre_hijo(int sig){}
