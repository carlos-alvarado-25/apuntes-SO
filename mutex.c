#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define N 1000000

pthread_t suma, resta;
int contador = 0;
pthread_mutex_t lock;

void funcion_suma(void* arg)
{
    for (int i = 0; i < N; i++){
        pthread_mutex_lock(&lock);
        contador=contador+1;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void funcion_resta(void* arg)
{
    for (int i = 0; i < N; i++){
        pthread_mutex_lock(&lock);
        contador=contador-1;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}


int main()
{
    contador = 0;
    
    pthread_mutex_init(&lock, NULL);

    pthread_create(&suma, NULL, &funcion_suma, NULL);
    pthread_create(&resta, NULL, &funcion_resta, NULL);
    pthread_join(suma, NULL);
    pthread_join(resta, NULL);

    printf("Resultadin: %d \n", contador);
    pthread_mutex_destroy(&lock);

    return 0;

}
