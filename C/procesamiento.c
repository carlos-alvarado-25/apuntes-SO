#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define ROWS 3
#define COLS 3
#define CICLOS 3

pthread_t sensor, filtro, alerta;
static volatile int mapaTermico[ROWS][COLS];

static volatile sig_atomic_t activar_sensor = 0;
static volatile sig_atomic_t activar_filtro = 0;
static volatile sig_atomic_t activar_alerta = 0;
static volatile sig_atomic_t apagar = 0;

void manejadorSensor(int sig) { activar_sensor = 1; }
void manejadorFiltro(int sig) { activar_filtro = 1; }
void manejadorAlerta(int sig) { activar_alerta = 1; }

void* sensor_f(void* arg){
    for (int i = 0; i < CICLOS; i++){
        while(!activar_sensor){
            pause();
        }
        activar_sensor = 0;

        for (int k = 0; k < ROWS; k++){
            for (int j = 0; j < COLS; j++){
                mapaTermico[k][j] = (rand() % 100) + 1;
            }
        }

        printf("\n--- INICIANDO CICLO %d ---\n", i + 1);
        printf("[SENSOR]: Ya capturé el mapa, te toca limpiarlo\n");

        pthread_kill(filtro, SIGUSR1);
    }
    return NULL;
}

void* filtro_f(void* arg){
    while(1){
        while(!activar_filtro && !apagar){
            pause();
        }

        if (apagar){
            printf("[FILTRO]: Procesando apagado..\n");
            pthread_kill(alerta, SIGUSR2);
            break;
        }

        if (activar_filtro){
            for (int k = 0; k < ROWS; k++){
                for (int j = 0; j < COLS; j++){
                    if (mapaTermico[k][j] > 80){
                        printf("[FILTRO]: ERROR SIMULADO EN %d. Restando 10...\n", mapaTermico[k][j]);
                        mapaTermico[k][j] -= 10;
                    }
                }
            }

            activar_filtro = 0;
            printf("[FILTRO]: MAPA LIMPIO, ANALIZALO...\n");

            pthread_kill(alerta, SIGUSR2);
        }
    }
    return NULL;
}

void* alerta_f(void* arg){
    while (1){
        while (!activar_alerta && !apagar){
            pause();
        }

        if (apagar){
            printf("[ALERTA]: Procesando apagado..\n");
            break;
        }

        if (activar_alerta){
            printf("[ALERTA]: Mapa térmico actual: \n");
            for (int k = 0; k < ROWS; k++){
                for (int j = 0; j < COLS; j++){
                    if (mapaTermico[k][j] > 70){
                        printf("[🔥%d] ", mapaTermico[k][j]);
                    } else {
                        printf("[🌲%d] ", mapaTermico[k][j]);
                    }
                }
                printf("\n");
            }

            activar_alerta = 0;
            printf("[ALERTA]: MAPA ANALIZADO, COMIENZA SIGUIENTE CICLO...\n");

            pthread_kill(sensor, SIGALRM);
        }
    }
    return NULL;
}

int main(){
    srand((unsigned)time(NULL));

    signal(SIGALRM, manejadorSensor);
    signal(SIGUSR1, manejadorFiltro);
    signal(SIGUSR2, manejadorAlerta);

    pthread_create(&sensor, NULL, sensor_f, NULL);
    pthread_create(&filtro, NULL, filtro_f, NULL);
    pthread_create(&alerta, NULL, alerta_f, NULL);

    pthread_kill(sensor, SIGALRM);

    pthread_join(sensor, NULL);

    printf("\n[Main]: Ciclos completados. Iniciando apagado en cascada...\n");
    apagar = 1;

    pthread_kill(filtro, SIGUSR1);

    pthread_join(filtro, NULL);
    pthread_join(alerta, NULL);

    printf("[Main]: Programa finalizado con éxito.\n");
    return 0;
}
