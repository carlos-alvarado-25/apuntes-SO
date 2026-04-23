#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;
int ultimo_fibonacci = 0;

void* hilo_fibonacci(void* arg) {
    int n = *((int*)arg);
    
    pthread_mutex_lock(&mutex);
    
    int a = 0, b = 1;
    printf("Sucesión: ");
    for (int i = 0; i <= n; i++) {
        if (i <= 1) ultimo_fibonacci = i; // Calculo la sucesión de fibonacci
        else {
            ultimo_fibonacci = a + b;
            a = b;
            b = ultimo_fibonacci;
        }
        printf("%d ", ultimo_fibonacci); // Imprimiendo el último 
    }

    pthread_mutex_unlock(&mutex); // SALIDA DE SECCIÓN CRÍTICA
    return NULL;
}

void* hilo_primo(void* arg) {
   
    pthread_mutex_lock(&mutex); // ENTRADA A SECCIÓN CRÍTICA

    int n = ultimo_fibonacci;
    int es_primo = (n > 1);
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) { es_primo = 0; break; }
    }

    printf("\n");
    printf("Resultado: %d %s.\n", n, es_primo ? "ES primo" : "NO es primo");
    
    pthread_mutex_unlock(&mutex); // SALIDA DE SECCIÓN CRÍTICA
    return NULL;
}

int main() {
    int numero;
    pthread_t h1, h2;
    pthread_mutex_init(&mutex, NULL); // Inicializo el mutex

    printf("Introduce un número: "); // Scaneo el número
    scanf("%d", &numero);

    pthread_create(&h1, NULL, hilo_fibonacci, &numero); // Creo ambos hilos en simultáneo, pasandole el número escaneado al de fibonacci
    pthread_create(&h2, NULL, hilo_primo, NULL);

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}