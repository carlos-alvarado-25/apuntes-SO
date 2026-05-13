#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define DISPONIBLES 150
#define NUM_USUARIOS 5

static int ticketsDisponibles = DISPONIBLES;
int tickets_deseados[NUM_USUARIOS] = {30, 40, 50, 20, 10};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t hiloSupervisor;
static volatile sig_atomic_t taquillaAgotada = 0;

void manejador_soldout(int sig) {
    taquillaAgotada = 1;
}

void venderTicket(long id) {

    pthread_mutex_lock(&mutex);

    if (ticketsDisponibles > 0) {
        usleep(1000);
        ticketsDisponibles--;

        if (ticketsDisponibles == 0) {
            printf(">>> Usuario %ld se llevó el ÚLTIMO ticket. Tocando la campana... <<<\n", id);
            pthread_kill(hiloSupervisor, SIGUSR1);
        }
    }
    
    pthread_mutex_unlock(&mutex);
}

void* comprarTickets(void* arg) {

    long id = (long)arg;

    int cantidadAComprar = tickets_deseados[id];

    printf("Usuario %ld está comprando %d tickets...\n", id, cantidadAComprar);

    for (int i = 0; i < cantidadAComprar; i++) {
        venderTicket(id);
    }

    return NULL;
}

void* funcion_supervisor(void* arg) {
    signal(SIGUSR1, manejador_soldout);

    printf("[SUPERVISOR] Taquilla abierta. Me voy a dormir. Avisadme si se acaban...\n");

    while (!taquillaAgotada) {
        pause(); 
    }

    printf("\n🚨 [SUPERVISOR] ¡ALERTA! Me han despertado. ¡Se vendió el último ticket! Colgando el cartel de SOLD OUT... 🚨\n");
    return NULL;
}

int main(){

    pthread_t hilos[NUM_USUARIOS];

    printf("🎟️ INICIO: %d TICKETS DISPONIBLES 🎟️\n\n", ticketsDisponibles);

    pthread_create(&hiloSupervisor, NULL, funcion_supervisor, NULL);
    sleep(1);

    for (long i = 0; i < NUM_USUARIOS; i++) {
        pthread_create(&hilos[i], NULL, comprarTickets, (void*)i);
    }

    for (int i = 0; i < NUM_USUARIOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_join(hiloSupervisor, NULL);

    printf("\nTickets disponibles al final: %d\n", ticketsDisponibles);

    pthread_mutex_destroy(&mutex);

    return 0;
}