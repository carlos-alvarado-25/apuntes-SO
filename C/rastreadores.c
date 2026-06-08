#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define FILAS 5
#define COLUMNAS 5
#define RASTREADORES 5

int matrix[FILAS][COLUMNAS];
pthread_t supervisor;
static volatile sig_atomic_t alarma = 0;
int fila_amenaza = -1; int col_amenaza = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void manejador_supervisor(int sig) {
    alarma = 1;
}

void* supervisor_f(void* arg) {
    while (!alarma){
        pause();

        if (alarma == 1) {
            printf("¡AMENAZA NEUTRALIZADA EN LA FILA %d, COLUMNA %d!", fila_amenaza, col_amenaza);
        }
    }
}

void* rastreador_f(void* arg) {

    long id = (long)(intptr_t)arg;

    for(int i = 0; i < COLUMNAS; i++){
        pthread_mutex_lock(&mutex);

        if (alarma == 1) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int valor = matrix[id][i];

        if (valor == 99) {
            fila_amenaza = id;
            col_amenaza = i;
            pthread_kill(supervisor, SIGUSR1);
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
        usleep(100000);
    }

    return NULL;
}

int main(){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = manejador_supervisor;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    matrix[3][2] = 99;
    pthread_t rastreadores[RASTREADORES];

    pthread_create(&supervisor, NULL, &supervisor_f, NULL);

    for (int i = 0; i < RASTREADORES; i++) {
        pthread_create(&rastreadores[i], NULL, &rastreador_f, (void*)(intptr_t)i);
    }

    for (int i = 0; i < RASTREADORES; i++) {
        pthread_join(rastreadores[i], NULL);
    }

    pthread_join(supervisor, NULL);

    return 0;
}
