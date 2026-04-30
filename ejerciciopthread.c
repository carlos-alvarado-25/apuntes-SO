/*Implementar en C un programa con dos hilos que cooperan mediante señales y un mutex.
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
El número de iteraciones configurable mediante #define */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define N 5 
#define RETARDO_US 50000 /* 50 ms: tiempo que espera el productor para que el consumidor instale signal()*/

static int dato = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_t hilo_consumidor;

static volatile int hay_dato = 0;
static volatile int fin = 0;

// Manejador para SIGUSR1: Leer el dato
void manejador_sigusr1(int sig) {
    pthread_mutex_lock(&mutex);
    suma_total += dato;
    printf("[Consumidor] Recibido SIGUSR1. Dato: %d | Suma acumulada: %d\n", dato, suma_total);
    pthread_mutex_unlock(&mutex);
}

// Manejador para SIGUSR2: Finalizar
void manejador_sigusr2(int sig) {
    terminado = 1;
    printf("[Consumidor] Recibido SIGUSR2. Finalizando...\n");
}

// Función del hilo Consumidor
void* funcion_consumidor(void* arg) {
    // Registrar manejadores de señales
    signal(SIGUSR1, manejador_sigusr1);
    signal(SIGUSR2, manejador_sigusr2);

    printf("[Consumidor] Esperando datos...\n");

    while (!terminado) {
        pause(); // Bloqueado hasta recibir cualquier señal
    }

    printf("[Consumidor] SUMA TOTAL FINAL: %d\n", suma_total);
    return NULL;
}

// Función del hilo Productor
void* funcion_productor(void* arg) {
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        // Simular un pequeño retardo para asegurar que el consumidor esté en pause()
        usleep(100000); 

        pthread_mutex_lock(&mutex);
        dato = (rand() % 100) + 1; // Generar número entre 1 y 100
        printf("[Productor] Generado dato: %d\n", dato);
        pthread_mutex_unlock(&mutex);

        // Notificar al consumidor
        pthread_kill(hilo_consumidor, SIGUSR1);
    }

    // Indicar fin de producción
    usleep(100000);
    pthread_kill(hilo_consumidor, SIGUSR2);

    return NULL;
}

int main() {
    pthread_t hilo_productor;

    // Crear hilos
    if (pthread_create(&hilo_consumidor, NULL, funcion_consumidor, NULL) != 0) {
        perror("Error al crear consumidor");
        return 1;
    }

    // Damos un momento al consumidor para que registre sus manejadores
    sleep(1);

    if (pthread_create(&hilo_productor, NULL, funcion_productor, NULL) != 0) {
        perror("Error al crear productor");
        return 1;
    }

    // Esperar a que ambos terminen
    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);

    // Limpiar recursos
    pthread_mutex_destroy(&mutex);

    printf("Programa finalizado con éxito.\n");
    return 0;
}