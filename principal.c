#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t hijo;
    
    hijo = fork();
    
    if (hijo > 0) 
    {
        wait(NULL);
        puts("El proceso suicida hijo ha acabado de ejecutar el esclavo, e largo");
    }
    else if (hijo == 0) {
        execlp("./esclavo", "esclavo", NULL); // En este punto, principal.c se muere, y pasa a sustituirse por esclavo.c
        puts("Esto en teorìa no sale ya por pantalla");
    }
    
    return 0;
}