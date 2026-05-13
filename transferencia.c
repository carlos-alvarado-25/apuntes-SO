#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;

int cuenta_A = 1000;
int cuenta_B = 1000;

void* transferencia_A_a_B(void* arg) {
    // 1. Bloquea A
    pthread_mutex_lock(&mutex_A);
    printf("Hilo 1: Cuenta A bloqueada. Esperando Cuenta B...\n");
    
    sleep(1); // Simulamos que el sistema operativo cambia de contexto

    // 2. Intenta bloquear B
    pthread_mutex_lock(&mutex_B);
    printf("Hilo 1: Ambas cuentas bloqueadas. Transfiriendo...\n");
    cuenta_A -= 100;
    cuenta_B += 100;
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);

    return NULL;
}

void* transferencia_B_a_A(void* arg) {
    // 1. Bloquea B
    pthread_mutex_lock(&mutex_A);
    printf("Hilo 2: Cuenta B bloqueada. Esperando Cuenta A...\n");
    
    sleep(1);

    // 2. Intenta bloquear A (¡AQUÍ SE QUEDARÁ ATASCADO!)
    pthread_mutex_lock(&mutex_B); 
    printf("Hilo 2: Ambas cuentas bloqueadas. Transfiriendo...\n");
    cuenta_B -= 100;
    cuenta_A += 100;
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);

    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;

    pthread_create(&hilo1, NULL, transferencia_A_a_B, NULL);
    pthread_create(&hilo2, NULL, transferencia_B_a_A, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Transferencias completadas.\n");
    printf("Saldo A: %d | Saldo B: %d\n", cuenta_A, cuenta_B);
    return 0;
}