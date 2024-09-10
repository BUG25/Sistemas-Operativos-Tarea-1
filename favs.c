#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void favs_agregar(){

    FILE *Inv = fopen("Inventory.txt", "a");  // Abre el archivo en modo apéndice

    // Verificador de error
    if (Inv == NULL) {
        fprintf(stderr, "Error al crear el archivo Inventory.txt\n");
        exit(EXIT_FAILURE);
    }

    printf("Ingrese comandos favoritos: ");
    scanf("%d", &fav_count);

    fav_cmd = (Fav*)malloc(fav_count * sizeof(Fav));

    if(fav_cmd == NULL){
        fprintf(stderr, "Error de asignación de memoria.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i< fav_count; i++){
        printf("Ingresa el comando favorito %d: ", i+1);
        scanf("%s", fav_cmd[i].name);

        //Comporbación de comando
        char check_cmd[150]; 
        snprintf(check_cmd, sizeof(check_cmd), "command -v %s > /dev/null 2>&1", fav_cmd[i].name);  //(buffer, tamaño máximo, "comando que verifica si otro comando existe, redirección de salida estándar y de error") 
        int existe = system(check_cmd);

        if(existe == 0){
            fprintf(Inv, "%s\n", fav_cmd[i].name);
            printf("Comando '%s' agregado a favoritos.\n", fav_cmd[i].name);
        }else{
            printf("El comando '%s' NO existe en el sistema:(. \n)", fav_cmd[i].name);
        }
    }

    //liberación de memoria
    free(fav_cmd);
    fclose(Inv);
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
    FILE *Inv = fopen("Inventory.txt", "r");  // Abre el archivo en modo lectura

    // Verificador de error
    if (Inv == NULL) {
        fprintf(stderr, "Error al abrir el archivo Inventory.txt\n");
        return;
    }

    char command[100];
    char search_term[100];
    int found = 0;

    // Solicitar al usuario el comando a buscar
    printf("Ingrese el comando que desea buscar: ");
    scanf("%s", search_term);

    // Leer cada línea del archivo y comparar con el término de búsqueda
    while (fgets(command, sizeof(command), Inv) != NULL) {
        command[strcspn(command, "\n")] = 0;  // Eliminar el salto de línea

        if (strcmp(command, search_term) == 0) {
            printf("El comando '%s' se encuentra en la lista de favoritos.\n", search_term);
            found = 1;
            break;  // Salir del bucle si se encuentra el comando
        }
    }

    // Si el comando no se encontró
    if (!found) {
        printf("El comando '%s' NO se encuentra en la lista de favoritos.\n", search_term);
    }

    // Cerrar el archivo
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

void favs_cargar(){}

void favs_guardar(){}
