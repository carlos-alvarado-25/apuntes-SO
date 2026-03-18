#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main()
{
    pid_t proceso;
    int status;
    int numero = 7;

    printf("Soy el padre (PID %d): voy a crear el hijo \n", getpid());

    proceso=fork();

    if (proceso < 0){
        perror("Error en el fork");
	exit(EXIT_FAILURE);
    } else if (proceso==0) {
        printf("Hijo, PID %d: evaluando el nùmero %d es par o impar...\n", getpid(), numero);
	if (numero%2==0)
	   exit (0);
	else
	   exit(1);
    }
    else // Proceso padre 
    {
	printf("Padre, PID: %d. Esperando a que mi hijo, PID %d, termine su tarea \n", getpid(), proceso);
	wait(&status);
	if (WIFEXITED(status))
	{
	     int resultado_hijo = WEXITSTATUS(status);
	     if (resultado_hijo=0)
	     {
		puts("El hijo ha acabado, la variable es PAR");
	     }
	     else if (resultado_hijo==1)
	     {
		puts("El hijo ha acabado, la variable es IMPAR");	
	     }
	     else {
		puts("El hijo retorno un codigo desconocido");
	     }
	} else {
	     puts("El hijo no termino normalmente");
	}
    }

    return 0;
}
