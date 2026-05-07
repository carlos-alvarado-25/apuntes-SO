#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// El contador que el manejador irá aumentando
static volatile sig_atomic_t monedas = 0;
static volatile sig_atomic_t moneda_recibida = 0;

void manejador_moneda(int sig) {
    monedas++;          // Sumamos el intento
    moneda_recibida = 1; // Avisamos que "pasó algo"
    
    const char *msg = "\n Clinc! Detectando moneda...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {
    signal(SIGUSR1, manejador_moneda);

    int objetivo = 3;

    printf("=== MÁQUINA DE CAFÉ VIRTUAL ===\n");
    printf("===============================\n\n");


	// Si el número de monedas ingresadas es menor al objetivo, que me inserte la moneda
    while (monedas < objetivo) {
        
        if (moneda_recibida) {
            printf(">>> Moneda recibida. Llevas %d/%d monedas.\n", monedas, objetivo);
            moneda_recibida = 0; // Con la bandera, al momento de sumar una moneda que imprima, y devuelva la bandera a 0
        }

        printf("Máquina encendida. Esperando monedas... (Mi PID es %d)\n", getpid());
        sleep(3);
    }
    
    printf("\n[CAFÉ] ¡Objetivo alcanzado! Sirviendo café calientito... ☕\n");
    printf("Apagando máquina. ¡Disfrute su bebida!\n");
    
    exit(0);

    return 0;
}


