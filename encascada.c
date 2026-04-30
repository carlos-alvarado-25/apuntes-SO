#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LIMITE 5

pthread_t array[LIMITE];
int n = 0;

void calcula() {
    long long unsigned t;
    for (t = 0; t < (0xFFFFFFF); t++);
}

void *creaThread(void *arg) {

    printf("He nacido. Soy el hilo con ID %lu\n", pthread_self());

    n++;
    calcula();

    if (n < LIMITE) {
        pthread_create(&array[n], NULL, &creaThread, NULL);
        pthread_join(array[n], NULL);
    } else {
        char g[10];
        fgets(g, sizeof(g), stdin);
    }

    printf("Soy el hilo con ID %lu y acabo mi ejecución\n", pthread_self());

    return 0;
}

int main(void) {
    if (pthread_create(&array[n], NULL, &creaThread, NULL) != 0) {
        perror("Error al crear el hilo");
        return 1;
    }

    pthread_join(array[0], NULL);

    return 0;
}