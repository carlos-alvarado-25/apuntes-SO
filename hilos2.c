#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define N 1000

pthread_t ntid;
int contador = 0;


void funcion_resta(const char *s)
{
    pid_t pid;
    pthread_t tid;
    pid = getpid();
    tid=pthread_self(); // recibe el ID del hilo
    printf("%s pid %u tid %u \n", s, (unsigned int)pid, (unsigned int)tid);
    for (int i = 0; i < N; i++)
    {
        contador = contador-1;
    }
}

void* funcion_suma(void *arg)
{
    for (int i = 0; i < N; i++)
    {
        contador = contador+1;
    }
    return NULL;
}

void ejecutar() 
{
    pthread_create(&ntid /*Nombre de la variable que contendra el hilo */, NULL, &funcion_suma /*pasar la dirección de memoria de la función*/, NULL);
    funcion_resta("Hilo principal: ");
    pthread_join(ntid, NULL);
    printf("El valor del contador es %d \n", contador);
}


int main(void)
{
    ejecutar();

    return 0;
}
