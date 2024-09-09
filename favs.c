#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
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

int esNumerica(const char *cadena) {
    while (*cadena) {
        if (!isdigit(*cadena)) {
            return 0; // Si algún carácter no es un dígito, retorna falso
        }
        cadena++;
    }
    return 1; // Todos los caracteres son dígitos
}

void favs_eliminar() {
    if (fav_count == 0) {
        printf("No hay comandos favoritos almacenados que eliminar.\n");
        return;
    }
    char id_eliminar[7];
    char id_antesComa[3], id_despuesComa[3];
    printf("Ingrese el ID de dos comandos que desee eliminar, de la forma "x,y": ");
    scanf("%s", &id_eliminar);

    // Verificar que el formato de ingreso es el correcto:
    char *buscar_coma = strchr(id_eliminar, ',');
    while(buscar_coma == NULL){
        printf("Error en el formato al ingresar los IDs. Intentelo nuevamente.\nIngrese el ID de dos comandos que desee eliminar, de la forma "x,y": ");
        scanf("%s", &id_eliminar);
    }

    // Copiar ID antes y despues de la coma:
    strncpy(id_antesComa, id_eliminar, buscar_coma - id_eliminar);
    id_antesComa[buscar_coma - id_aliminar] = '\0';
    strcpy(id_despuesComa, buscar_coma + 1);

    // Verificar que ambas cadenas sean numéricas
    while(!esNumerica(id_antesComa) && !esNumerica(id_despuesComa)){
        printf("Error en el formato al ingresar los IDs, alguno de los IDs dados no es un numero. Intentelo nuevamente.\nIngrese el ID de dos comandos que desee eliminar, de la forma "x,y": ");
        scanf("%s", &id_eliminar);

        strncpy(id_antesComa, id_eliminar, buscar_coma - id_eliminar);
        id_antesComa[buscar_coma - id_aliminar] = '\0';
        strcpy(id_despuesComa, buscar_coma + 1);
    }

    // Pasar las cadenas a numeros:
    int id_numeroAntesComa = atoi(id_antesComa);
    int id_numeroDespuesComa = atoi(id_despuesComa);

    // Borrar los IDs:
    int encontrar = 0;
    for (int i = 0; i < fav_count; i++) {
        if (fav_cmd[i].id == id_numeroAntesComa) {
            // Desplazar los elementos de la lista de comandos almacenados:
            for (int j = i; j < fav_count - 1; j++) {
                fav_cmd[j] = fav_cmd[j + 1];
            }
            fav_count--;

            // Redimensionar el arreglo para liberar memoria
            fav_cmd = realloc(fav_cmd, sizeof(Fav) * fav_count);

            printf("Comando con ID (%d) eliminado.\n", id_numeroAntesComa);
            encontrar = 1;
            break;
        }
    }
    
    for (int i = 0; i < fav_count; i++) {
        if (fav_cmd[i].id == id_numeroDespuesComa) {
            // Desplazar los elementos de la lista de comandos almacenados:
            for (int j = i; j < fav_count - 1; j++) {
                fav_cmd[j] = fav_cmd[j + 1];
            }
            fav_count--;

            // Redimensionar el arreglo para liberar memoria
            fav_cmd = realloc(fav_cmd, sizeof(Fav) * fav_count);

            printf("Comando con ID (%d) eliminado.\n", id_numeroDespuesComa);
            encontrar = 1;
            break;
        }
    }

    if (!encontrar) {
        printf("No se encontró alguno de los comandos que deseea eliminar\n");
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
