#pragma once

#include <SDL3/SDL.h>


/* CONSTANTES GLOBALES */
#define GRID_ROWS 6
#define GRID_COLS 8

/**
 * enum PipeType - Définit les différents types de tuyaux
 * @PIPE_EMPTY: Case vide
 * @PIPE_STRAIGHT: Tuyau droit (I)
 * @PIPE_ELBOW: Tuyau coudé (L)
 * @PIPE_CROSS: Croisement (+)
 * @PIPE_SOURCE: Point de départ de l'eau
 * @PIPE_DRAIN: Point d'arrivée
 */
enum PipeType
{
	PIPE_EMPTY = 0,
	PIPE_STRAIGHT,
	PIPE_ELBOW,
	PIPE_CROSS,
	PIPE_SOURCE,
	PIPE_DRAIN
};

/**
 * struct Cell - Représente une case unique de la grille
 * @type: Le type de tuyau présent
 * @angle: L'angle de rotation (0, 90, 180, 270)
 * @isFixed: Si vrai, le joueur ne peut pas tourner cette pièce
 * @isWet: Si vrai, de l'eau coule dans ce tuyau
 */
struct Cell
{
	PipeType type;
	int angle;
	bool isFixed;
	bool isWet;
};

/**
 * struct GameState - L'état global du jeu (Le sac à dos)
 * @window: Pointeur vers la fenêtre SDL
 * @renderer: Pointeur vers le moteur de rendu SDL
 * @grid: Tableau 2D contenant toutes les cellules
 * @score: Score actuel du joueur
 */
struct GameState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	Cell grid[GRID_ROWS][GRID_COLS];
	int score;
};