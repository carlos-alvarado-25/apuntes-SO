#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> // MUY IMPORTANTE: Necesario para usar strcmp() y strcat()

#define VERSION 1.00 // Definimos la versión para el printf de tu profesor

int main(void)
{
    char comando[20];
    pid_t hijo;

    // Saludo inicial calcado al de la foto
    printf("Bienvenidos a JL-Shell %0.2f. Introduzca un comando para ejecutar:\n", VERSION);
    scanf("%s", comando);

    // Bucle: se repite mientras el usuario NO escriba "salir"
    while (strcmp(comando, "salir") != 0)
    {
        hijo = fork();
        
        if (hijo == 0)
        {
            // --- CÓDIGO DEL HIJO ---
            char cadena[50] = "/bin/";
            strcat(cadena, comando); // Pega el comando al final de /bin/
            
            // puts(cadena); // Tu profe lo tiene comentado, sirve para depurar y ver "/bin/ls"
            
            execl(cadena, comando, NULL); // Ejecuta usando la ruta completa
            
            // Si el comando no existe en /bin/ (ej: escribes una tontería), execl falla y llega aquí
            printf("Comando no encontrado.\n");
            return 0; 
        }
        else if (hijo > 0)
        {
            // --- CÓDIGO DEL PADRE ---
            wait(NULL); // Espera a que el hijo termine su comando
            
            // Volvemos a pedir un comando para que el bucle continúe (esto es lo que estaba cortado abajo)
            printf("Introduzca un comando para ejecutar:\n");
            scanf("%s", comando);
        }
    }
    
    printf("Saliendo de JL-Shell. ¡Adiós!\n");
    return 0;
}