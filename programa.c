#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


/* Creamos una tuberia, un proceso hijo, se queda a la espera mientras el padre lee 2 nùmeros. 
Leemos el primer número, se lo enviamos al hijo, Leemos el segundo número y se lo enviamos al hijo. 
Los suma y los muestra por pantalla Papà siempre sobrevive al hijo*/

int main()
{
    int tuberia[2];
    int numero1;
    int numero2;

    pid_t hijo;

    pipe(tuberia);

    hijo = fork();

    if (hijo > 0) {
        close(tuberia[0]);
        printf("Introduce el primer nùmero: ");
        write(tuberia[1], &numero1, sizeof(numero1));

        printf("Introduce el segundo nùmero: ");
        write(tuberia[1], &numero2, sizeof(numero2));

        close(tuberia[1]);
        wait(NULL);
        puts("El hijo ha finalizado, termino mi ejecución \n");

    } else if (hijo == 0)
    {
        close(tuberia[1]);
        read(tuberia[0], &numero1, sizeof(numero1));
        printf("HIJO ha recibido \n");
        write(1, &numero1, sizeof(numero1));

        read(tuberia[0], &numero2, sizeof(numero2));
        printf("HIJO ha recibido \n");
        write(1, &numero2, sizeof(numero2));

        int suma = numero1 + numero2;
        printf("La suma es: %d\n", suma);

        close(tuberia[0]);
        exit(0);
    }

    return 0;
}