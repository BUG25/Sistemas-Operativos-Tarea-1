#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "favs.h"

#define MAX_INPUT_LENGTH 1024
// Agregación de memoria dinámica

// Divide un comando en argumentos
int parse_command(char *command, char ***args) {
    int num_args = 0;
    char *token;
    *args = NULL;

    token = strtok(command, " ");
    while (token != NULL) {
        *args = realloc(*args, sizeof(char *) * (num_args + 1));
        if (*args == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
        (*args)[num_args] = token;
        num_args++;
        token = strtok(NULL, " ");
    }

    *args = realloc(*args, sizeof(char *) * (num_args + 1));
    if (*args == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    (*args)[num_args] = NULL;

    return num_args;
}

// Ejecuta un comando
void execute_command(char *command) {
    char **args;
    int num_args = parse_command(command, &args);

    if (execvp(args[0], args) == -1) {
        perror("Error al ejecutar el comando");
        exit(EXIT_FAILURE);
    }
}

// Ejecuta comandos conectados por pipes
void execute_pipe_commands(char *commands[], int num_commands) {
    int **pipe_fds;
    pid_t pid;
    int i;

    // Crear los pipes necesarios
    pipe_fds = malloc(sizeof(int *) * (num_commands - 1));
    if (pipe_fds == NULL) {
        perror("Error al asignar memoria para pipes");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < num_commands - 1; i++) {
        pipe_fds[i] = malloc(sizeof(int) * 2);
        if (pipe(pipe_fds[i]) == -1) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    // Iterar sobre cada comando para crear un proceso hijo que lo ejecute
    for (i = 0; i < num_commands; i++) {
        pid = fork();
        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Si no es el primer comando, redirige la entrada estándar
            if (i > 0) {
                if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) == -1) {
                    perror("Error al redirigir entrada");
                    exit(EXIT_FAILURE);
                }
            }

            // Si no es el último comando, redirige la salida estándar
            if (i < num_commands - 1) {
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1) {
                    perror("Error al redirigir salida");
                    exit(EXIT_FAILURE);
                }
            }

            // Cerrar todos los pipes en el proceso hijo para que no haya fugas
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            // Ejecutar el comando actual
            execute_command(commands[i]);
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }
    }

    // Cerrar todos los pipes en el proceso padre
    for (int j = 0; j < num_commands - 1; j++) {
        close(pipe_fds[j][0]);
        close(pipe_fds[j][1]);
        free(pipe_fds[j]);
    }
    free(pipe_fds);

    // Esperar a que todos los procesos hijos terminen
    for (int j = 0; j < num_commands; j++) {
        wait(NULL);
    }
}

//PARA MANEJAR LOS COMANDOS FAVS
//FALTA COMPLETAR
void handle_favs(char *input){
    char *token = strtok(input, " ");
    if(strcmp(token, "favs") == 0){
        token = strtok(NULL, " ");
        if(token != NULL){
            if(strcmp(token, "crear") == 0){

            }else if(strcmp(token, "mostrar") == 0){

            }else if(strcmp(token, "eliminar") == 0){
                
            }else if(strcmp(token, "buscar") == 0){
                char *cmd = strtok(NULL, " ");
                if(cmd != NULL) favs_buscar(cmd);
                else printf("Error: Debes proporcionar un comando para buscar.\n");
            }else if(strcmp(token, "borrar") == 0){
                favs_borrar();
            }else if(strcmp(token, "ejecutar") == 0){

            }else if(strcmp(token, "cargar") == 0){

            }else if(strcmp(token, "guardar") == 0){

            }else{
                printf("Error: Comando desconocido despues de 'favs'.\n");
            }
        }
    }
    
}

int main() {
    char input[MAX_INPUT_LENGTH];
    char **commands;
    int num_commands;

    while (1) { // Bucle infinito para mantener la shell en ejecución
        printf("$ "); // Prompt
        fflush(stdout);

        // Lee la entrada del usuario
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0'; // Elimina el salto de línea al final

        // Si el usuario escribe "exit", se termina el programa
        if (strcmp(input, "exit") == 0) {
            break;
        }

        //MANEJO DE LOS FAVS
        if (strncmp(input, "favs ", 5) == 0) {
            handle_favs(input);
            continue;
        }

        // Divide el input en comandos usando "|" como delimitador
        char *token = strtok(input, "|");
        num_commands = 0;
        commands = NULL;

        while (token != NULL) {
            commands = realloc(commands, sizeof(char *) * (num_commands + 1));
            if (commands == NULL) {
                perror("Error al asignar memoria");
                exit(EXIT_FAILURE);
            }
            commands[num_commands++] = token;
            token = strtok(NULL, "|");
        }
        commands = realloc(commands, sizeof(char *) * (num_commands + 1));
        if (commands == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
        commands[num_commands] = NULL;

        // Elimina espacios extra en los comandos
        for (int i = 0; i < num_commands; i++) {
            commands[i] += strspn(commands[i], " ");
        }

        // Ejecución de comandos, si hay más de uno se ejecuta con pipes
        if (num_commands > 1) {
            execute_pipe_commands(commands, num_commands);
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                execute_command(commands[0]);
            } else if (pid < 0) {
                perror("Error al crear el proceso hijo");
            } else {
                wait(NULL);
            }
        }

        free(commands);
    }

    return 0;
}
