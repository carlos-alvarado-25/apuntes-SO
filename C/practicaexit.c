#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define NUM_HIJOS 3

int main(){

    for (int i = 0; i < NUM_HIJOS; i++){
        int id = i + 1;

        pid_t hijo = fork();

        if (hijo == 0){
            printf("  -> [Hijo %d] Creado con PID: %d. Buscando...\n", id, getpid());
            sleep(20);
            exit(id * 10);
        }
    }

    for(int i = 0; i < NUM_HIJOS; i++){
        int status;
        int pidRecogido = wait(&status);

        if (WIFEXITED(status)){
            int id = WEXITSTATUS(status);
            printf("[HIJO %d]: Finalizando su trabajo...\n", id);
        }
    }

    return 0;
}
