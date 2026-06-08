#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define NUM_HIJOS 3
#define INCREMENTOS 1000000

static int *variable;

int main(){

    variable = mmap(NULL, sizeof(*variable), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < NUM_HIJOS; i++){
        int id = i + 1;

        pid_t hijo = fork();

        if (hijo == 0){
            for(int i = 0; i < INCREMENTOS; i++){
                *variable += 1;
            }
            exit(id);
        }

    }

    for (int i = 0; i < NUM_HIJOS; i++){
        int status;
        pid_t pidFinalizado = wait(&status);

        if (WIFEXITED(status)){
            int idRecuperado = WEXITSTATUS(status);
            printf("Hijo con PID: %d recogido. Código de salida: %d\n", pidFinalizado, idRecuperado);
        }
    }

    printf("Valor de la variable compartida al finalizar: %d\n", *variable);
    int incrementosPerdidos = (NUM_HIJOS * INCREMENTOS) - *variable;
    printf("Incrementos perdidos: %d\n", incrementosPerdidos);

    return 0;
}
