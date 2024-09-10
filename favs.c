#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include "favs.h"

Fav *fav_cmd = NULL;
int fav_count = 0;

void favs_crear(){
    FILE *Inv = fopen("Inventory.txt", "w");

    // Verificador de error
    if (Inv == NULL) {
        fprintf(stderr, "Error al crear el archivo Inventory.txt\n");
        exit(EXIT_FAILURE);
    }

    printf("Archivo Inventory.txt creado exitosamente en el directorio.\n");

    fclose(Inv);  // Cierra el archivo
}

void favs_agregar(char *command){
    // Comandos que deben ser excluidos
    const char *excluded_commands[] = { "favs", "set recordatorio", NULL };

    // Verificar si el comando es uno de los excluidos
    for (int i = 0; excluded_commands[i] != NULL; i++) {
        if (strncmp(command, excluded_commands[i], strlen(excluded_commands[i])) == 0) {
            return; // No agregamos comandos que estén en la lista de excluidos
        }
    }

    // Abrir el archivo de favoritos en modo de lectura
    FILE *Inv = fopen("Inventory.txt", "r");
    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    // Verificar si el comando ya está en la lista de favoritos
    char line[MAX_INPUT_LENGTH];
    while (fgets(line, sizeof(line), Inv) != NULL) {
        line[strcspn(line, "\n")] = 0; // Eliminar el salto de línea
        if (strcmp(line, command) == 0) {
            fclose(Inv);
            return; // El comando ya está en favoritos, no lo agregamos
        }
    }
    fclose(Inv);

    // Si no está, lo agregamos a la lista de favoritos
    Inv = fopen("Inventory.txt", "a");
    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt para escribir\n");
        return;
    }

    fprintf(Inv, "%s\n", command);
    fclose(Inv);

    printf("Comando '%s' agregado a favoritos.\n", command);
}

void favs_mostrar() {
    FILE *Inv = fopen("Inventory.txt", "r");  // Abre el archivo en modo lectura

    // Verificador de error
    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    Fav instancia_fav;  //accede al struct en el header
    int index = 1;
    int found = 0;

    printf("Lista de comandos favoritos almacenados:\n");

    // Leer cada línea del archivo y almacenarla en instancia_fav.command
    while (fgets(instancia_fav.command, sizeof(instancia_fav.command), Inv) != NULL) {
        found = 1;
        // Asignar el índice a la estructura
        instancia_fav.id = index++;

        // Imprimir el índice y el comando almacenado
        printf("[%d] %s", instancia_fav.id, instancia_fav.command);
    }

    if (!found) {
        printf("No hay comandos favoritos almacenados.\n");
    }

    fclose(Inv);
}

void favs_eliminar() {
    FILE *Inv = fopen("Inventory.txt", "r");  // Abrir archivo en modo lectura

    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    Fav fav_cmds[100];  //temporal
    int fav_count = 0;
    char command[100];

    // Lee el archivo y almacena los comandos en arreglo temporal
    while (fgets(command, sizeof(command), Inv) != NULL) {
        command[strcspn(command, "\n")] = 0;  //limpia buffer
        strcpy(fav_cmds[fav_count].command, command);
        fav_cmds[fav_count].id = fav_count + 1;  // Asignar índices
        fav_count++;
    }
    fclose(Inv);

    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados.\n");
        return;
    }

    printf("Lista de comandos favoritos:\n");
    for (int i = 0; i < fav_count; i++) {
        printf("[%d] %s\n", fav_cmds[i].id, fav_cmds[i].command);
    }

    int eliminar_index;
    printf("Ingrese el número del comando que desea eliminar: ");
    scanf("%d", &eliminar_index);

    if (eliminar_index < 1 || eliminar_index > fav_count) {
        printf("Índice inválido. Operación cancelada.\n");
        return;
    }

    // Abrir el archivo en modo escritura para reescribir los comandos
    Inv = fopen("Inventory.txt", "w");
    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt para escritura\n");
        return;
    }

    //Reescribir todos los comandos excepto el seleccionado para eliminar
    for (int i = 0; i < fav_count; i++) {
        if (fav_cmds[i].id != eliminar_index) {
            fprintf(Inv, "%s\n", fav_cmds[i].command);
        }
    }

    fclose(Inv);

    printf("Comando [%d] eliminado con éxito.\n", eliminar_index);
}


void favs_buscar(const char *cmd){
    FILE *Inv = fopen("Inventory.txt", "r");

    if(Inv == NULL){
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    char command[100];
    int found = 0;
    int index = 1;

    printf("Comandos que contienen '%s':\n", cmd);

    while(fgets(command, sizeof(command), Inv) != NULL){
        command[strcspn(command, "\n")] = 0;

        if(strstr(command, cmd) != NULL){
            printf("[%d] %s\n", index, command);
            found = 1;
        }
        index++;
    }

    if (!found) {
        printf("No se encontraron comandos que contengan '%s'.\n", cmd);
    }

    fclose(Inv);
}


void favs_borrar(){
    FILE *Inv = fopen("Inventory.txt", "w");

    // Verificador de error
    if (Inv == NULL) {
        fprintf(stderr, "Error al crear el archivo Inventory.txt\n");
        exit(EXIT_FAILURE);
    }

    printf("Archivo Inventory.txt vaciado.\n");

    fclose(Inv);  // Cierra el archivo
}


void favs_ejecutar(){
    FILE *Inv = fopen("Inventory.txt", "r");

    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    Fav fav_cmds[100];
    int fav_count = 0;
    char command[100];

    // Lee todos los comandos favoritos en el arreglo fav_cmds
    while (fgets(command, sizeof(command), Inv) != NULL) {
        command[strcspn(command, "\n")] = 0;  // Limpia el salto de línea
        strcpy(fav_cmds[fav_count].command, command);
        fav_cmds[fav_count].id = fav_count + 1; // Asigna un índice
        fav_count++;
    }
    fclose(Inv);

    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados.\n");
        return;
    }

    int num;
    printf("Ingrese el número del comando que desea ejecutar: ");
    scanf("%d", &num);

    if (num < 1 || num > fav_count) {
        printf("Número de comando inválido.\n");
        return;
    }

    // Ejecutar el comando correspondiente
    pid_t pid = fork();
    if (pid == 0) {
        // Proceso hijo
        char *args[3]; // Comando y NULL
        args[0] = fav_cmds[num - 1].command; // Asigna el comando a ejecutar
        args[1] = NULL; // Termina la lista de argumentos

        printf("Ejecutando comando: %s\n", args[0]);

        // Ejecuta el comando usando execvp
        if (execvp(args[0], args) == -1) {
            perror("Error al ejecutar el comando");
        }
        exit(EXIT_FAILURE); // Sale si hay error en execvp
    } else if (pid < 0) {
        // Error en fork
        perror("Error al crear el proceso hijo");
    } else {
        // Proceso padre espera a que el hijo termine
        wait(NULL);
    }
}

void favs_cargar(){
    #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include "favs.h"

void favs_ejecutar(int id) {
    FILE *Inv = fopen("Inventory.txt", "r");

    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    Fav fav_cmds[100];
    int fav_count = 0;
    char command[100];

    // Lee todos los comandos favoritos en el arreglo fav_cmds
    while (fgets(command, sizeof(command), Inv) != NULL) {
        command[strcspn(command, "\n")] = 0;  // Limpia el salto de línea
        strcpy(fav_cmds[fav_count].command, command);
        fav_cmds[fav_count].id = fav_count + 1; // Asigna un índice
        fav_count++;
    }
    fclose(Inv);

    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados.\n");
        return;
    }

    // Validación del número proporcionado
    if (id < 1 || id > fav_count) {
        printf("Número de comando inválido.\n");
        return;
    }

    // Ejecutar el comando correspondiente
    pid_t pid = fork();
    if (pid == 0) {
        // Proceso hijo
        char *args[3]; // Comando y NULL
        args[0] = fav_cmds[id - 1].command; // Asigna el comando a ejecutar
        args[1] = NULL; // Termina la lista de argumentos

        printf("Ejecutando comando: %s\n", args[0]);

        // Ejecuta el comando usando execvp
        if (execvp(args[0], args) == -1) {
            perror("Error al ejecutar el comando");
        }
        exit(EXIT_FAILURE); // Sale si hay error en execvp
    } else if (pid < 0) {
        // Error en fork
        perror("Error al crear el proceso hijo");
    } else {
        // Proceso padre espera a que el hijo termine
        wait(NULL);
    }
}

void favs_guardar(){
    // Abrir el archivo en modo escritura ("w") para sobrescribir el contenido existente
    FILE *Inv = fopen("Inventory.txt", "w");

    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt para guardar.\n");
        return;
    }

    // Guardar cada comando en el archivo
    for (int i = 0; i < fav_count; i++) {
        fprintf(Inv, "%s\n", fav_cmd[i].command);
    }

    fclose(Inv);  // Cerrar el archivo

    printf("Comandos guardados exitosamente en Inventory.txt.\n");
}
