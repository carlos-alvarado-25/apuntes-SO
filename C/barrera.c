#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PROCESADORES 5

int matriz[5][5] = {
    {3, 1, 2, 3, 4},
    {7, 1, 3, 3, 4},
    {5, 1, 2, 21, 4},
    {2, 1, 4, 3, 4},
    {0, 1, 2, 3, 4},
};
int hilos_terminados = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t escritor;
static volatile sig_atomic_t bandera = 0;

void manejador_escritor(int sig){
    bandera = 1;
}

void* escritor_f(void* arg){

    while (!bandera) {
        pause();

        if (bandera) {
            pthread_mutex_lock(&mutex);
            int fd = open("matriz_final.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            write(fd, matriz, sizeof(matriz));
            close(fd);
            pthread_mutex_unlock(&mutex);
            char* msg_fin = "PROCESAMIENTO TERMINADO\n";
            write(1, msg_fin, strlen(msg_fin));
            exit(0);
        }
    }

}

void* procesadores_f(void* arg) {
    long id = (long)(intptr_t)arg;

    for (int j = 0; j < 5; j++){
        matriz[id][j] *= 2;
        usleep(50000);
    }

    pthread_mutex_lock(&mutex);
    hilos_terminados += 1;

    if (hilos_terminados >= 5){
        pthread_kill(escritor, SIGUSR1);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = manejador_escritor;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    pthread_t procesadores[PROCESADORES];

    pthread_create(&escritor, NULL, &escritor_f, NULL);

    for (int i = 0; i < PROCESADORES; i++){
        pthread_create(&procesadores[i], NULL, procesadores_f, (void*)(intptr_t)i);
    }

    for (int i = 0; i < PROCESADORES; i++){
        pthread_join(procesadores[i], NULL);
    }

    pthread_join(escritor, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
