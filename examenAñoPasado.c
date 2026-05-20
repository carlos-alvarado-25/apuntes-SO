#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define ROWS 4
#define COLS 3

int matriz[ROWS][COLS], ipos, jpos;
pthread_t coordinador, positivo, negativo;

void positivo_handler(int sig) {}
void negativo_handler(int sig) {}
void coordinador_handler(int sig) {}

void* positivo_f(){
    pthread_kill(coordinador, SIGALRM);
    
    while(1) {
        pause();
        
        printf("[POSITIVO] Introduce valor para matriz[%d][%d]: ", ipos, jpos);
        scanf("%d", &matriz[ipos][jpos]);
        
        pthread_kill(coordinador, SIGALRM);
    }
    return NULL;
}

void* negativo_f(){
    pthread_kill(coordinador, SIGALRM);
    
    while(1) {
        pause();
        
        printf("[NEGATIVO] Introduce valor para matriz[%d][%d]: ", ipos, jpos);
        scanf("%d", &matriz[ipos][jpos]);
        
        pthread_kill(coordinador, SIGALRM);
    }
    return NULL;
}

void* coordinador_f(){
	
    pthread_create(&positivo, NULL, positivo_f, NULL);
    pause(); 
    
    pthread_create(&negativo, NULL, negativo_f, NULL);
    pause(); 

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int indiceActual = i * j;
            
            ipos = i;
            jpos = j;
            
            if (indiceActual % 2 == 0) {
                pthread_kill(positivo, SIGUSR1);
            } else {
                pthread_kill(negativo, SIGUSR2);
            }
            
            pause();
        }
    }
    
    printf("\n--- MATRIZ COMPLETADA ---\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }
    printf("-------------------------\n");
    
    printf("[COORDINADOR] Eliminando hilo negativo...\n");
    pthread_cancel(negativo);
    pthread_join(negativo, NULL);
    
    printf("[COORDINADOR] Eliminando hilo positivo...\n");
    pthread_cancel(positivo);
    pthread_join(positivo, NULL);
    
    printf("[COORDINADOR] Tareas finalizadas. Saliendo...\n");
    return NULL;
}

int main(){
    signal(SIGUSR1, positivo_handler);
    signal(SIGUSR2, negativo_handler);
    signal(SIGALRM, coordinador_handler);
    
    if (pthread_create(&coordinador, NULL, coordinador_f, NULL) != 0) {
        perror("Error al crear el coordinador");
        return 1;
    }
    
    pthread_join(coordinador, NULL);
    printf("Programa principal finalizado ordenadamente.\n");
    return 0;
}
