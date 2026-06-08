#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define ITERACIONES 5

pthread_t sensor, central;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile int temperaturaActual = 0;
static volatile int alertas = 0;
static volatile sig_atomic_t nuevaTemperatura = 0;
static volatile sig_atomic_t alarma = 0;

void manejadorRebote(int sig){}

void manejadorNuevaTemperatura(int sig){
    nuevaTemperatura = 1;
}

void manejadorAlarma(int sig){
    alarma = 1;
}

void* central_f(void* arg){
    while(1){
        pause();

        if (nuevaTemperatura){
            pthread_mutex_lock(&mutex);
            if (temperaturaActual < 0 || temperaturaActual > 40){
                alertas += 1;
            }
            pthread_mutex_unlock(&mutex);
            nuevaTemperatura = 0;
            pthread_kill(sensor, SIGALRM);
        }

        if (alarma) {
            printf("Total de Alertas detectadas: %d\n", alertas);
            alarma = 0;
            break;
        }
    }

    return NULL;
}

void* sensor_f(void* arg){
    for (int i = 0; i < ITERACIONES; i++){
        int temperaturaGenerada = (rand() % 45) - 10;

        printf("temperaturaGenerada: %d\n", temperaturaGenerada);

        pthread_mutex_lock(&mutex);
        temperaturaActual = temperaturaGenerada;
        pthread_mutex_unlock(&mutex);

        pthread_kill(central, SIGUSR1);
        pause();
    }

    printf("MUESTREO FINALIZADO...\n");
    pthread_kill(central, SIGUSR2);

    return NULL;
}

int main(){
    srand((unsigned)time(NULL));

    signal(SIGUSR1, manejadorNuevaTemperatura);
    signal(SIGUSR2, manejadorAlarma);
    signal(SIGALRM, manejadorRebote);

    pthread_create(&central, NULL, central_f, NULL);
    pthread_create(&sensor, NULL, sensor_f, NULL);

    pthread_join(sensor, NULL);
    pthread_join(central, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
