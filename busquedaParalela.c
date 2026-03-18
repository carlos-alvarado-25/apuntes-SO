#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 10

int main() {
    // Array de prueba a buscar (hemos añadido dos números para completar los 10)
    int numeros[SIZE] = {2, 5, 12, 23, 42, 50, 64, 75, 90, 100}; 
    int objetivo = 50; // Puedes cambiar este número por uno que no esté (ej: 99)

    pid_t pid1, pid2;
    int status1, status2;

    // Crear el primer hijo
    pid1 = fork();

    if (pid1 < 0) {
        perror("Error al crear el Hijo 1");
        exit(1);
    } else if (pid1 == 0) {
        // --- CÓDIGO DEL HIJO 1 ---
        // Busca en la primera mitad (índices 0 a 4)
        for (int i = 0; i < SIZE / 2; i++) {
            if (numeros[i] == objetivo) {
                exit(1); // Éxito: lo encontró
            }
        }
        exit(0); // Fracaso: no lo encontró
    }

    // Crear el segundo hijo (esto lo ejecuta solo el padre)
    pid2 = fork();

    if (pid2 < 0) {
        perror("Error al crear el Hijo 2");
        exit(1);
    } else if (pid2 == 0) {
        // --- CÓDIGO DEL HIJO 2 ---
        // Busca en la segunda mitad (índices 5 a 9)
        for (int i = SIZE / 2; i < SIZE; i++) {
            if (numeros[i] == objetivo) {
                exit(1); // Éxito: lo encontró
            }
        }
        exit(0); // Fracaso: no lo encontró
    }

    // --- CÓDIGO DEL PADRE ---
    // Esperar a que terminen ambos hijos específicamente
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    printf("Buscando el número: %d\n", objetivo);

    // Inspeccionar el estado del Hijo 1
    if (WIFEXITED(status1)) {
        int resultado1 = WEXITSTATUS(status1);
        if (resultado1 == 1) {
            printf("Hijo 1: Encontró el objetivo en la primera mitad (Estado: %d).\n", resultado1);
        } else {
            printf("Hijo 1: No encontró el objetivo (Estado: %d).\n", resultado1);
        }
    }

    // Inspeccionar el estado del Hijo 2
    if (WIFEXITED(status2)) {
        int resultado2 = WEXITSTATUS(status2);
        if (resultado2 == 1) {
            printf("Hijo 2: Encontró el objetivo en la segunda mitad (Estado: %d).\n", resultado2);
        } else {
            printf("Hijo 2: No encontró el objetivo (Estado: %d).\n", resultado2);
        }
    }

    return 0;
}