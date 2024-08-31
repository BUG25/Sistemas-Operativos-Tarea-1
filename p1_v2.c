#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*
CREO QUE FALTA CONFIGURAR QUE SEAN PIPES ILIMITADAS Y ARGS ILIMITADOS 
TODO ILIMITADO CON MEMORIA DINÁMICA SUPONGO
Y HACER MÁS PRUEBAS, ETC, NO ES VERSIÓN FINAL
*/

#define MAX_INPUT_LENGTH 1024
#define MAX_NUM_PIPES 20  //Número máximo de pipes que se pueden usar en un comando

//Divide un comando en argumentos
int parse_command(char* command, char** args){
    int n = 0;
    while((args[n] = strsep(&command, " ")) != NULL){
        if(*args[n] != '\0') n++;
    }
    return n;
}

void execute_command(char *command){
    char *args[MAX_INPUT_LENGTH];
    int num_params = parse_command(command, args);

    //Ejecuta el comando
    if(execvp(args[0], args) == -1){
        perror("Error al ejecutar el comando");
        exit(EXIT_FAILURE);
    }
}

//Ejecución de comandos conectados por pipes
void execute_pipe_commands(char *commands[], int num_commands){
    int pipe_fd[MAX_NUM_PIPES][2];
    pid_t pid;

    //Crear los pipes necesarios
    for(int i = 0; i < num_commands - 1; i++){
        if(pipe(pipe_fd[i]) == -1){
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    //Iterar sobre cada comando para crear un proceso hijo que lo ejecute
    for(int i = 0; i < num_commands; i++){
        pid = fork();
        if(pid == -1){
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }else if (pid == 0){
            //Si no es el primer comando, redirige la entrada estándar
            if(i > 0){
                dup2(pipe_fd[i - 1][0], STDIN_FILENO);
            }
            //Si no es el último comando, redirige la salida estándar
            if(i < num_commands - 1){
                dup2(pipe_fd[i][1], STDOUT_FILENO);
            }
            //Cerrar todos los pipes en el proceso hijo para que no haya fugas
            for(int j = 0; j < num_commands - 1; j++){
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }

            //Ejecutar el comando actual
            execute_command(commands[i]);
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }
    }

    //Cerrar todos los pipes en el proceso padre
    for(int i = 0; i < num_commands - 1; i++){
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }

    //Espera a que todos los procesos hijos terminen
    for(int i = 0; i < num_commands; i++){
        wait(NULL);
    }
}

int main(){
    char input[MAX_INPUT_LENGTH];
    char *commands[MAX_NUM_PIPES + 1]; //Se almacenan los comandos separados

    while(1){ //Bucle infinito para mantener la shell en ejecución
        printf("$ "); //Prompt
        fflush(stdout); //

        //Lee la entrada del usuario
        if(fgets(input, sizeof(input), stdin) == NULL){
            perror("Error al leer la entrada");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0'; //Elimina el salto de línea al final

        //Si el usuario escribe "exit", se termina el programa
        if(strcmp(input, "exit") == 0){
            break;
        }

        //Divide el input en comandos usando "|" como delimitador
        int num_commands = 0;
        commands[num_commands] = strtok(input, "|");
        while(commands[num_commands] != NULL && num_commands < MAX_NUM_PIPES){
            commands[++num_commands] = strtok(NULL, "|");
        }

        //Elimina espacios extra en los comandos
        for(int i = 0; i < num_commands; i++){
            commands[i] = commands[i] + strspn(commands[i], " ");
        }

        //Ejecución de comandos, si hay más de uno se ejecuta con pipes
        if(num_commands > 1){
            execute_pipe_commands(commands, num_commands);
        }else{
            pid_t pid = fork();
            if(pid == 0){
                execute_command(commands[0]);
            }else if(pid < 0){
                perror("Error al crear el proceso hijo");
            }else{
                wait(NULL);
            }
        }
    }

    return 0;
}
