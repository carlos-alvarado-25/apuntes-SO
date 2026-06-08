#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_COMANDO 256
#define ARCHIVO_HISTORIAL ".cerlet_history"

// --- ESTRUCTURA DE ALIAS ---
#define MAX_ALIASES 50
struct Alias {
    char nombre[50];
    char comando[200];
};

struct Alias lista_alias[MAX_ALIASES];
int total_aliases = 0;

// --- FUNCIÓN PARA LEER EL .cerletrc ---
void cargar_configuracion() {
    char ruta_rc[1024];
    sprintf(ruta_rc, "%s/.cerletrc", getenv("HOME"));

    FILE *archivo = fopen(ruta_rc, "r");
    if (!archivo) return;

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) && total_aliases < MAX_ALIASES) {
        linea[strcspn(linea, "\n")] = 0; // Quitar el salto de línea

        // 1. Ignorar líneas vacías o comentarios
        if (strlen(linea) == 0 || linea[0] == '#') continue;

        // 2. Si empieza por "alias ", lo guardamos en la memoria
        if (strncmp(linea, "alias ", 6) == 0) {
            char *puntero_igual = strchr(linea, '=');
            if (puntero_igual != NULL) {
                *puntero_igual = '\0';
                strcpy(lista_alias[total_aliases].nombre, linea + 6);
                strcpy(lista_alias[total_aliases].comando, puntero_igual + 1);
                total_aliases++;
            }
        }
        // 3. ¡LO NUEVO!: Si no es un alias, ejecutamos el comando tal cual
        else {
            system(linea);
        }
    }
    fclose(archivo);
}

int main()
{
    signal(SIGCHLD, SIG_IGN);
    read_history(ARCHIVO_HISTORIAL);

    printf("\033[H\033[J");

    // CARGAMOS LA CONFIGURACIÓN ANTES DE EMPEZAR
    cargar_configuracion();
    printf("\n");

    while (1) {
        char *usuario = getenv("USER");
        if (usuario == NULL) usuario = "usuario";

        char ruta[1024];
        getcwd(ruta, sizeof(ruta));

        char *ultimaCarpeta = strrchr(ruta, '/');
        if (ultimaCarpeta != NULL && strcmp(ruta, "/") != 0) {
            ultimaCarpeta++;
        } else {
            ultimaCarpeta = ruta;
        }

        char promptAColor[2048];
        sprintf(promptAColor, "\x01\033[1m\x02%s\x01\033[0m\x02 \x01\033[90m\x02%s\x01\033[0m\x02 > ", usuario, ultimaCarpeta);

        char *comandoIntroducido = readline(promptAColor);

        if (comandoIntroducido == NULL) {
            printf("\n[Shell]: Saliendo (Ctrl+D)...\n");
            break;
        }

        if (strlen(comandoIntroducido) > 0) {
            add_history(comandoIntroducido);
            append_history(1, ARCHIVO_HISTORIAL);
        } else {
            free(comandoIntroducido);
            continue;
        }

        // --- EXPANSIÓN DE ALIAS ---
        char comandoExpandido[MAX_COMANDO * 2] = "";
        char copiaSeguridad[MAX_COMANDO];
        strncpy(copiaSeguridad, comandoIntroducido, MAX_COMANDO - 1);

        char *primeraPalabra = strtok(copiaSeguridad, " ");
        char *restoDelComando = strtok(NULL, ""); // Captura todo lo que sobra (los argumentos)

        int esAlias = 0;
        for (int j = 0; j < total_aliases; j++) {
            if (strcmp(primeraPalabra, lista_alias[j].nombre) == 0) {
                // Si encontramos un alias, ponemos el comando real en el buffer
                strcpy(comandoExpandido, lista_alias[j].comando);
                esAlias = 1;
                break;
            }
        }

        if (!esAlias) {
            strcpy(comandoExpandido, primeraPalabra); // Si no es alias, se queda igual
        }

        if (restoDelComando != NULL) {
            strcat(comandoExpandido, " ");
            strcat(comandoExpandido, restoDelComando); // Le volvemos a pegar los argumentos (ej. /etc)
        }

        // --- TOKENIZACIÓN DEL COMANDO YA EXPANDIDO ---
        char *argumentos[20];
        int i = 0;
        argumentos[i] = strtok(comandoExpandido, " ");
        while (argumentos[i] != NULL && i < 19) {
            i++;
            argumentos[i] = strtok(NULL, " ");
        }
        argumentos[i] = NULL;

        if (argumentos[0] == NULL) {
            free(comandoIntroducido);
            continue;
        }

        // --- BUILT-INS ---
        if (strcmp(argumentos[0], "exit") == 0) {
            write_history(ARCHIVO_HISTORIAL);
            free(comandoIntroducido);
            break;
        }

        if (strcmp(argumentos[0], "cd") == 0) {
            if (argumentos[1] == NULL) chdir(getenv("HOME"));
            else if (chdir(argumentos[1]) != 0) perror("Cerlet-OS: cd");
            free(comandoIntroducido);
            continue;
        }

        // --- SEGUNDO PLANO ---
        int segundoPlano = 0;
        if (i > 0 && strcmp(argumentos[i-1], "&") == 0) {
            segundoPlano = 1;
            argumentos[i-1] = NULL;
        }

        // --- CLONACIÓN ---
        pid_t hijo = fork();

        if (hijo < 0) {
            perror("Fork fallido");
            free(comandoIntroducido);
            exit(EXIT_FAILURE);
        }

        if (hijo > 0) {
            if (segundoPlano == 0) waitpid(hijo, NULL, 0);
            else printf("[1] Proceso en segundo plano PID: %d\n", hijo);
            free(comandoIntroducido);
        } else if (hijo == 0) {
            execvp(argumentos[0], argumentos);
            printf("Cerlet-OS: comando no encontrado: %s\n", argumentos[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
