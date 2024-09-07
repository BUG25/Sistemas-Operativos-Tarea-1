#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "favs.h"

Fav *fav_cmd = NULL;
int fav_count = 0;

void favs_crear(){}

void favs_agregar(){}

void favs_mostrar() {
    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados.\n");
        return;
    }

    printf("Lista de comandos favoritos almacenados:\n");
    for (int i = 0; i < fav_count; i++) {
        printf("[%d] %s\n", fav_cmd[i].id, fav_cmd[i].command);
    }
}

void favs_eliminar() {
    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados que eliminar.\n");
        return;
    }

    int id_eliminar;
    printf("Ingrese el ID del comando que desea eliminar: ");
    scanf("%d", &id_eliminar);

    int encontrar = 0;
    for (int i = 0; i < fav_count; i++) {
        if (fav_cmd[i].id == id_eliminar) {
            // Desplazar los elementos de la lista de comandos almacenados:
            for (int j = i; j < fav_count - 1; j++) {
                fav_cmd[j] = fav_cmd[j + 1];
            }
            fav_count--;

            // Redimensionar el arreglo para liberar memoria
            fav_cmd = realloc(fav_cmd, sizeof(Fav) * fav_count);

            printf("Comando con ID (%d) eliminado.\n", id_eliminar);
            encontrar = 1;
            break;
        }
    }

    if (!encontrar) {
        printf("No se encontró un comando favorito con el ID (%d).\nPrueba con otro ID.\n", id_eliminar);
    }
}


void favs_buscar(const char *cmd){
    if (fav_count == 0) {
        printf("La lista de favoritos esta vacia\n");
        return;
    }

    int found = 0;
    size_t len_cmd = strlen(cmd);

    for(int i = 0; i < fav_count; i++){
        if(strstr(fav_cmd[i].command, cmd) != NULL){
            printf("[%d] %s\n", fav_cmd[i].id, fav_cmd[i].command);
            found = 1;
        }
    }

    if(!found){
        printf("No se encontraron comandos que contengan '%s'.\n", cmd);
    }
}


void favs_borrar(){
    if(fav_count == 0){
        printf("La lista de favoritos ya esta vacia\n");
        return;
    }

    free(fav_cmd);
    fav_cmd = NULL;
    fav_count = 0;

    printf("Todos los comandos favoritos han sido borrados\n");
}


void favs_ejecutar(){}

void favs_cargar(){}

void favs_guardar(){}
