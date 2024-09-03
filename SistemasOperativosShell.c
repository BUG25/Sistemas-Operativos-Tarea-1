#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 100

void executeCommand(char *args[]) {
    if (execvp(args[0], args) == -1) {
        perror("shell");
        exit(EXIT_FAILURE);
    }
}

void executePipeCommands(char *commands[]) {
    int num_commands = 0;
    int pipefds[2 * (MAX_ARGS - 1)];
    int i;

    while (commands[num_commands] != NULL) {
        num_commands++;
    }

    for (i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + 2 * i) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (i > 0) {
                if (dup2(pipefds[2 * (i - 1)], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i < num_commands - 1) {
                if (dup2(pipefds[2 * i + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }

            char *args[MAX_ARGS];
            int arg_count = 0;
            char *token = strtok(commands[i], " ");
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            executeCommand(args);
        }
    }

    for (int j = 0; j < 2 * (num_commands - 1); j++) {
        close(pipefds[j]);
    }

    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    while (1) {
        printf("myshell:$ ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        // Eliminar el salto de línea al final de la entrada
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            break;
        }

        char *commands[MAX_ARGS];
        int num_commands = 0;
        char *token = strtok(input, "|");

        while (token != NULL) {
            commands[num_commands++] = token;
            token = strtok(NULL, "|");
        }
        commands[num_commands] = NULL;

        if (num_commands == 1) {
            char *args[MAX_ARGS];
            int arg_count = 0;
            token = strtok(commands[0], " ");
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                executeCommand(args);
            } else {
                wait(NULL);
            }
        } else {
            executePipeCommands(commands);
        }
    }
    return 0;
}
