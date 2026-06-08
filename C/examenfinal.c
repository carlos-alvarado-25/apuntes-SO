#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define ROWS 4
#define COLS 3

int matriz[ROWS][COLS];
pthread_t coordinador, positivo, negativo;
int columnaResultante, filaResultante;

static volatile sig_atomic_t coordinadora = 0;
static volatile sig_atomic_t positiva = 0;
static volatile sig_atomic_t negativa = 0;
static volatile sig_atomic_t terminar = 0;

void manejador_coordinador(int sig){
    coordinadora = 1;
}
void manejador_positivo(int sig){
    positiva = 1;
}
void manejador_negativo(int sig){
    negativa = 1;
}

void* positivo_f(void* arg){

    pthread_kill(coordinador, SIGALRM);

    while(!terminar) {
        pause();

        if (terminar) break;

        if (positiva){
            int numero;
            printf("Positivo - Ingrese un numero: \n");
            scanf("%d", &numero);
            matriz[filaResultante][columnaResultante] = numero;

            positiva = 0;
            pthread_kill(coordinador, SIGALRM);
        }
    }

    return NULL;

}

void* negativo_f(void* arg){

    pthread_kill(coordinador, SIGALRM);

    while(!terminar) {
        pause();

        if (terminar) break;

        if (negativa){
            int numero;
            printf("Negativo - Ingrese un numero: \n");
            scanf("%d", &numero);
            matriz[filaResultante][columnaResultante] = numero;

            negativa = 0;
            pthread_kill(coordinador, SIGALRM);
        }
    }

    return NULL;
}

void* coordinador_f(void* arg){

    pthread_create(&positivo, NULL, &positivo_f, NULL);
    while (!coordinadora){
        pause();
    }
    coordinadora = 0;

    pthread_create(&negativo, NULL, &negativo_f, NULL);
    while (!coordinadora){
        pause();
    }
    coordinadora = 0;

    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if ((i * j) % 2 == 0 || (i*j) == 0){
                filaResultante = i;
                columnaResultante = j;
                pthread_kill(positivo, SIGUSR1);
                pause();
            } else {
                filaResultante = i;
                columnaResultante = j;
                pthread_kill(negativo, SIGUSR2);
                pause();
            }
        }
    }

    printf("\n--- MATRIZ RESULTANTE ---\n");
        for (int i = 0; i < ROWS; i++){
            for (int j = 0; j < COLS; j++){
                printf("[%3d] ", matriz[i][j]);
            }
            printf("\n");
        }
        printf("-------------------------\n");

    terminar = 1;
    pthread_kill(negativo, SIGUSR2);
    pthread_kill(positivo, SIGUSR1);

    pthread_join(negativo, NULL);
    pthread_join(positivo, NULL);

    return NULL;
}

int main(){

    signal(SIGALRM, manejador_coordinador);
    signal(SIGUSR1, manejador_positivo);
    signal(SIGUSR2, manejador_negativo);

    pthread_create(&coordinador, NULL, &coordinador_f, NULL);
    pthread_join(coordinador, NULL);

    printf("PROGRAMA TERMINADO\n");

    return 0;
}
