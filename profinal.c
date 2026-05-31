#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Definimos el tamaño del mapa (puedes cambiarlo después al cargar el archivo)
#define FILAS 10
#define COLUMNAS 10
#define TAMANIO_BLOQUE 40 // Cada casilla del mapa medirá 40x40 píxeles

// Arreglo bidimensional para representar el mapa del juego
char mapa[FILAS][COLUMNAS] = {
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
    {'#', 'P', '.', '.', '#', '.', '.', '.', 'F', '#'},
    {'#', '.', '#', '.', '#', '.', '#', '#', '.', '#'},
    {'#', '.', '#', '.', '.', '.', '.', '#', '.', '#'},
    {'#', '.', '#', '#', '#', '#', '.', '#', '.', '#'},
    {'#', '.', '.', '.', '.', '.', '.', '.', '.', '#'},
    {'#', '#', '#', '.', '#', '#', '#', '#', '.', '#'},
    {'#', '.', '.', '.', '.', '.', '.', '#', '.', '#'},
    {'#', 'F', '.', '#', '#', '#', '.', '.', '.', '#'},
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
};

// Función simple para dibujar el mapa en la ventana de SDL
void renderizarMapa(SDL_Renderer *renderer) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            
            // Creamos un cuadrado para la casilla actual
            SDL_Rect bloque = {
                j * TAMANIO_BLOQUE, // Posición X en pantalla
                i * TAMANIO_BLOQUE, // Posición Y en pantalla
                TAMANIO_BLOQUE,     // Ancho
                TAMANIO_BLOQUE      // Alto
            };

            // Dependiendo del carácter en el arreglo, elegimos un color sencillo
            if (mapa[i][j] == '#') {
                // Paredes: color Azul
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &bloque);
            } 
            else if (mapa[i][j] == '.') {
                // Pellets (puntos): dibujamos un cuadradito pequeño blanco en el centro
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect pellet = {
                    (j * TAMANIO_BLOQUE) + 18,
                    (i * TAMANIO_BLOQUE) + 18,
                    4,
                    4
                };
                SDL_RenderFillRect(renderer, &pellet);
            } 
            else if (mapa[i][j] == 'P') {
                // Jugador (Pac-Man): color Amarillo
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(renderer, &bloque);
            } 
            else if (mapa[i][j] == 'F') {
                // Fantasma: color Rojo
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &bloque);
            }
        }
    }
}

int main(int argc, char **argv) {
    // Inicialización idéntica a tu código de referencia
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error %s\n", SDL_GetError());
        return 1;
    }

    // Ajustamos la ventana al tamaño del mapa (10 columnas * 40 px = 400 de ancho)
    SDL_Window *window = SDL_CreateWindow(
        "Pac-Man Simplificado - UNAM",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        COLUMNAS * TAMANIO_BLOQUE,
        FILAS * TAMANIO_BLOQUE,
        0
    );

    if (!window) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    // Ciclo principal del juego
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Limpiar pantalla con fondo negro
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Llamamos a nuestra función para dibujar todo el laberinto
        renderizarMapa(renderer);

        // Mostrar en pantalla
        SDL_RenderPresent(renderer);
        
        // Un pequeño retraso para no saturar el procesador
        SDL_Delay(16); 
    }

    // Limpieza final
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
