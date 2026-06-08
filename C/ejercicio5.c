#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#define DATOS 5
#define MAX_VALOR 20
int numeros[DATOS];
unsigned long long factoriales[DATOS];
pthread_mutex_t mutex;
unsigned long long calcular_factorial(int n)
{
unsigned long long resultado = 1;
for (int i = 1; i <= n; i++)
{
resultado *= i;
}
return resultado;
}
void *calcular_pares(void *arg)
{
for (int i = 0; i < DATOS; i += 2)
{
unsigned long long resultado = calcular_factorial(numeros[i]);
pthread_mutex_lock(&mutex);
factoriales[i] = resultado;
pthread_mutex_unlock(&mutex);
}
return NULL;
}
void *calcular_impares(void *arg)
{
for (int i = 1; i < DATOS; i += 2)
{
unsigned long long resultado = calcular_factorial(numeros[i]);
pthread_mutex_lock(&mutex);
factoriales[i] = resultado;
pthread_mutex_unlock(&mutex);
}
return NULL;
}
int main()
{
srand(time(NULL));
int tuberia[2];
if (pipe(tuberia) == -1)
{
perror("Error al crear la tuberia");
return 1;
}
for (int i = 0; i < DATOS; i++)
{
numeros[i] = (rand() % MAX_VALOR) + 1;
}
pid_t hijo;
hijo = fork();
if (hijo == 0)
{
close(tuberia[0]);
printf("Soy el hijo mis números son: \n");
for (int i = 0; i < DATOS; i++)
{
printf("%d ", numeros[i]);
}
printf("\n");
pthread_mutex_init(&mutex, NULL);
pthread_t hilo_pares, hilo_impares;
pthread_create(&hilo_pares, NULL, calcular_pares, NULL);
pthread_create(&hilo_impares, NULL, calcular_impares, NULL);
pthread_join(hilo_pares, NULL);
pthread_join(hilo_impares, NULL);
pthread_mutex_destroy(&mutex);
write(tuberia[1], factoriales, sizeof(factoriales));
close(tuberia[1]);
exit(0);
}
else if (hijo > 0)
{
// Padre
close(tuberia[1]); // Cierra la tuberia
wait(NULL); // Espera que termine el hijo
unsigned long long factoriales_recibidos[DATOS];
read(tuberia[0], factoriales_recibidos, sizeof(factoriales_recibidos));
close(tuberia[0]);
// Mostrar resultados
printf("\nProceso padre - Resultados:\n");
for (int i = 0; i < DATOS; i++)
{
printf("Número: %d - Factorial: %llu\n", numeros[i], factoriales_recibidos[i]);
}
}
return 0;
}
