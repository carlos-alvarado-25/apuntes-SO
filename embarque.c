#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define CAPACIDAD 50
#define NUM_COMPUERTAS 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int astronautas_a_bordo = 0;

pthread_t hilo_control;
static volatile sig_atomic_t nave_lista = 0;

void manejador_supervisor(int sig) {
    nave_lista = 1;
}

int abordar_astronautas(long id) {
    int pudo_abordar = 0;

    pthread_mutex_lock(&mutex);

    if (astronautas_a_bordo < CAPACIDAD) {
        astronautas_a_bordo++;
        printf("COMPUERTA [%ld]: Astronauta a bordo. Total %d/50 \n", id, astronautas_a_bordo);
        pudo_abordar = 1;

        if (astronautas_a_bordo == CAPACIDAD) {
            pthread_kill(hilo_control, SIGUSR2);
        }
    }

    pthread_mutex_unlock(&mutex);

    return pudo_abordar;
}

void* embarcar(void* arg) {

    long id = (long)arg;

    while(1) {
        
        if (abordar_astronautas(id) == 0) {
            break;
        }

        usleep(1000);
    }

    return NULL;
}

void* funcion_supervisor(void* arg) {
    signal(SIGUSR2, manejador_supervisor);

    printf("[TORRE] Iniciando protocolo de embarque. A la espera de confirmación...\n");

    while (!nave_lista) {
        pause();
    }

    printf("\n🚨 [TORRE] Capacidad al 100%%. ¡Iniciando secuencia de lanzamiento! 🚀\n");

    return NULL;
}

int main() {
    pthread_t hilos[NUM_COMPUERTAS];

    printf("INICIA DESPEGUE... \n\n");

    pthread_create(&hilo_control, NULL, funcion_supervisor, NULL);
    sleep(1);

    for (long i = 0; i < NUM_COMPUERTAS; i++) {
        pthread_create(&hilos[i], NULL, embarcar, (void*)i);
    }

    for (int i = 0; i < NUM_COMPUERTAS; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_join(hilo_control, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}