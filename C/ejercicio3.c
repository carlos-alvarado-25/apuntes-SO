#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

static int cuentaA = 1000;
static int cuentaB = 1000;
pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;

pthread_t transferA_B, transferB_A;

void* transferA_B_f(void* arg){
    for (int i = 0; i < 100000; i++){
        pthread_mutex_lock(&mutexA);
        cuentaA -= 1;
        pthread_mutex_lock(&mutexB);
        cuentaB += 1;
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexA);
    }

    return NULL;
}

void* transferB_A_f(void* arg){
    for (int i = 0; i < 100000; i++){
        pthread_mutex_lock(&mutexA);
        cuentaA += 1;
        pthread_mutex_lock(&mutexB);
        cuentaB -= 1;
        pthread_mutex_unlock(&mutexB);
        pthread_mutex_unlock(&mutexA);
    }

    return NULL;
}

int main(){
    pthread_create(&transferA_B, NULL, transferA_B_f, NULL);
    pthread_create(&transferB_A, NULL, transferB_A_f, NULL);
    pthread_join(transferA_B, NULL);
    pthread_join(transferB_A, NULL);

    printf("Saldo final Cuenta A: %d\n", cuentaA);
    printf("Saldo final Cuenta B: %d\n", cuentaB);

    pthread_mutex_destroy(&mutexA);
    pthread_mutex_destroy(&mutexB);
}
