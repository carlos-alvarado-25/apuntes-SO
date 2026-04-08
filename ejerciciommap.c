/* Papá crea un hijo, y al menos una variable compartida. El padre una vez que ya ha creado al hijo, el padre leé un número entero. Cuando el padre leé el número 
el hijo va a calcular su factorial. y el padre una vez que el hijo ha terminado de calcular el factorial, saca el valor de ese factorial por pantalla.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
    pid_t hijo;
    
    int *variable_compartida = mmap(NULL, sizeof(*variable_compartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *variable_compartida = -1;

    hijo = fork();

    if (hijo > 0) {

        int entero;

        puts("Padre: Inserta un entero");
        scanf("%d", &*variable_compartida);
        printf("El entero leido es %d\n", *variable_compartida);
        wait(NULL);

        printf("El factorial calculado por el hijo es: %d\n", *variable_compartida);

    } else if (hijo == 0) {

        while(*variable_compartida == -1) {
            
        }

        int numero_leido = *variable_compartida;
        int factorial = 1;

        for (int i = 1; i <= numero_leido; i++) {
            factorial *= i;
        }

        *variable_compartida = factorial;
        
        exit(0);
    }

    munmap(variable_compartida, sizeof(int));
    return 0;
}