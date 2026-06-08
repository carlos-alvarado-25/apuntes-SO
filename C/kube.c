#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define HILOS 2

int cargaCPU = 0;
static volatile sig_atomic_t emergencia = 0;
pthread_t monitor;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void manejador_emergencia(int sig){
    emergencia = 1;
}

void* workerNode(void* arg){

    while (!emergencia) {
        usleep(rand() % 200000);
        int valor = (rand() % 11) + 5;

        pthread_mutex_lock(&mutex);

        if (emergencia) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        cargaCPU += valor;
        printf("[Worker X] Procesando... Carga actual: %d\n", cargaCPU);

        if (cargaCPU >= 90) {
            pthread_kill(monitor, SIGUSR1);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
void* monitorf(void* arg){
    printf("[KINTOH] Control Plane iniciado. Vigilando métricas del clúster...\n");

    while (!emergencia){
        pause();

        if (emergencia) {
            printf("\n🚨 [KINTOH] ¡ALERTA CRÍTICA! Carga al %d. \nDeteniendo tráfico hacia los Worker Nodes... 🚨\n", cargaCPU);
        }
    }

    return NULL;
}

int main(){

    srand((unsigned)time(NULL));
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = manejador_emergencia;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    pthread_t workers[HILOS];

    pthread_create(&monitor, NULL, &monitorf, NULL);

    sleep(1);

    for (int i = 0; i < HILOS; i++) {
        pthread_create(&workers[i], NULL, &workerNode, (void *)(intptr_t)i);
    }

    pthread_join(monitor, NULL);

    for (int i = 0; i < HILOS; i++) {
        pthread_join(workers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;

}
