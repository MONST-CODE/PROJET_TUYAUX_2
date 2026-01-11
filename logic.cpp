#include "logic.h"

/**
 * InitGameData - Remplit la grille au démarrage
 * @gameState: Pointeur vers l'état du jeu
 */
void InitGameData(GameState* gameState)
{
	int i, j;
	gameState->score = 0;

	/* Initialisation du générateur aléatoire */
	srand((unsigned int)time(NULL));

	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			/* Génère un tuyau aléatoire entre 1 et 3 (Droit, Coude, Croix) */
			/* 0 est vide, on met peu de vides pour tester */
			int randomType = (rand() % 3) + 1; 
			
			gameState->grid[i][j].type = (PipeType)randomType;
			gameState->grid[i][j].angle = 0;
			gameState->grid[i][j].isFixed = false;
			gameState->grid[i][j].isWet = false;
		}
	}
	
	/* On force une source en haut à gauche pour l'exemple */
	gameState->grid[0][0].type = PIPE_SOURCE;
	gameState->grid[0][0].isWet = true;
}

/**
 * HandleGridClick - Transforme le clic en rotation
 * @gameState: Pointeur vers l'état du jeu
 * @mouseX: Coordonnée X de la souris
 * @mouseY: Coordonnée Y de la souris
 */
void HandleGridClick(GameState* gameState, float mouseX, float mouseY)
{
	int windowWidth, windowHeight;
	float cellSize, startX, startY;
	int gridCol, gridRow;

	/* Note : On doit refaire le calcul de position pour savoir où on a cliqué.
	   Idéalement, ces valeurs de layout seraient dans GameState, mais
	   pour l'instant on les recalcule pour rester simple et robuste. */
	
	SDL_GetWindowSize(gameState->window, &windowWidth, &windowHeight);
	
	cellSize = (windowWidth < windowHeight) ? 
		((float)windowWidth / 10.0f) : ((float)windowHeight / 8.0f);
	
	startX = (windowWidth - (cellSize * GRID_COLS)) / 2.0f;
	startY = (windowHeight - (cellSize * GRID_ROWS)) / 2.0f;

	/* Conversion Pixels -> Index de tableau */
	gridCol = (int)((mouseX - startX) / cellSize);
	gridRow = (int)((mouseY - startY) / cellSize);

	/* Vérification qu'on est bien DANS la grille */
	if (gridCol >= 0 && gridCol < GRID_COLS && gridRow >= 0 && gridRow < GRID_ROWS)
	{
		Cell* c = &gameState->grid[gridRow][gridCol];
		
		/* Si la case n'est pas vide et n'est pas fixe, on tourne */
		if (c->type != PIPE_EMPTY && !c->isFixed)
		{
			c->angle = (c->angle + 90) % 360;
			/* Ici, plus tard, on appellera CheckFlow(gameState) */
			SDL_Log("Clic sur case [%d][%d] -> Nouvel angle: %d", gridRow, gridCol, c->angle);
		}
	}
}