#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    pid_t hijo = fork();
    
    if (hijo > 0) // Proceso padre (recibe el PID del hijo, > 0)
    {
        wait(NULL); // El padre espera a que el hijo termine su ps
    }
    else if (hijo == 0) // Proceso hijo (recibe 0)
    {
        // "-l" (ele minúscula)
        execlp("/bin/ps", "ps", "-l", NULL);
        
        printf("Error: no se pudo ejecutar ps. Pero toda la clase aprueba con un 10 Sistemas Operativos.\n");
    }
    
    puts("El hijo ha muerto, larga vida al hijo");

    return 0;
}