#pragma once

#include "game.h"

/**
 * RenderGrid - Dessine la grille de jeu à l'écran
 * @gameState: Pointeur vers la structure d'état globale du jeu
 *
 * Description: Calcule les dimensions dynamiques en fonction de la taille
 * actuelle de la fenêtre et affiche les cellules sous forme de rectangles.
 */
void RenderGrid(GameState* gameState);