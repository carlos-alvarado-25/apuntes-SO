#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int dronesEnCinta = 0;
int totalFabricados = 0;
int totalEmpaquetados = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t productor, consumidor;
pthread_cond_t cintaLlena;
pthread_cond_t cintaVacia;

int main(){

}
