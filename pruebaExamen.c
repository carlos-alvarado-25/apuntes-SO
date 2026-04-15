#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
    pid_t lee, escribe;
    int *variableCompartida;
    
    int pipe_lectura[2]; 
    int pipe_fin[2];     

    if (pipe(pipe_lectura) == -1 || pipe(pipe_fin) == -1) {
        perror("Error creando tuberías");
        exit(EXIT_FAILURE);
    }

    variableCompartida = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    printf("[Padre %d] Iniciando programa. Creando procesos...\n", getpid());

    lee = fork();
    if (lee == 0) {
        // PROCESO LECTOR
        close(pipe_lectura[0]); 
        close(pipe_fin[1]);     
        
        printf("[Lector %d] Creado. Esperando teclado...\n", getpid());
        printf("Introduce un numero: ");
        scanf("%d", variableCompartida);
        
        printf("[Lector %d] Numero guardado. Avisando al escritor por la tuberia 1...\n", getpid());
        char aviso = '1';
        write(pipe_lectura[1], &aviso, 1);
        
        printf("[Lector %d] Bloqueado en tuberia 2, esperando permiso para morir...\n", getpid());
        read(pipe_fin[0], &aviso, 1);
        
        printf("[Lector %d] Permiso recibido. ¡Muriendo!\n", getpid());
        exit(0);
    }
    
    escribe = fork();
    if (escribe == 0) {
        // PROCESO ESCRITOR
        close(pipe_lectura[1]); 
        close(pipe_fin[0]);     
        
        printf("[Escritor %d] Creado. Bloqueado en tuberia 1 esperando al lector...\n", getpid());
        char aviso;
        read(pipe_lectura[0], &aviso, 1);
        
        printf("[Escritor %d] Aviso recibido. El numero leido es %d\n", getpid(), *variableCompartida);
        printf("Pulse Enter para finalizar...\n");
        
        while (getchar() != '\n'); 
        getchar();
        
        printf("[Escritor %d] Enter detectado. Dando permiso al lector por tuberia 2...\n", getpid());
        aviso = '1';
        write(pipe_fin[1], &aviso, 1);
        
        printf("[Escritor %d] ¡Muriendo!\n", getpid());
        exit(0);
    }
    
    // PROCESO PADRE
    close(pipe_lectura[0]); close(pipe_lectura[1]);
    close(pipe_fin[0]); close(pipe_fin[1]);
    
    printf("[Padre %d] Ya no pinto nada. Esperando a que mis hijos mueran...\n", getpid());
    wait(NULL);
    wait(NULL);
    
    printf("[Padre %d] Mis dos hijos han finalizado. Fin del programa.\n", getpid());
    munmap(variableCompartida, sizeof(int));
    return 0;
}
