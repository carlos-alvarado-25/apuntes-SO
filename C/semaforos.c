#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NUMERO_DE_COCHES 5

sem_t plazas;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int cochesAtendidos = 0;

void* coches_f(void* arg){
    long id = (long)(intptr_t)arg;

    sem_wait(&plazas);
    printf("Coche %ld: Llega y espera para entrar...\n", id);

    sleep(1);

    pthread_mutex_lock(&mutex);
    cochesAtendidos++;
    pthread_mutex_unlock(&mutex);

    printf("Coche %ld: [SALE] Abandona el parking.\n", id);

    sem_post(&plazas);

    return NULL;

}

int main(){
    pthread_t coches[NUMERO_DE_COCHES];

    if (sem_init(&plazas, 0, 3) != 0) {
            perror("Error inicializando el semáforo");
            return -1;
    }

    for(int i = 0; i < NUMERO_DE_COCHES; i++){
        pthread_create(&coches[i], NULL, &coches_f, (void*)(intptr_t)i);
    }

    for(int i = 0; i < NUMERO_DE_COCHES; i++){
        pthread_join(coches[i], NULL);
    }

    printf("\n--- FIN DEL DÍA ---\n");
    printf("Total de coches atendidos hoy: %d\n", cochesAtendidos);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&plazas);

    return 0;
}
