/* Papá tiene un buffer de 100 caracteres, y el usuario tiene que itnroducir una cadena por teclado. El padre le envía al hijo la cadena. 
El hijo cuenta el número de vocales que hay en la cadena, y le devuelve al padre ese número. El padre saca el número por pantalla, y nos vamos de paseo.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

int contarVocales(char buffer[]);

int main()
{
    int tuberia1[2];
    int tuberia2[2];
    char buffer[100];

    pid_t hijo;

    pipe(tuberia1);
    pipe(tuberia2);

    hijo = fork();

    if (hijo > 0)
    {
        close(tuberia1[0]);
        close(tuberia2[1]);
        printf("[PADRE] Introduce tu cadena: \n");
        fgets(buffer, 100, stdin);

        buffer[strcspn(buffer, "\n")] = 0;

        write(tuberia1[1], buffer, strlen(buffer) + 1);

        close(tuberia1[1]);
        wait(NULL);

        int resultado;
        read(tuberia2[0], &resultado, sizeof(resultado));
        printf("[PADRE] Número de vocales: %d\n", resultado);

        close(tuberia2[0]);

    } else if (hijo == 0)
    {
        close(tuberia1[1]);
        close(tuberia2[0]);

        read(tuberia1[0], buffer, sizeof(buffer));
        printf("[HIJO] recibiendo buffer... \n");
        
        int numeroDeVocales = contarVocales(buffer);

        write(tuberia2[1], &numeroDeVocales, sizeof(numeroDeVocales));

        close(tuberia2[1]);

        exit(EXIT_SUCCESS);

    }
}

int contarVocales(char buffer[])
{
    int contadorDeVocales = 0;

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (strchr("aeiouAEIOU", buffer[i]))
            contadorDeVocales++;
    }

    return contadorDeVocales;
}