#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Dimensiones de la ventana y de las baldosas (tiles)
const int ANCHO_VENTANA = 640;
const int ALTO_VENTANA = 480;
const int TAMANO_TILE = 40; // Cada cuadro del mapa mide 40x40 píxeles

// Dimensiones del mapa (Arreglo bidimensional)
#define FILAS 10
#define COLUMNAS 15

// Variables globales para SDL2
SDL_Window* ventana = NULL;
SDL_Renderer* renderizador = NULL;

// REQUISITO 5.2 y 5.3: Representación del mapa ampliado
char mapa[FILAS][COLUMNAS] = {
    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
    {'#','.','.','.','.','.','.','#','.','.','.','.','.','.','#'},
    {'#','.','#','#','.','#','.','#','.','#','.','#','#','.','#'},
    {'#','.','#','.','.','#','.','.','.','#','.','.','#','.','#'},
    {'#','.','.','.','#','#','#','#','#','#','#','.','.','.','#'},
    {'#','.','#','.','.','.','.','.','.','.','.','.','#','.','#'},
    {'#','.','#','#','.','#','.','#','.','#','.','#','#','.','#'},
    {'#','.','.','.','.','#','.','#','.','#','.','.','.','.','#'},
    {'#','.','.','.','.','.','.','.','.','.','.','.','.','.','#'},
    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}
};

// Posiciones en la matriz (No en píxeles directos para facilitar colisiones después)
int pacmanX = 1; 
int pacmanY = 1;

int fantasmaX = 13;
int fantasmaY = 8;

// Control de tiempo para el movimiento del fantasma
Uint32 ultimoMovimientoFantasma = 0;
const int RETARDO_FANTASMA = 300; // El fantasma se mueve cada 300 milisegundos

bool inicializarSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return false;
    }

    ventana = SDL_CreateWindow(
        "Pac-Man - Mapa Ampliado y Fantasma",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        ANCHO_VENTANA, ALTO_VENTANA,
        SDL_WINDOW_SHOWN
    );

    if (ventana == NULL) {
        return false;
    }

    renderizador = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    if (renderizador == NULL) {
        return false;
    }

    return true;
}

void cerrarSDL() {
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
}

// REQUISITO 7.4: Función para renderizar el mapa, Pac-Man y el Fantasma
void renderizarTodo() {
    // Fondo negro
    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
    SDL_RenderClear(renderizador);

    // Dibujar el mapa desde la matriz
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            SDL_Rect tile = { c * TAMANO_TILE, f * TAMANO_TILE, TAMANO_TILE, TAMANO_TILE };
            
            if (mapa[f][c] == '#') {
                SDL_SetRenderDrawColor(renderizador, 0, 0, 255, 255); // Paredes Azules
                SDL_RenderFillRect(renderizador, &tile);
            } else if (mapa[f][c] == '.') {
                // Dibujar pellets pequeños en el centro de la baldosa
                SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255); // Blanco
                SDL_Rect pellet = { (c * TAMANO_TILE) + 18, (f * TAMANO_TILE) + 18, 4, 4 };
                SDL_RenderFillRect(renderizador, &pellet);
            }
        }
    }

    // Dibujar a Pac-Man (Cuadro Amarillo)
    SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255);
    SDL_Rect rectPacman = { pacmanX * TAMANO_TILE, pacmanY * TAMANO_TILE, TAMANO_TILE, TAMANO_TILE };
    SDL_RenderFillRect(renderizador, &rectPacman);

    // Dibujar al Fantasma (Cuadro Rojo)
    SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
    SDL_Rect rectFantasma = { fantasmaX * TAMANO_TILE, fantasmaY * TAMANO_TILE, TAMANO_TILE, TAMANO_TILE };
    SDL_RenderFillRect(renderizador, &rectFantasma);

    SDL_RenderPresent(renderizador);
}

int main(int argc, char* args[]) {
    if (!inicializarSDL()) {
        return 1;
    }

    bool juegoCorriendo = true;
    SDL_Event evento;

    while (juegoCorriendo) {
        
        // 1. CAPTURA DE EVENTOS (Movimiento de Pac-Man por celdas)
        while (SDL_PollEvent(&evento) != 0) {
            if (evento.type == SDL_QUIT) {
                juegoCorriendo = false;
            }
            else if (evento.type == SDL_KEYDOWN) {
                int siguienteX = pacmanX;
                int siguienteY = pacmanY;

                switch (evento.key.keysym.sym) {
                    case SDLK_UP:    siguienteY--; break;
                    case SDLK_DOWN:  siguienteY++; break;
                    case SDLK_LEFT:  siguienteX--; break;
                    case SDLK_RIGHT: siguienteX++; break;
                }

                // REQUISITO 5.4: Detección básica de colisiones con paredes para Pac-Man
                if (mapa[siguienteY][siguienteX] != '#') {
                    pacmanX = siguienteX;
                    pacmanY = siguienteY;
                    
                    // Si pasa sobre un pellet, se lo come (lo borra de la matriz)
                    if (mapa[pacmanY][pacmanX] == '.') {
                        mapa[pacmanY][pacmanX] = ' ';
                    }
                }
            }
        }

        // 2. LÓGICA DEL FANTASMA (Movimiento automático por tiempo)
        Uint32 tiempoActual = SDL_GetTicks();
        if (tiempoActual - ultimoMovimientoFantasma > RETARDO_FANTASMA) {
            int sigFantasmaX = fantasmaX;
            int sigFantasmaY = fantasmaY;

            // REQUISITO 5.5.2: Comportamiento sugerido (Persecución simple en X e Y)
            if (pacmanX < fantasmaX) sigFantasmaX--;
            else if (pacmanX > fantasmaX) sigFantasmaX++;
            else if (pacmanY < fantasmaY) sigFantasmaY--;
            else if (pacmanY > fantasmaY) sigFantasmaY++;

            // Si el movimiento no choca con pared, el fantasma avanza
            if (mapa[sigFantasmaY][sigFantasmaX] != '#') {
                fantasmaX = sigFantasmaX;
                fantasmaY = sigFantasmaY;
            }

            ultimoMovimientoFantasma = tiempoActual;
        }

        // REQUISITO 5.7: Detección de colisión entre Pac-Man y el Fantasma (Fin del juego preliminar)
        if (pacmanX == fantasmaX && pacmanY == fantasmaY) {
            printf("¡El fantasma te atrapo! Fin de la partida.\n");
            juegoCorriendo = false;
        }

        // 3. RENDERIZADO
        renderizarTodo();

        SDL_Delay(16);
    }

    cerrarSDL();
    return 0;
}
