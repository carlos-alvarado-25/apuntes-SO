#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_HIJOS 4
#define INCREMENTOS 100000

int main()
{
	int *contador = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (contador == MAP_FAILED) {
        perror("Error en mmap");
        exit(EXIT_FAILURE);
    }

    *contador = 0;
        
    for (int i = 0; i < NUM_HIJOS; i++)
    {
		pid_t pid = fork(); // Declaro un bucle que haga fork de cada proceso 
		
		if (pid < 0) {
			perror("Error en fork");
			exit(EXIT_FAILURE);
		}
		
		if (pid == 0){ // En el bloque de los hijos, declaramos el bucle que genera la condición de carrera
			int id_interno = i + 1; // Sumo 1 a la posición del bucle y hago que ese sea el id_interno EJ: hijo1 en posición 0 = id 1, etc.
			for (int j = 0; j < INCREMENTOS; j++) {
				*contador = *contador + 1;
			}
			exit(id_interno); // Sale con el id interno 
		}
	}
	
	printf("=== ORDEN DE FINALIZACION DE LOS HIJOS ===\n");
    for (int i = 0; i < NUM_HIJOS; i++) {
        int status;
        pid_t pid_finalizado = wait(&status);

        if (WIFEXITED(status)) {
            int id_recuperado = WEXITSTATUS(status);
            
            // Buscamos el PID correspondiente al ID recuperado 
            // (En este caso, id 1 corresponde a pids[0], id 2 a pids[1], etc.)
            printf("Posicion %d: Finalizo el hijo con ID interno %d (PID: %d)\n", 
					i + 1, id_recuperado, pid_finalizado);
        }
    }
    
    int total = NUM_HIJOS * INCREMENTOS;
	int incrementosPerdidos = total - *contador;
	
	printf("== RESULTADO DEL CONTADOR COMPARTIDO == \n");
	printf("Valor esperado del contador %d \n", total);
	printf("Valor obtenido del contador %d \n", *contador);
	printf("Incrementos perdidos: %d \n", incrementosPerdidos);
	
	munmap(contador, sizeof(int));
    
	return 0;
}

