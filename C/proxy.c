#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define DESTINO_HTTP  1
#define DESTINO_HTTPS 2

#define MUESTRAS 5

static int *variableCompartida;
pid_t http, https;
static volatile sig_atomic_t alarma = 0;
static volatile sig_atomic_t apagar = 0;
static volatile sig_atomic_t ack = 0;

void handlerReverse(int sig){
    alarma = 1;
}

void handlerOff(int sig){
    apagar = 1;
}

void handlerRebote(int sig){
    ack = 1;
}

void lockHelper(int valor){
    if (valor <= 50){
        variableCompartida[1] = DESTINO_HTTP;
        kill(http, SIGUSR1);
    } else {
        variableCompartida[1] = DESTINO_HTTPS;
        kill(https, SIGUSR1);
    }

    while (!ack){
        pause();
    }

    ack = 0;
}

int main(){
    srand((unsigned)time(NULL));
    signal(SIGUSR1, handlerReverse);
    signal(SIGUSR2, handlerRebote);
    signal(SIGALRM, handlerOff);

    variableCompartida = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    http = fork();

    if (http == 0){
        while(1){
            while(!alarma && !apagar){
                pause();
            }

            if (apagar){
                printf("Procesando apagado..\n");
                break;
            }

            if (alarma){

                if (variableCompartida[1] == DESTINO_HTTP){
                    int valorLeido = variableCompartida[0];
                    printf("[Trabajador HTTP] Enrutando tráfico con factor: %d\n", valorLeido);
                    alarma = 0;
                    int ppid = getppid();
                    kill(ppid, SIGUSR2);
                } else if (variableCompartida[1] == DESTINO_HTTPS){
                    alarma = 0;
                }
            }
        }
        exit(0);
    }

    https = fork();

    if (https == 0){
        while(1){
            while(!alarma && !apagar){
                pause();
            }

            if (apagar){
                printf("Procesando apagado..\n");
                break;
            }

            if (alarma){
                if (variableCompartida[1] == DESTINO_HTTPS){
                    int valorLeido = variableCompartida[0];
                    printf("[Trabajador HTTPS] Enrutando tráfico con factor: %d\n", valorLeido);
                    alarma = 0;
                    int ppid = getppid();
                    kill(ppid, SIGUSR2);
                } else if (variableCompartida[1] == DESTINO_HTTP){
                    alarma = 0;
                }
            }
        }

        exit(0);
    }

    for (int i = 0; i < MUESTRAS; i++){
        int factorGenerado = (rand() % 100);
        variableCompartida[0] = factorGenerado;
        lockHelper(variableCompartida[0]);
    }

    printf("[Maestro] Enviando señal de cierre general...\n");
    kill(http, SIGALRM);
    kill(https, SIGALRM);

    wait(NULL);
    wait(NULL);

    munmap(variableCompartida, sizeof(int) * 2);
    printf("[Maestro] Sistema de Proxy Inverso cerrado.\n");
    return 0;

}
