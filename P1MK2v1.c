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
                // Primero obtenemos el nombre del archivo.
                char *filename = input + 5;
                // Se abre el archivo en modo lectura.
                FILE *file = fopen(filename, "r");
                
                // En caso de que el archivo no exista:
                if (file == NULL) {
                    perror("Error al abrir el archivo");
                // En caso de que se logre abrir un archivo:
                } else {
                    char line[256]; // Se establece un lìmite para las lineas.
                    while (fgets(line, sizeof(line), file)) {  // Leer línea por línea.
                        printf("%s", line);  // Imprimir cada línea.
                    }
                    fclose(file);  // Cerrar el archivo.
                }
            }
            //$HEAD (aun en proceso)
            else if (strncmp(input, "$head ", 6) == 0) {
                // Primero obtenemos el nombre del archivo:
                char *filename = input + 6;
                // Se abre el archivo con este nombre en modo lectura:
                FILE *file = fopen(filename, "r");

                // En caso de que el archivo no exista:
                if (file == NULL) {
                    perror("Error al abrir el archivo");
                // En caso de que se logre abrir un archivo:
                } else {
                    char line[256]; // Se establece un límite de carácteres para las líneas del archivo.
                    int line_count = 0; // Contador de lìneas leidas.
                    while (fgets(line, sizeof(line), file) && line_count < 10) {  // Leer línea por línea.
                        printf("%s", line);  // Imprimir cada línea.
                        line_count++;
                    }
                    fclose(file);  // Cerrar el archivo.
                }
            }
            //$TAIL
            else if (strncmp(input, "$tail ", 6) == 0) {
                // Código para el comando $tail
            }
            //$GREP //falta probar con los pipes
            else if (strncmp(input, "$grep ", 6) == 0){
                char *token = strtok(input + 6, " ");
                int show_line_numbers = 0, ignore_case = 0, show_filenames = 0, show_no_match = 0;
                char *pattern = NULL;
                char *filename = NULL;

                //Procesar opciones
                while(token != NULL){
                    if(token[0] == '-'){
                        if(strchr(token, 'n')) show_line_numbers = 1;
                        if(strchr(token, 'i')) ignore_case = 1;
                        if(strchr(token, 'l')) show_filenames = 1;
                        if(strchr(token, 'v')) show_no_match = 1;
                    }else if(pattern == NULL){
                        pattern = token;

                        //Manejar comillas en el patrón
                        if(pattern[0] == '"' && pattern[strlen(pattern)-1] == '"'){
                            pattern[strlen(pattern)-1] = '\0'; //Eliminar comilla final
                            pattern++; //omitir comilla inicial
                        }
                    }else{
                        filename = token;
                    }
                    token = strtok(NULL, " ");
                }

                if(pattern == NULL || filename == NULL){
                    fprintf(stderr, "Modo de uso: $grep -nivl patron archivo\n");
                }else{
                    FILE *file = fopen(filename, "r");
                    if(file == NULL){
                        perror("Error al abrir el archivo");
                    }else{
                        char line[256];
                        int line_number = 0;
                        int match_count = 0;

                        while(fgets(line, sizeof(line),file)){
                            line_number++;
                            char *match = strstr(line, pattern);

                            //-i: Búsqueda insensible a mayúsculas y minúsculas
                            if(ignore_case){
                                char line_lower[256];
                                char pattern_lower[256];
                                for(int i = 0; line[i]; i++){
                                    line_lower[i] = tolower(line[i]);
                                }
                                line_lower[strlen(line)] = '\0';
                                for(int i = 0; pattern[i]; i++){
                                    pattern_lower[i] = tolower(pattern[i]);
                                }
                                pattern_lower[strlen(pattern)] = '\0';
                                match = strstr(line_lower, pattern_lower);
                            }

                            //-v: Muestra líneas que no coinciden con el patrón
                            int is_match = (match != NULL);
                            if(show_no_match){
                                is_match = !is_match;
                            }
                            if(is_match){
                                match_count = 1;
                                if(show_filenames){
                                    printf("%s\n", filename);
                                    break; //-l: se muestra el nombre del archivo una vez
                                }else{
                                    if(show_line_numbers){
                                        printf("%d:", line_number);
                                    }
                                    printf("%s", line);
                                }
                            }
                        }
                        printf("\n");
                        fclose(file); 

                        if(show_filenames && !match_count){
                        //Por si no hay nada al usar -l
                        }
                    } 
                }
            }
            //$WC //Falta probar con los pipes
            else if (strncmp(input, "$wc ", 4) == 0){
                char *token = strtok(input + 4, " ");
                int lines_count = 0, words_count = 0, chars_count = 0;
                char *filename = NULL;

                //Procesar -l,-w,-c
                while(token != NULL){
                    if(token[0] == '-'){
                        if(strchr(token, 'l')) lines_count = 1;
                        if(strchr(token, 'w')) words_count = 1;
                        if(strchr(token, 'c')) chars_count = 1;
                    }else{
                        filename = token;
                    }
                    token = strtok(NULL, " ");
                }

                if(!lines_count && !words_count && !chars_count){
                    lines_count = 1;
                    words_count = 1;
                    chars_count = 1;
                }

                if(filename == NULL){
                    fprintf(stderr, "Modo de uso: $wc -lwc archivo\n");
                }else{
                    FILE *file = fopen(filename, "r");
                    if(file == NULL){
                        perror("Error al abrir el archivo");
                    }else{
                        int lines = 0, words = 0, chars = 0;
                        char line[256];
                        while(fgets(line, sizeof(line), file)){
                            chars += strlen(line);
                            lines++;
                            char *word = strtok(line, " \t\n");
                            while(word != NULL){
                                words++;
                                word = strtok(NULL, " \t\n");
                            }
                        }
                        fclose(file);

                        if(lines_count) printf("Lineas: %d\n", lines);
                        if(words_count) printf("Palabras: %d\n", words);
                        if(chars_count) printf("Caracteres: %d\n", chars);
                    }
                }
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
