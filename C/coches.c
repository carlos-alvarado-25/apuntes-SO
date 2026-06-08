#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define NUM_COCHES 4

pthread_t ensamblador, pintor, mainz, hiloPrincipal;
static volatile sig_atomic_t mainzar = 0;
static volatile sig_atomic_t ensamblar = 0;
static volatile sig_atomic_t pintar = 0;
static volatile int terminar = 0;

void manejadorMainz(int sig){
    mainzar = 1;
}

void manejadorEnsamblador(int sig){
    ensamblar = 1;
}

void manejadorPintor(int sig){
    pintar = 1;
}

void* pintor_f(void* arg){

    pthread_kill(hiloPrincipal, SIGALRM);

    while(1){
        pause();

        if (terminar){
            printf("[Pintor]: No tengo más que pintar...\n");
            break;
        }

        if (pintar){
            printf("[Pintor]: Pintando ferrari...\n");
            sleep(1);
            pthread_kill(mainz, SIGALRM);
            pintar = 0;
        }
    }

    return 0;
}

void* ensamblador_f(void* arg){

    pthread_kill(hiloPrincipal, SIGALRM);

    while(1){
        pause();

        if (terminar){
            printf("[Ensamblador]: No tengo más que ensamblar...\n");
            break;
        }

        if (ensamblar){
            printf("[Ensamblador]: Ensamblando ferrari...\n");
            sleep(1);
            pthread_kill(pintor, SIGUSR2);
            ensamblar = 0;
        }
    }

    return 0;
}

void* mainz_f(void* arg){

    for (int i = 0; i < NUM_COCHES; i++){
        printf("[MAIN]: Iniciemos la fabricación de los FERRARIS %d\n", i + 1);
        pthread_kill(ensamblador, SIGUSR1);
        while(!mainzar){
            pause();
        }
        mainzar = 0;

        printf("[MAIN]: Ferrari [%d] ensamblado y pintado con éxito! \n", i + 1);
    }

    terminar = 1;

    pthread_kill(ensamblador, SIGUSR1);
    pthread_kill(pintor, SIGUSR2);

    return NULL;
}

int main(){

    hiloPrincipal = pthread_self();

    signal(SIGUSR1, manejadorEnsamblador);
    signal(SIGUSR2, manejadorPintor);
    signal(SIGALRM, manejadorMainz);

    pthread_create(&ensamblador, NULL, ensamblador_f, NULL);
    while(!mainzar) pause();
    mainzar = 0;
    pthread_create(&pintor, NULL, pintor_f, NULL);
    while(!mainzar) pause();
    mainzar = 0;
    pthread_create(&mainz, NULL, mainz_f, NULL);

    pthread_join(mainz, NULL);
    pthread_join(ensamblador, NULL);
    pthread_join(pintor, NULL);

    return 0;
}
