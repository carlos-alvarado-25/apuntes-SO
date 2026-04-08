/* Comunicación entre procesos con memoria compartida anónima. EL hijo calcula los 10 primeros números de Fibonacci y los escribe en la memoria compartida.
EL padre los lee tras esperar al hijo y los saca por pantalla*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define N 10

static int variable_compartida[];


int main()
{
    pid_t hijo;

    variable_compartida = mmap(NULL, sizeof(*variable_compartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    hijo = fork();

    if (hijo == 0)
    {
        variable_compartida[0] = 0;
        variable_compartida[1] = 1;

        for (int i = 2; i < N; i++) {
            variable_compartida[i] = variable_compartida[i - 1] + variable_compartida[i - 2];
        }

        exit(0);

    } else if (hijo > 0) {
        wait(NULL);

        printf("Los 10 primeros números de Fibonacci son: \n");
        for (int i = 0; i < N; i++) {
            printf("%d ", variable_compartida[i]);
        }
        printf("\n");
    }
}