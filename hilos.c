#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

pthread_t ntid;

void printids(const char *s)
{
    pid_t pid;
    pthread_t tid;
    pid = getpid();
    tid=pthread_self(); // recibe el ID del hilo
    printf("%s pid %u tid %u \n", s, (unsigned int)pid, (unsigned int)tid);
    return;
}

void* nuevo_hilo (void *arg)
{
    printids("Nuevo hilo: ");
    return NULL;
}

int main(void) 
{
    pthread_create(&ntid /*Nombre de la variable que contendra el hilo */, NULL, &nuevo_hilo /*pasar la dirección de memoria de la función*/, NULL);
    printids("Hilo principal: ");
    pthread_join(ntid, NULL);
    return 0;
}