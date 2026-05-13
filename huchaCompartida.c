#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define INGRESOS 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int saldo = 0;

void* funcion_ingresar(void* arg) {
    for (int i = 0; i < INGRESOS; i++) {
        pthread_mutex_lock(&mutex);
        saldo++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;

    printf("Saldo inicial: %d \n", saldo);

    pthread_create(&hilo1, NULL, funcion_ingresar, NULL);
    pthread_create(&hilo2, NULL, funcion_ingresar, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Saldo final esperado: 2000000\n");
    printf("Saldo final REAL: %d\n", saldo);

    pthread_mutex_destroy(&mutex);

    return 0;
}