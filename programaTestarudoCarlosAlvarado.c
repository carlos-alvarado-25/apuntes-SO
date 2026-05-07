#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// El contador que el manejador irá aumentando
static volatile sig_atomic_t contador = 0;
static volatile sig_atomic_t bandera_presionado = 0;

void manejador_sigint(int sig) {
    contador++;          // Sumamos el intento
    bandera_presionado = 1; // Avisamos que "pasó algo"
    
    const char *msg = "\n ¡Ouch! eso duele. ";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {
    signal(SIGINT, manejador_sigint);

    int intentos_totales = 3;

    printf("--- Programa testarudo iniciado... (PID: %d) ---\n", getpid());
    printf("Tienes %d intentos...\n\n", intentos_totales);

    while (contador < intentos_totales) {
        
        if (bandera_presionado) {
            // El while se encarga de la lógica y los prints, usamos la bandera para avisar que el contador sumo uno (o que se presionó el Ctrl + C)
            int restantes = intentos_totales - contador;
            
            if (restantes > 0) {
                printf("Te quedan %d intentos.\n", restantes);
            }
            
            bandera_presionado = 0; // Reset de la bandera
        }	

        printf("Trabajando arduamente... (PID: %d)\n", getpid());
        sleep(2);
    }
    
    printf("\nEstá bien, me rindo. ¡Adiós!\n");
    exit(0);

    return 0;
}

