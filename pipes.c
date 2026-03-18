#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int tuberia[2]; // Cambiamos el nombre para que no choque con la función pipe()
    
    pid_t hijo;
    char mensaje[] = "Hola proceso hijo! Misión Cumplida!";
    char buffer[100];
    
    // Ahora sí estamos llamando a la función pipe() y pasándole nuestro array
    pipe(tuberia); 
    
    hijo = fork();
    
    if (hijo > 0)
    {
        // Siempre se cierra el lado de la tubería que no se va a usar
        close(tuberia[0]); 
        printf("[Padre] escribiendo en el pipe...\n");
        sleep(2);
        // Escribe en el lado 1 de la tubería
        write(tuberia[1], mensaje, sizeof(mensaje)); 
        close(tuberia[1]);
        
        wait(NULL);
        puts("El hijo ha finalizado, termino mi ejecución \n");
    }
    else if (hijo == 0)
    {
        close(tuberia[1]);
        read(tuberia[0], buffer, sizeof(buffer));
        printf("[Hijo] ha recibido este mensaje de papá: %s\n", buffer);
        close(tuberia[0]);
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}