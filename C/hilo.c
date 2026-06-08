#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t hilo;
int contador = 0;


void* funcion(void* arg){
	contador+1;
	printf("Contador: %d", contador);
}


int main(){
	pthread_t tid;
	tid = pthread_self();
	printf("tid %u a", (unsigned int)tid);
	pthread_create(&hilo, NULL, &funcion, NULL);
	pthread_join(hilo, NULL);
	return 0;
}
