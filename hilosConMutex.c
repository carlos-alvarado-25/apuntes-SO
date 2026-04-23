#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define NUM_HILOS 5
#define INCREMENTOS 100000
#define VALOR_ESPERADO (NUM_HILOS * INCREMENTOS) // 500.000

long contador = 0;
pthread_mutex_t mutex;

void* hiloSinMutex(void* arg)
{
    for (int i = 0; i < INCREMENTOS; i++){
        contador++;
    }

    return NULL;
}

void* hiloConMutex(void* arg)
{
    for (int i = 0; i < INCREMENTOS; i++){
        pthread_mutex_lock(&mutex);
        contador++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

long ejecutar_prueba(void *(*funcion_hilo)(void*), const char *etiqueta)
{
    pthread_t hilos[NUM_HILOS];
    contador = 0;
    printf("\n[%s]\n", etiqueta);

    for (int i = 0; i < NUM_HILOS; i++){
        pthread_create(&hilos[i], NULL, funcion_hilo, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++){
        pthread_join(hilos[i], NULL);
    }

    printf(" Resultado obtenido: %ld \n", contador);
    printf(" Resultado esperado: %d \n", VALOR_ESPERADO);
    printf(" Estado            : %s \n", contador==VALOR_ESPERADO ? "Correcto" : "Incorrecto (race condition)");
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    // Nos vamos a la secciòn critica
    ejecutar_prueba(hiloSinMutex, "SIN MUTEX");
    ejecutar_prueba(hiloConMutex, "CON MUTEX");

    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}