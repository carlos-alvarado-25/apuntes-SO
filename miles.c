#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define N 1000000

int main(void)
{
    pid_t hijo;
    int *variable_compartida = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *variable_compartida = 0;

    hijo=fork();

    if (hijo == 0)
    {
        for (int i = 0; i < N; i++)
        {
            *variable_compartida = *variable_compartida+1;
        }

        exit(0);
    } else if (hijo > 0) {
        for (int i = 0; i < N; i++)
        {
            *variable_compartida = *variable_compartida-1;
        }
    }

    wait(NULL);
    printf("Resultadin: %d\n", *variable_compartida);
    munmap(variable_compartida, sizeof(int));

    return 0;
}