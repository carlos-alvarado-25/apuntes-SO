#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define MAX_MENSAJE 100

typedef struct {
    int bandera;
    char mensaje[MAX_MENSAJE];
} Datos;

int main()
{
    int tuberia[2];
    pid_t hijo1, hijo2;

    Datos *variable = mmap(NULL, MAX_MENSAJE * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    variable->bandera = 0;

    pipe(tuberia);

    hijo1 = fork();

    if (hijo1 == 0) {

        close(tuberia[0]);

        char palabra[MAX_MENSAJE];

        printf("Ingresa una palabra: \n");
        scanf("%s", palabra);

        strcpy(variable->mensaje, palabra);
        variable->bandera = 1;

        while(variable->bandera != 2) {
            usleep(2);
        }

        printf("Hijo 1: El mensaje cifrado es %s \n", variable->mensaje);

        int vocalesModificadas = 0;
        for (int i = 0; i < strlen(variable->mensaje); i++) {
            if (variable->mensaje[i] == '*'){
                vocalesModificadas++;
            }
        }

        write(tuberia[1], &vocalesModificadas, sizeof(int));

        exit(0);

    }

    hijo2 = fork();

    if (hijo2 == 0) {

        close(tuberia[0]);
        close(tuberia[1]);

        while (variable->bandera != 1) {
            usleep(2);
        }

        printf("Hijo2: Mensaje de memoria compartida: %s \n", variable->mensaje);

        char vocales[] = "aeiouAEIOU";

        for (int i = 0; i < strlen(variable->mensaje); i++) {
            char letra = variable->mensaje[i];
            for (int j = 0; j < strlen(vocales); j++){
                if (letra == vocales[j]) {
                    variable->mensaje[i] = '*';
                    break;
                }
            }
        }

        variable->bandera = 2;

        exit(0);

    }

    close(tuberia[1]);

    int leido;

    read(tuberia[0], &leido, sizeof(int));

    printf("PADRE: Reporte Recibido. Se modificaron %d vocales \n", leido);

    wait(NULL);
    wait(NULL);

    printf("PADRE: Ambos Hijos acabaron satisfactoriamente \n");

    munmap(variable, sizeof(Datos));

    return 0;

}
