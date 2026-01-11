#include "renderer.h"

/**
 * RenderGrid - Implémentation du rendu de la grille
 * @gameState: Pointeur vers l'état du jeu
 */
void RenderGrid(GameState* gameState)
{
	int windowWidth, windowHeight;
	float cellSize;
	float gridWidth, gridHeight;
	float startX, startY;
	int i, j;

	/* 1. Récupération dynamique de la taille de la fenêtre */
	SDL_GetWindowSize(gameState->window, &windowWidth, &windowHeight);

	/* 2. Calcul de la taille d'une case (adaptatif) */
	/* On divise par 10 (largeur) ou 8 (hauteur) pour laisser des marges */
	cellSize = (windowWidth < windowHeight) ? 
		((float)windowWidth / 10.0f) : ((float)windowHeight / 8.0f);

	gridWidth = cellSize * GRID_COLS;
	gridHeight = cellSize * GRID_ROWS;

	/* 3. Centrage de la grille */
	startX = (windowWidth - gridWidth) / 2.0f;
	startY = (windowHeight - gridHeight) / 2.0f;

	/* 4. Boucle de dessin */
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			SDL_FRect cellRect = {
				startX + (j * cellSize),
				startY + (i * cellSize),
				cellSize - 4.0f, /* -4 pour l'espacement visuel (gutter) */
				cellSize - 4.0f
			};

			/* Choix de la couleur : Bleu si eau, Gris sinon */
			if (gameState->grid[i][j].isWet)
				SDL_SetRenderDrawColor(gameState->renderer, 0, 120, 255, 255);
			else if (gameState->grid[i][j].type != PIPE_EMPTY)
				SDL_SetRenderDrawColor(gameState->renderer, 100, 100, 120, 255);
			else
				SDL_SetRenderDrawColor(gameState->renderer, 40, 40, 50, 255);

			/* Remplissage */
			SDL_RenderFillRect(gameState->renderer, &cellRect);

			/* Bordure */
			SDL_SetRenderDrawColor(gameState->renderer, 200, 200, 200, 255);
			SDL_RenderRect(gameState->renderer, &cellRect);
		}
	}
}