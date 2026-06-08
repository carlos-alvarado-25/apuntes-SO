#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>


#define ROWS 4
#define COLS 3

static int matriz[ROWS][COLS];
pthread_t coordinador, positivo, negativo;
static sig_atomic_t coordinadora = 0;
static sig_atomic_t positiva = 0;
static sig_atomic_t negativa = 0;
static sig_atomic_t terminar = 0;
static volatile int filaActual = 0;
static volatile int columnaActual = 0;

void manejadorCoordinador(int sig){
    coordinadora = 1;
}

void manejadorNegativo(int sig){
    negativa = 1;
}

void manejadorPositivo(int sig){
    positiva = 1;
}

void* negativo_f(void* arg){

    pthread_kill(coordinador, SIGUSR1);

    while(!terminar){
        pause();

        if (terminar) break;

        printf("[Negativa] Desperté. Voy a pedir números...\n");

        if (negativa){
            int numero;
            printf("Ingresa los números a la matriz: ");
            scanf("%d", &numero);
            matriz[filaActual][columnaActual] = numero;
            negativa = 0;
            pthread_kill(coordinador, SIGUSR1);
        }
    }

    return NULL;
}

void* positivo_f(void* arg){

    pthread_kill(coordinador, SIGUSR1);

    while(!terminar){
        pause();

        if (terminar) break;

        printf("[Positivo] Desperté. Voy a pedir números...\n");

        if (positiva){
            int numero;
            printf("Ingresa los números a la matriz: ");
            scanf("%d", &numero);
            matriz[filaActual][columnaActual] = numero;
            positiva = 0;
            pthread_kill(coordinador, SIGUSR1);
        }
    }

    return NULL;
}

void* coordinador_f(void* arg){

    pthread_create(&positivo, NULL, positivo_f, NULL);
    while(!coordinadora){
        pause();
    }
    coordinadora = 0;

    pthread_create(&negativo, NULL, negativo_f, NULL);
    while(!coordinadora){
        pause();
    }
    coordinadora = 0;

    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if (i * j % 2 == 0 || i*j == 0){
                filaActual = i;
                columnaActual = j;
                pthread_kill(positivo, SIGUSR2);
                while(!coordinadora){
                    pause();
                }
                coordinadora = 0;
            } else {
                filaActual = i;
                columnaActual = j;
                pthread_kill(negativo, SIGALRM);
                while(!coordinadora){
                    pause();
                }
                coordinadora = 0;
            }
        }
    }

    printf("\n--- MATRIZ RESULTANTE ---\n");
        for (int i = 0; i < ROWS; i++){
            for (int j = 0; j < COLS; j++){
                printf("[%d] ", matriz[i][j]);
            }
            printf("\n");
        }
        printf("-------------------------\n");

    terminar = 1;

    pthread_kill(positivo, SIGUSR2);
    pthread_kill(negativo, SIGALRM);

    pthread_join(positivo, NULL);
    pthread_join(negativo, NULL);

    return NULL;
}

int main(){

    signal(SIGUSR1, manejadorCoordinador);
    signal(SIGUSR2, manejadorPositivo);
    signal(SIGALRM, manejadorNegativo);

    pthread_create(&coordinador, NULL, coordinador_f, NULL);
    pthread_join(coordinador, NULL);

    return 0;

}
