#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FILAS 4

int matriz[FILAS][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};
pthread_t volcador;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile sig_atomic_t señal = 0;

void manejador_volcador(int sig){
    señal = 1;
}

void* sondeadores_f(void* arg){
    long id = (long)(intptr_t)arg;

    while(1){
        int columnaEscogida = (rand() % FILAS);
        pthread_mutex_lock(&mutex);
        int valor = matriz[id][columnaEscogida] += (rand() % 15) + 1;
        char* message = "Actualizando...\n";
        write(1, message, strlen(message));

        if (valor >= 100){
            pthread_kill(volcador, SIGUSR2);
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        usleep(50000);
    }

    return NULL;
}

void* volcador_f(void* arg){
    while (!señal){
        pause();

        if (señal) {
            pthread_mutex_lock(&mutex);
            int fd = open("kintoh_panic.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            write(fd, matriz, sizeof(matriz));
            close(fd);
            char* msg_fin = "¡VOLCADO CRITICO COMPLETADO!\n";
            write(1, msg_fin, strlen(msg_fin));
            exit(1);
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main(){

    srand((unsigned)time(NULL));

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = manejador_volcador;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    pthread_t sondeadores[FILAS];

    pthread_create(&volcador, NULL, volcador_f, NULL);

    for (int i = 0; i < FILAS; i++) {
        pthread_create(&sondeadores[i], NULL, sondeadores_f, (void*)(intptr_t)i);
    }

    for(int i = 0; i < FILAS; i++){
        pthread_join(sondeadores[i], NULL);
    }

    pthread_join(volcador, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
