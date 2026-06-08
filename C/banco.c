#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int cuentaA = 1000;
int cuentaB = 1000;
pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;
pthread_t transferA_B, transferB_A;

void* transferA_B_f(void* arg) {

    for (int i = 0; i < 50; i++){
        pthread_mutex_lock(&mutexA);  // REGLA DE ORO, JERARQUIZAR LOCKS E INDEPENDIENTEMENTE DE LA OPERACIÓN SIEMPRE SE LOCKEAN EN EL MISMO ORDEN
        usleep(50000);
        pthread_mutex_lock(&mutexB);
        cuentaA -= 10;
        cuentaB += 10;
        printf("[Hilo 1] Transferencia de A -> B completada.\n");
        printf("Saldo: A: %d, B: %d\n", cuentaA, cuentaB);
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexA);
        usleep(1000);
    }

    return NULL;

}

void* transferB_A_f(void* arg) {

    for (int i = 0; i < 50; i++){
        pthread_mutex_lock(&mutexA);
        usleep(50000);
        pthread_mutex_lock(&mutexB);
        cuentaA += 10;
        cuentaB -= 10;
        printf("[Hilo 2] Transferencia de B -> A completada.\n");
        printf("Saldo: A: %d, B: %d\n", cuentaA, cuentaB);
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexA);
        usleep(1000);
    }

    return NULL;
}

int main(){
    pthread_create(&transferA_B, NULL, &transferA_B_f, NULL);
    pthread_create(&transferB_A, NULL, &transferB_A_f, NULL);

    pthread_join(transferA_B, NULL);
    pthread_join(transferB_A, NULL);

    printf("SALDO FINAL DE A: %d\n", cuentaA);
    printf("SALDO FINAL DE B: %d\n", cuentaB);

    pthread_mutex_destroy(&mutexA);
    pthread_mutex_destroy(&mutexB);

    return 0;
}
