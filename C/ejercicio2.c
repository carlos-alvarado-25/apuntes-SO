#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define ITERACIONES 6

pthread_t balanceador, trabajadorA, trabajadorB;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int idTarea = 0;
static volatile sig_atomic_t trabajarA = 0;
static volatile sig_atomic_t trabajarB = 0;
static volatile sig_atomic_t apagar = 0;

void manejadorWorkerA(int sig){
    trabajarA = 1;
}

void manejadorWorkerB(int sig){
    trabajarB = 1;
}

void manejadorClose(int sig){
    apagar = 1;
}

void manejadorAlarma(int sig){}

void lockHelper(int id, int valor){
    pthread_mutex_lock(&mutex);
    idTarea = valor;
    pthread_mutex_unlock(&mutex);

    if (id % 2 == 0){
        pthread_kill(trabajadorA, SIGUSR1);
        pause();
    } else {
        pthread_kill(trabajadorB, SIGVTALRM);
        pause();
    }
}

void* balanceador_f(void* arg){
    for (int i = 0; i < ITERACIONES; i++){
        int random = (rand() % 100) + 1;
        lockHelper(i, random);
    }

    pthread_kill(trabajadorA, SIGUSR2);
    pthread_kill(trabajadorB, SIGUSR2);

    return NULL;
}

void* trabajadorA_f(void* arg){
    while(1){
        while (!trabajarA && !apagar) {
            pause();
        }

        if (apagar){
            printf("Trabajador A: Apagado en progreso...\n");
            break;
        }

        if (trabajarA){
            pthread_mutex_lock(&mutex);
            int leido = idTarea;
            printf("Trabajador A procesando a: %d\n", leido);
            pthread_mutex_unlock(&mutex);

            trabajarA = 0;
            pthread_kill(balanceador, SIGALRM);
        }
    }
    return 0;
}

void* trabajadorB_f(void* arg){
    while(1){
        while (!trabajarB && !apagar) {
            pause();
        }

        if (apagar){
            printf("Trabajador B: Apagado en progreso...\n");
            break;
        }

        if (trabajarB){
            pthread_mutex_lock(&mutex);
            int leido = idTarea;
            printf("Trabajador B procesando a: %d\n", leido);
            pthread_mutex_unlock(&mutex);

            trabajarB = 0;
            pthread_kill(balanceador, SIGALRM);
        }
    }
    return 0;
}

int main(){
    srand((unsigned)time(NULL));

    signal(SIGUSR1, manejadorWorkerA);
    signal(SIGUSR2, manejadorClose);
    signal(SIGALRM, manejadorAlarma);
    signal(SIGVTALRM, manejadorWorkerB);

    pthread_create(&balanceador, NULL, balanceador_f, NULL);
    pthread_create(&trabajadorA, NULL, trabajadorA_f, NULL);
    pthread_create(&trabajadorB, NULL, trabajadorB_f, NULL);

    pthread_join(trabajadorB, NULL);
    pthread_join(trabajadorA, NULL);
    pthread_join(balanceador, NULL);

    pthread_mutex_destroy(&mutex);
    printf("--> Servidor cerrado de forma segura y limpia <--\n");

    return 0;
}
