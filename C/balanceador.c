#include <pthread.h>
#include <signal.h>
#include <unistd.h>

pthread_t router, servidorA, servidorB, cliente;
static volatile sig_atomic_t clientear = 0;
static volatile sig_atomic_t routear = 0;
static volatile sig_atomic_t servirA = 0;
static volatile sig_atomic_t servirB = 0;
static volatile int valorArchivo;

void manejadorRouter(int sig){
    routear = 1;
}

void* cliente_f(void* arg){

    for (int i = 0; i < 4; i++){
        valorArchivo = 1;
        pthread_kill(router, SIGUSR1);
        while(!clientear){
            pause();
        }
        clientear = 0;
    }

    return NULL;

}

void* router_f(void* arg){

    while(1){
        pause();

        if (routear){
            if (valorArchivo % 2 == 0){
                pthread_kill(servidorB, SIGUSR2);
                pause();
            } else {

            }
        }
    }

}

void* servidorA_f(void* arg){

}

void* servidorB_f(void* arg){

}

int main(){

    signal(SIGUSR1, manejadorRouter);

    pthread_create(&cliente, NULL, cliente_f, NULL);

    pthread_create(&router, NULL, router_f, NULL);
    pthread_create(&servidorA, NULL, servidorA_f, NULL);
    pthread_create(&servidorB, NULL, servidorB_f, NULL);

    pthread_join(cliente, NULL);
    pthread_join(router, NULL);
    pthread_join(servidorA, NULL);
    pthread_join(servidorB, NULL);

    return 0;
}
