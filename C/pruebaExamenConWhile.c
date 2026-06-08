#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

static int *variableCompartida;

int main()
{
    pid_t leer, escribir;
    variableCompartida = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    variableCompartida[1] = 0;

    leer = fork();

    if (leer == 0) {

        int ingreso;

        scanf("%d", &ingreso);

        variableCompartida[0] = ingreso;
        variableCompartida[1] = 1;

        while (variableCompartida[1] != 2) {
            usleep(2);
        }

        exit(0);
    }

    escribir = fork();

    if (escribir == 0) {

        while (variableCompartida[1] == 0) {
            usleep(2);
        }

        printf("La variable es %d \n", variableCompartida[0]);

        printf("Pulsa una tecla para continuar...\n");
        while (getchar() != '\n');
        getchar();

        variableCompartida[1] = 2;

        exit(0);

    }

    wait(NULL);
    printf("Hijo leer totalmente muerto %d \n", getpid());
    wait(NULL);
    printf("Hijo escribir totalmente muerto %d \n", getpid());

    printf("Padre: Ambos hijos terminaron");

    munmap(variableCompartida, sizeof(int) *2);

    return 0;

}
