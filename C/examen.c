#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define PRESIONES 4

int presiones[PRESIONES];
int sensoresListos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t computadoraCentral;
static volatile sig_atomic_t telemetria_lista = 0;

void sig_handler(int sig){
    telemetria_lista = 1;
}

void* computadoraCentral_f(void* arg){

    signal(SIGUSR1, sig_handler);

    printf("[CENTRALITA] Sistema iniciado. Esperando telemetría de los 4 neumáticos...\n");

    while(!telemetria_lista) {
        pause();
    }

    printf("\n[CENTRALITA] Telemetría recibida. Analizando datos...\n");

    int correcto = 1;

    for (int i = 0; i < sensoresListos; i++) {
        printf(" PRESION [%d]: %d PSI\n", i, presiones[i]);

        if (presiones[i] < 28) {
            correcto = 0;
        }
    }

    if (correcto == 1) {
        printf("✅ [OK] Presiones correctas. Motor arrancado.\n");
    } else {
        printf("🚨 [ALERTA] Presión baja detectada. Arranque bloqueado.\n");
    }

    return NULL;

}

void* hilos_f(void* arg){
    long id = (long)(intptr_t)arg;
    usleep(rand() % 500000);

    int presion_detectada = (rand() % 11) + 25;

    pthread_mutex_lock(&mutex);
    presiones[id] = presion_detectada;
    sensoresListos++;

    printf("[SENSOR %ld] Lectura completada: %d PSI. (Total listos: %d/4)\n",
               id, presion_detectada, sensoresListos);

    if (sensoresListos == 4) {
        printf("[SENSOR %ld] ¡Soy el último! Enviando señal de telemetría completa...\n", id);
        pthread_kill(computadoraCentral, SIGUSR1);
    }
    pthread_mutex_unlock(&mutex);
}

int main() {

    pthread_t hilos[4];

    pthread_create(&computadoraCentral, NULL, &computadoraCentral_f, NULL);
    sleep(1);

    for(int i = 0; i < 4; i++){
        pthread_create(&hilos[i], NULL, &hilos_f, (void *)(intptr_t)i);
    }

    for(int i = 0; i < 4; i++){
        pthread_join(hilos[i], NULL);
    }

    pthread_join(computadoraCentral, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;

}
