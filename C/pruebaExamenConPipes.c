#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

static int *variableCompartida;

int main()
{
    int tuberiaIda[2];
    int tuberiaVuelta[2];
    pid_t leer, escribir;
    variableCompartida = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (pipe(tuberiaIda) == -1 || pipe(tuberiaVuelta) == -1) {
        perror("pipes failed");
        exit(EXIT_FAILURE);
    }

    leer = fork();

    if (leer == 0) {

        close(tuberiaIda[0]);
        close(tuberiaVuelta[1]);

        int ingreso;

        scanf("%d", &ingreso);

        *variableCompartida = ingreso;

        char aviso = '1';

        write(tuberiaIda[1], &aviso, 1); // Avisando mediante write que ya escribí el valor en la variable compartida

        read(tuberiaVuelta[0], &aviso, 1); // Bloquear en tubería 2 esperando el write del otro lado para morir

        printf("Muriendo hijo %d \n", getpid());

        exit(0);
    }

    escribir = fork();

    if (escribir == 0) {

        close(tuberiaIda[1]);
        close(tuberiaVuelta[0]);

        char aviso;

        read(tuberiaIda[0], &aviso, 1); // Leer el aviso del proceso leer para iniciar a imprimir, al tener ya disponible el valor de la variable compartida

        printf("La variable es %d \n", *variableCompartida);

        printf("Pulsa una tecla para continuar...\n");
        while (getchar() != '\n');
        getchar();

        write(tuberiaVuelta[1], &aviso, 1); // Desbloquea al proceso anterior escribiendo para pasar a ejecutar al mismo tiempo el exit(0)

        printf("Muriendo hijo %d \n", getpid());

        exit(0);

    }

    close(tuberiaIda[0]);
    close(tuberiaVuelta[1]);
    close(tuberiaIda[1]);
    close(tuberiaVuelta[0]);

    wait(NULL);
    wait(NULL);

    printf("Padre: Ambos hijos terminaron");

    munmap(variableCompartida, sizeof(int));

}
