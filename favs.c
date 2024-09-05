#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "favs.h"

Fav *fav_cmd = NULL;
int fav_count = 0;

void favs_crear(){}

void favs_agregar(){}

void favs_mostrar(){}

void favs_eliminar(){}

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
