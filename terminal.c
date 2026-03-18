#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int opcion;
    pid_t hijo;

    while (1) {
        // Mostrar el menú
        printf("\n--- Menú de Comandos ---\n");
        printf("1. Ejecutar 'ls' (Listar directorio)\n");
        printf("2. Ejecutar 'ps' (Listar procesos)\n");
        printf("3. Ejecutar 'pwd' (Mostrar ruta actual)\n");
        printf("4. Ejecutar 'date' (Mostrar fecha y hora)\n");
        printf("-1. Salir\n");
        printf("Introduce un número: ");
        
        // Leer la opción elegida
        if (scanf("%d", &opcion) != 1) {
            // Limpiar el buffer si el usuario mete una letra por error
            while(getchar() != '\n'); 
            printf("Por favor, introduce un número válido.\n");
            continue;
        }

        // Condición de salida
        if (opcion == -1) {
            printf("Saliendo del programa. ¡Hasta luego!\n");
            break; // Rompe el bucle while y termina el programa
        }

        // Validar que la opción esté en el rango correcto
        if (opcion < 1 || opcion > 4) {
            printf("Opción no válida. Inténtalo de nuevo.\n");
            continue; // Vuelve al inicio del bucle
        }

        // Crear el proceso hijo
        hijo = fork();

        if (hijo < 0) {
            perror("Error al crear el proceso hijo");
        } 
        else if (hijo == 0) {
            // --- CÓDIGO DEL HIJO ---
            // El hijo ejecuta el comando correspondiente
            switch (opcion) {
                case 1:
                    execlp("ls", "ls", NULL);
                    break;
                case 2:
                    execlp("ps", "ps", NULL);
                    break;
                case 3:
                    execlp("pwd", "pwd", NULL);
                    break;
                case 4:
                    execlp("date", "date", NULL);
                    break;
            }
            
            // Si exec falla, el código llega hasta aquí
            perror("Error al ejecutar el comando");
            exit(1); // El hijo muere con error si el comando no existe
        } 
        else {
            // --- CÓDIGO DEL PADRE ---
            // El padre espera a que el hijo (el comando) termine antes de volver a mostrar el menú
            wait(NULL);
            printf("\n[El proceso hijo terminó de ejecutar el comando]\n");
        }
    }
    
    return 0;
}