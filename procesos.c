#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso");
        exit(1);
    }

    if (pid > 0) {
        // --- Proceso Padre ---
        printf("[PADRE] Mi PID es %d. Voy a morir en 5 segundos...\n", getpid());
        sleep(5);
        printf("[PADRE] Finalizando...\n");
        exit(0);
    } else {
        // --- Proceso Hijo ---
        printf("[HIJO]  Inicio. Mi PID es %d y mi padre (PPID) es %d\n", getpid(), getppid());
        
        // Esperamos a que el padre muera
        printf("[HIJO]  Esperando a que mi padre muera...\n");
        sleep(10);
        
        // Verificamos quién es el nuevo padre
        printf("\n[HIJO]  Ahora mi PID sigue siendo %d, pero mi nuevo PPID es %d\n", getpid(), getppid());
        printf("[HIJO]  Finalizando.\n");
    }

    return 0;
}
