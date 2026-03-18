#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int pipe[2];
    
    pid_t hijo;
    char mensaje[] = "Hola proceso hijo! Misión Cumplida!";
    char buffer[100];
    
    pipe(pipe);
    
    hijo=fork();
    
    if (hijo > 0)
    {
        close(pipe[0]); // Siempre se cierra el lado de la tuberìa que no se va a usar, (Escribiremos en 1)
        printf("[Padre] escribiendo en el pipe...\n");
        write(pipe[1], mensaje, sizeof(mensaje)); //Escribe en el lado 1 de la tuberìa
        close(pipe[1]);
        wait(NULL);
        puts("El hijo ha finalizado, termino mi ejecución \n");
    }
    else if (hijo == 0)
    {
        close(pipe[1]);
        read(pipe[0], buffer, sizeof(buffer));
        printf("[Hijo] ha recibido este mensaje de papá %s\n", buffer);
        close(pipe[0]);
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}