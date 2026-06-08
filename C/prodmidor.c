#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define ITERACIONES 5
#define RETARDO_US 50000

pthread_t productor, consumidor;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int dato = 0;
static volatile int fin = 0;
static volatile int hayDato = 0;

void manejador_sigusr1(int sig){
    hayDato = 1;
}

void manejador_sigusr2(int sig){
    fin = 1;
}

void* hilo_consumidor(void* arg){
    signal(SIGUSR1, manejador_sigusr1);
    signal(SIGUSR2, manejador_sigusr2);

    int suma = 0;

    while (!fin) {
        hayDato = 0;
        pause();

        if (hayDato) {
            pthread_mutex_lock(&mutex);
            int valor = dato;
            pthread_mutex_unlock(&mutex);

            suma += valor;
            printf("[Consumidor] Recibido: %3d | Suma parcial: %d\n", valor, suma);
        }
    }

    printf("[Consumidor]: Fin. Suma total = %d\n", suma);
    return NULL;
}

void* hilo_productor(void* arg){
    srand((unsigned)time(NULL));

    for (int i = 0; i < ITERACIONES; i++) {
        int valor = (rand() % 100) + 1;

        pthread_mutex_lock(&mutex);
        dato = valor;
        pthread_mutex_unlock(&mutex);

        printf("[Producto] Enviado: %3d\n", valor);
        pthread_kill(consumidor, SIGUSR1);

        sleep(1);
    }

    pthread_kill(consumidor, SIGUSR2);
    printf("[Productor] Fin de producción \n");
    return NULL;
}

int main() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Evita que las señales rompan otras funciones del SO

    // 2. Registramos SIGUSR1
    sa.sa_handler = manejador_sigusr1;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error en SIGUSR1");
        exit(EXIT_FAILURE);
    }

    // 3. Registramos SIGUSR2 (reutilizando la misma estructura 'sa')
    sa.sa_handler = manejador_sigusr2;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Error en SIGUSR2");
        exit(EXIT_FAILURE);
    }

    // 4. AHORA SÍ: Creamos los hilos. Nacerán 100% protegidos.
    pthread_create(&consumidor, NULL, &hilo_consumidor, NULL);
    pthread_create(&productor, NULL, &hilo_productor, NULL);

    pthread_join(productor, NULL);
    pthread_join(consumidor, NULL);

    pthread_mutex_destroy(&mutex);
    printf("Programa terminado correctamente..\n");
    return 0;
}
