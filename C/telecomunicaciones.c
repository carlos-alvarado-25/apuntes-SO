#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define EMISIONES 10

pthread_t transmisor, antenaA, antenaB;
volatile int frecuenciaCanal;
volatile int antenaActiva;

static volatile sig_atomic_t alarma = 0;
static volatile sig_atomic_t apagar = 0;
static volatile sig_atomic_t ack = 0;

void manejadorAntenas(int sig){
    alarma = 1;
}

void manejadorApagado(int sig){
    apagar = 1;
}

void manejadorRebote(int sig){
    ack = 1;
}

void* transmisor_f(void* arg){
    for (int i = 0; i < EMISIONES; i++){
        int frecuenciaGenerada = (rand() % 9999) + 1000;
        frecuenciaCanal = frecuenciaGenerada;

        if (i % 2 == 0){
            antenaActiva = 1;
            pthread_kill(antenaA, SIGUSR1);
        } else {
            antenaActiva = 2;
            pthread_kill(antenaB, SIGUSR1);
        }

        while (!ack){
            pause();
        }

        ack = 0;
    }

    return NULL;
}

void* antenaA_f(void* arg){
    while(1){

        while(!alarma && !apagar){
            pause();
        }

        if (apagar){
            printf("Finalizando Proceso...\n");
            break;
        }

        if (alarma){
            if (antenaActiva == 1){
                int leido = frecuenciaCanal;
                printf("[Antena A] Emitiendo en ráfaga de frecuencia: %d Mhz\n", leido);
                alarma = 0;
                pthread_kill(transmisor, SIGALRM);
            } else {
                alarma = 0;
            }
        }
    }

    return NULL;
}

void* antenaB_f(void* arg){
    while(1){

        while(!alarma && !apagar){
            pause();
        }

        if (apagar){
            printf("Finalizando Proceso...\n");
            break;
        }

        if (alarma){
            if (antenaActiva == 2){
                int leido = frecuenciaCanal;
                printf("[Antena B] Emitiendo en ráfaga de frecuencia: %d Mhz\n", leido);
                alarma = 0;
                pthread_kill(transmisor, SIGALRM);
            } else {
                alarma = 0;
            }
        }
    }

    return NULL;
}



int main(){
    srand((unsigned)time(NULL));
    signal(SIGUSR1, manejadorAntenas);
    signal(SIGUSR2, manejadorApagado);
    signal(SIGALRM, manejadorRebote);

    pthread_create(&transmisor, NULL, transmisor_f, NULL);
    pthread_create(&antenaA, NULL, antenaA_f, NULL);
    pthread_create(&antenaB, NULL, antenaB_f, NULL);

    pthread_join(transmisor, NULL);

    printf("[Main] Apagando antenas de forma coordinada...\n");
    pthread_kill(antenaA, SIGUSR2);
    pthread_kill(antenaB, SIGUSR2);

    pthread_join(antenaA, NULL);
    pthread_join(antenaB, NULL);

    return 0;
}
