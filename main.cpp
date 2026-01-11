#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "game.h"
#include "renderer.h"
#include "logic.h"

/**
 * SDL_AppInit - Initialisation principale de l'application
 * @appstate: Pointeur générique pour stocker notre GameState
 * @argc: Nombre d'arguments
 * @argv: Tableau des arguments
 *
 * Return: SDL_APP_CONTINUE en cas de succès, SDL_APP_FAILURE sinon.
 */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	/* 1. Allocation de la mémoire du jeu */
	GameState* gameState = new GameState();
	if (!gameState)
		return SDL_APP_FAILURE;
	
	*appstate = gameState;

	/* 2. Initialisation SDL Vidéo */
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return SDL_APP_FAILURE;
	}

	/* 3. Création Fenêtre (Redimensionnable pour respecter le sujet) */
	gameState->window = SDL_CreateWindow("Projet Pipe Revolution", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!gameState->window)
		return SDL_APP_FAILURE;

	/* 4. Création du Renderer */
	gameState->renderer = SDL_CreateRenderer(gameState->window, NULL);
	if (!gameState->renderer)
		return SDL_APP_FAILURE;

	/* 5. Initialisation des données de jeu */
	InitGameData(gameState);

	return SDL_APP_CONTINUE;
}

/**
 * SDL_AppEvent - Gestion des événements (Clavier, Souris)
 * @appstate: Notre GameState
 * @event: L'événement SDL reçu
 */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	GameState* gameState = (GameState*)appstate;

	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS; /* Quitter proprement */
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		/* Gestion du clic gauche */
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			HandleGridClick(gameState, event->button.x, event->button.y);
		}
	}

	return SDL_APP_CONTINUE;
}

/**
 * SDL_AppIterate - Boucle de rendu (60 FPS)
 * @appstate: Notre GameState
 */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	GameState* gameState = (GameState*)appstate;

	/* 1. Effacer l'écran (Fond Noir Profond) */
	SDL_SetRenderDrawColor(gameState->renderer, 20, 20, 30, 255);
	SDL_RenderClear(gameState->renderer);

	/* 2. Dessiner la grille (Appel modulaire) */
	RenderGrid(gameState);

	/* 3. Afficher le résultat */
	SDL_RenderPresent(gameState->renderer);

	return SDL_APP_CONTINUE;
}

/**
 * SDL_AppQuit - Nettoyage final
 * @appstate: Notre GameState
 */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	GameState* gameState = (GameState*)appstate;
	
	/* SDL3 gère la destruction window/renderer automatiquement à la fermeture,
	   mais on libère notre structure mémoire */
	if (gameState)
	{
		delete gameState;
	}
}