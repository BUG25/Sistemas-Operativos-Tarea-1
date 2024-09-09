# Shell Personalizada

Este proyecto incluye la implementación básica de un intérprete de comandos simple en Linux (shell). Permite ejecutar comandos estándar de Linux y conectar comandos por pipes. Además, incluye los comandos adicionales `favs` y `set_recordatorio`:

- **favs**: Permite mantener comandos favoritos guardados.
- **set_recordatorio**: Muestra un recordatorio en un tiempo indicado.

## Archivos

- `shell.c`: Implementa las funciones para manejar los comandos simples o unidos por pipes.
- `favs.h` y `favs.c`: Definen las funciones para el comando `favs`.

## Compilación y Ejecución

1. Es necesario tener un compilador compatible con C y un sistema operativo basado en Unix.
2. Para compilar, ejecuta el siguiente comando en la terminal:

    ```bash
    gcc main.c favs.c -o mishell
    ```

3. Para ejecutar la shell, utiliza el siguiente comando:

    ```bash
    ./mishell
    ```

4. Aparecerá el prompt de la shell (`$ `) en pantalla y podrás ingresar comandos.

## Ejemplos de Uso

1. **Ejecutar un comando Unix sin pipes**:

    ```bash
    $ wc archivo.txt
    ```

2. **Ejecutar comandos conectados por pipes**:

    ```bash
    $ grep test archivo.txt | wc -l
    ```

3. **Comando `favs`**:

    - Mostrar comandos favoritos:

        ```bash
        $ favs mostrar
        ```

    - Ejecutar un comando favorito (por ejemplo, el número 3):

        ```bash
        $ favs ejecutar 3
        ```

4. **Comando `set recordatorio`**:

    ```bash
    $ set recordatorio 5 "Hacer la tarea"
    ```

    Esto mostrará un recordatorio en 5 segundos con el mensaje "Hacer la tarea".


