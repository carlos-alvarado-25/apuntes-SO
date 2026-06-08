#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_HIJOS 3

int main(){
    int tuberia[2];
    pipe(tuberia);

    for (int i = 0; i < NUM_HIJOS; i++) {
        pid_t hijo = fork();

        if (hijo < 0) {
            perror("FORK FAILED");
            exit(EXIT_FAILURE);
        }

        if (hijo == 0) {
            int id = i + 1;
            close(tuberia[0]);

            char message[100];
            sprintf(message, "Agente %d: Servicio verificado correctamente\n", id);
            write(tuberia[1], message, strlen(message));
            close(tuberia[1]);

            exit(id * 10);

        }
    }

    close(tuberia[1]);

    char receivedMessage[200];
    int bytesLeidos;

    while ((bytesLeidos = read(tuberia[0], receivedMessage, sizeof(receivedMessage) - 1)) > 0){
        receivedMessage[bytesLeidos] = '\0';
        printf("%s", receivedMessage);
    }

    close(tuberia[0]);

    for (int i = 0; i < NUM_HIJOS; i++){
        int status;
        pid_t pidFinalizado = wait(&status);

        if (WIFEXITED(status)){
            int idRecuperado = WEXITSTATUS(status);
            printf("Hijo con PID: %d recogido. Código de salida: %d\n", pidFinalizado, idRecuperado);
        }
    }

}
