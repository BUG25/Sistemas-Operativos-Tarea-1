#define MAX_INPUT_LENGTH 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    //debe incluirse unistd.h para poder ejecutar proceso hijo
#include <sys/types.h> 
#include <sys/wait.h> 
#include <fcntl.h>     //para llamar open()
#include <ctype.h>

int main() {
    char input[MAX_INPUT_LENGTH];

    system("cmd.exe /c start wsl");

    while (1) { //usar 1 crea un bucle que se mantiene constantemente para que el prompt permanezca en ejecución
        printf("mishell: "); 

        //lee entrada de usuario
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0'; //elimina el salto de línea de la entrada del buffer

        //busca "|"
        char *pipe_token = strstr(input, "|");

        // ALGORITMO PIPE
        if (pipe_token) {
            *pipe_token = '\0'; //divide la entrada en dos comandos

            //comando antes de "|"
            char *command1 = input;

            //comando después de "|"
            char *command2 = pipe_token + 1;
            command2[strcspn(command2, "\n")] = '\0'; //elimina salto de línea

            //CREAR TUBERÍA
            int pipe_fd[2];
            if (pipe(pipe_fd) == -1) {
                perror("Error al crear la tubería");
                exit(EXIT_FAILURE);
            }

            pid_t pid1, pid2;

            //crea proceso hijo para el primer comando
            if ((pid1 = fork()) == -1) {
                perror("Error al crear el proceso hijo");
                exit(EXIT_FAILURE);
            } else if (pid1 == 0) {
                //PROCESO HIJO 1

                //cerrar lectura de la tubería
                close(pipe_fd[0]);

                //redirige la salida estándar al extremo de escritura de tubería
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);

                //ejecuta primer comando
                execlp(command1, command1, NULL);
                perror("Error al ejecutar el primer comando");
                exit(EXIT_FAILURE);
            }

            //crea proceso hijo para segundo comando
            if ((pid2 = fork()) == -1) {
                perror("Error al crear el proceso hijo");
                exit(EXIT_FAILURE);
            } else if (pid2 == 0) {
                //PROCESO HIJO 2

                //cierra la escritura de tubería
                close(pipe_fd[1]);

                //redirige entrada estándar a extremo de lectura de tubería
                dup2(pipe_fd[0], STDIN_FILENO);
                close(pipe_fd[0]);

                //ejecuta segundo comando
                execlp(command2, command2, NULL);
                perror("Error al ejecutar el segundo comando");
                exit(EXIT_FAILURE);
            }

            //cierra extremos de tubería en proceso padre
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            //espera que ambos procesos hijos terminen
            wait(NULL);
            wait(NULL);

        } else {

            // [COMANDOS]

            //$ECHO
            if (strncmp(input, "$echo", 5) == 0) {
                // Código para el comando $echo
            }
            //$CAT
            else if (strncmp(input, "$cat ", 5) == 0) {
                // Código para el comando $cat
            }
            //$HEAD
            else if (strncmp(input, "$head ", 6) == 0) {
                // Código para el comando $head
            }
            //$TAIL
            else if (strncmp(input, "$tail ", 6) == 0) {
                // Código para el comando $tail
            }
            //$GREP
            else if (strncmp(input, "$grep", 5) == 0) {
                // Código para el comando $grep
            }
            //$WC
            else if (strncmp(input, "$wc", 3) == 0) {
                // Código para el comando $wc
            }
            //$SORT
            else if (strncmp(input, "$sort", 5) == 0) {
                // Código para el comando $sort
            }
            //$PASTE
            else if (strncmp(input, "$paste", 6) == 0) {
                // Código para el comando $paste
            }

            //si el comando es "$exit" sale del bucle y termina la shell
            if (strcmp(input, "$exit") == 0) {
                break;
            }
        }
    }

    return 0;
}
