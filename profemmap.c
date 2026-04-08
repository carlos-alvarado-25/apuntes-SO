/* Papá crea un hijo, y al menos una variable compartida. El padre una vez que ya ha creado al hijo, el padre leé un número entero. Cuando el padre leé el número 
el hijo va a calcular su factorial. y el padre una vez que el hijo ha terminado de calcular el factorial, saca el valor de ese factorial por pantalla.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>

static long unsigned int *variable_compartida;

int main()
{
    pid_t hijo;
    
    variable_compartida = mmap(NULL, sizeof(*variable_compartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *variable_compartida = 0;

    hijo = fork();

    if (hijo > 0) { // Padre

        puts("Padre: Inserta un número para calcular su factorial");
        scanf("%lu", &*variable_compartida);
        wait(NULL);

        printf("EL valor del factorial es: %lu \n", *variable_compartida);

    } else if (hijo == 0) { // Hijo

        long unsigned int factorial = 1, i;
        while(*variable_compartida == 0)
        {
            usleep(2);
        }

        int numero_leido = *variable_compartida;

        for (i = 1; i <= numero_leido; i++) {
            factorial = factorial * i;
        }

        *variable_compartida = factorial;
        
        exit(0);
    }
    return 0;
}