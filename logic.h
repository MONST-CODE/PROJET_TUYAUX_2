#pragma once

#include "game.h"
#include <cstdlib> /* Pour rand() */
#include <ctime>   /* Pour time() */

/**
 * InitGameData - Initialise la grille avec des valeurs par défaut
 * @gameState: Pointeur vers l'état du jeu à remplir
 */
void InitGameData(GameState* gameState);

/**
 * HandleGridClick - Gère le clic de la souris sur la grille
 * @gameState: Pointeur vers l'état du jeu
 * @mouseX: Position X de la souris
 * @mouseY: Position Y de la souris
 *
 * Description: Convertit les pixels en coordonnées de grille et
 * effectue une rotation de 90 degrés si une case est touchée.
 */
void HandleGridClick(GameState* gameState, float mouseX, float mouseY);