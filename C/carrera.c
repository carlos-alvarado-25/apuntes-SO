#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define CORREDORES 4

pthread_t corredores[CORREDORES];

void manejador_corredor(int sig){

}

void* hiloCorredor_f(void* arg){
    long id = (long)(intptr_t)arg;

    if (id > 0) {
        pause();
    }

    printf("[Corredor %ld] ¡Tengo el testigo! Corriendo...\n", id);
    sleep(1);

    if (id < CORREDORES - 1) {
        printf("[Corredor %ld] Pasando el testigo al Corredor %ld...\n", id, id + 1);
        pthread_kill(corredores[id + 1], SIGUSR1);
    }

    return NULL;
}

int main() {

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = manejador_corredor;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    for (int i = CORREDORES - 1; i > 0; i--) {
        pthread_create(&corredores[i], NULL, &hiloCorredor_f, (void*)(intptr_t) i);
    }

    sleep(1);

    pthread_create(&corredores[0], NULL, &hiloCorredor_f, (void*)(intptr_t) 0);

    for (int i = 0; i < CORREDORES; i++) {
        pthread_join(corredores[i], NULL);
    }

    printf("\nCarrera Terminada");

    return 0;
}
