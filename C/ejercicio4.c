#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define CICLOS 3

pthread_t ventilador, bomba, principal;
static volatile sig_atomic_t ventilador_listo = 0;
static volatile sig_atomic_t main_listo = 0;
static volatile sig_atomic_t bomba_lista = 0;
static volatile sig_atomic_t terminar = 0;

void manejadorMain(int sig){
    main_listo = 1;
}

void manejadorVentilador(int sig){
    ventilador_listo = 1;
}

void manejadorBomba(int sig){
    bomba_lista = 1;
}

void* ventilador_f(void* arg){
    pthread_kill(principal, SIGALRM);

    while(!terminar){
        pause();

        if (terminar) break;

        if (ventilador_listo){
            printf("[Ventilador]: Extrayendo aire caliente...\n");
            ventilador_listo = 0;
            pthread_kill(bomba, SIGUSR2);
        }
    }

    return NULL;
}

void* bomba_f(void* arg){
    pthread_kill(principal, SIGALRM);

    while (!terminar){
        pause();

        if (terminar) break;

        if (bomba_lista){
            printf("[Bomba] Inyectando agua fria...\n");
            bomba_lista = 0;
            pthread_kill(principal, SIGALRM);
        }
    }

    return NULL;
}

int main(){

    principal = pthread_self();

    signal(SIGUSR1, manejadorVentilador);
    signal(SIGUSR2, manejadorBomba);
    signal(SIGALRM, manejadorMain);

    pthread_create(&ventilador, NULL, ventilador_f, NULL);
    while(!main_listo){
        pause();
    }
    main_listo = 0;
    pthread_create(&bomba, NULL, bomba_f, NULL);
    while(!main_listo){
        pause();
    }
    main_listo = 0;

    printf("[MAIN] Iniciando el ciclo de enfriamiento 1...\n");
    for (int i = 0; i < CICLOS; i++){
        pthread_kill(ventilador, SIGUSR1);

        while(!main_listo){
            pause();
        }
        main_listo = 0;

        printf("[MAIN] Ciclo %d completado.\n", i + 1);
    }

    terminar = 1;

    pthread_kill(ventilador, SIGUSR1);
    pthread_kill(bomba, SIGUSR2);

    pthread_join(ventilador, NULL);
    pthread_join(bomba, NULL);

    return 0;

}
