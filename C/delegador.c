#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define ELEMENTOS 5

int main(){

    int pipe_ida[2];
    int pipe_vuelta[2];

    pipe(pipe_ida);
    pipe(pipe_vuelta);


    pid_t hijo;
    hijo = fork();

    if (hijo < 0){
        perror("Fork failed");
        exit(1);
    }

    if (hijo == 0) {

        close(pipe_ida[1]);
        close(pipe_vuelta[0]);

        int arrayRecibido[ELEMENTOS];
        int totalElementos = sizeof(arrayRecibido) / sizeof(arrayRecibido[0]);
        int suma = 0;

        read(pipe_ida[0], arrayRecibido, sizeof(arrayRecibido));
        close(pipe_ida[0]);

        for (int i = 0; i < totalElementos ;i++){
            suma += arrayRecibido[i];
            printf("SUMA PARCIAL: %d\n", suma);
        }

        write(pipe_vuelta[1], &suma, sizeof(suma));
        close(pipe_vuelta[1]);

        exit(0);

    } else if (hijo > 0){

        close(pipe_ida[0]);
        close(pipe_vuelta[1]);

        int numeros[ELEMENTOS] = {4, 2, 1, 4, 5};
        int sumaTotal;

        write(pipe_ida[1], &numeros, sizeof(numeros));
        close(pipe_ida[1]);

        read(pipe_vuelta[0], &sumaTotal, sizeof(sumaTotal));
        close(pipe_vuelta[0]);

        printf("[Padre] El hijo me ha devuelto la suma total: %d\n", sumaTotal);

        wait(NULL);
    }

    return 0;
}
