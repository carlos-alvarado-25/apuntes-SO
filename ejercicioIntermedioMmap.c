/* Implementa un programa en C que calcule, de forma colaborativa entre un proceso padre y un proceso hijo,
la suma de todos los elementos de un array de eneteros. El array tendrá N elementos (con N par). 
Cada proceso calculará la suma de su mitad del array y escribirá ambos resultados y mostrará la suma total.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define N 10

int main(void)
{
    int datos[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int *sumas = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_t hijo;
    
    if (sumas == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    hijo = fork();

    if (hijo < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    int mitad = N / 2;

    if (hijo == 0) {

        for(int i = 0; i < mitad; i++){
            sumas[1] += datos[i];
        }

        printf("Hijo: La suma de mi mitad es %d\n", sumas[1]);
        munmap(sumas, sizeof(int) * 2);
        exit(0);

    } else if (hijo > 0) {
        for(int i = mitad; i < N; i++){
            sumas[0] += datos[i];
        }

        printf("Padre: La suma de mi mitad es %d\n", sumas[0]);

        wait(NULL);

        int sumaTotal = sumas[0] + sumas[1];
        printf("La suma total es: %d \n", sumaTotal);
        munmap(sumas, sizeof(int) * 2);
    }

    return 0;
}