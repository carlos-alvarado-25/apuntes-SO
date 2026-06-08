/*  Implementar en C un programa con dos hilos que cooperan mediante señales y un mutex.
Hilo Productor (productor);

Genera N nùmeros enteros aleatorios (entre 1 y 100), uno por iteración.
Antes de escribir cada número, adquiere el mutex para escribir en la variable compartida dato.
Una vez descrito el dato y liberado el mutex, notifica al consumidor enviandole SIGUSR1 con pthread_kill,
Tras enviar todos los datos, envía SIGUR2 al  consumidor para indicarle que ha terminado.

Hilo Consumidor (consumidor);

Registrar manejadores para SIGUSR1 y SIGUSR2.
En cada iteración llama a pause() para quedarse bloqueado hasta recibir una señal.
Al recibir SIGUSR1, adquiere el mutex, lee dato, lo acumula en una suma y lo imprime.
Al recibir SIGUSR2, imprime la suma total y termina.

Requisitos concretos:

- Usar pthread_mutex_lock / pthread_mutex_unlock para proteger dato.
Usar pause() en el consumidor como mecanismo de espera.
El número de iteraciones configurable mediante #define

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define ITERACIONES 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t productor, consumidor;
static volatile int dato = 0;
static volatile int suma = 0;
static volatile sig_atomic_t productora = 0;
static volatile sig_atomic_t consumidora = 0;

void manejadorRebote(int sig) {

}

void manejadorProductor(int sig) {
    productora = 1;
}

void manejadorConsumidor(int sig) {
    consumidora = 1;
}

void* consumidor_f(void* arg){

    while(1) {
        pause();

        if (productora) {
            pthread_mutex_lock(&mutex);
            suma += dato;
            pthread_mutex_unlock(&mutex);

            printf("SUMA PARCIAL: %d\n", suma);

            productora = 0;

            pthread_kill(productor, SIGALRM);
        }

        if (consumidora) {
            printf("SUMA TOTAL: %d", suma);
            consumidora = 0;
            break;
        }
    }

    return NULL;

}

void* productor_f(void* arg){

    for (int i = 0; i < ITERACIONES; i++){
        int valorGenerado = (rand() % 100) + 1;

        printf("VALOR GENERADO: %d\n", valorGenerado);

        pthread_mutex_lock(&mutex);
        dato = valorGenerado;
        pthread_mutex_unlock(&mutex);

        pthread_kill(consumidor, SIGUSR1);
        pause();
    }

    pthread_kill(consumidor, SIGUSR2);

    return NULL;
}

int main() {
    srand((unsigned)time(NULL));

    signal(SIGUSR1, manejadorProductor);
    signal(SIGUSR2, manejadorConsumidor);
    signal(SIGALRM, manejadorRebote);

    pthread_create(&consumidor, NULL, consumidor_f, NULL);
    pthread_create(&productor, NULL, productor_f, NULL);

    pthread_join(productor, NULL);
    pthread_join(consumidor, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
