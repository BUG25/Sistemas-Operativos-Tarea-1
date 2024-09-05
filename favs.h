#ifndef FAVS_H
#define FAVS_H

#define MAX_INPUT_LENGTH 1024

typedef struct{
    int id; //Cada comando debe además tener asociado un número que permita identificarlo
    char command[MAX_INPUT_LENGTH];
}Fav;


// Funciones para el manejo de favoritos
void favs_crear();
void favs_agregar();
void favs_mostrar();
void favs_eliminar();
void favs_buscar(const char *cmd);
void favs_borrar();
void favs_ejecutar();
void favs_cargar();
void favs_guardar();

#endif
