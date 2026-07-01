#ifndef GAME_H
#define GAME_H

#include "deck.h"
#include "structs.h"
#include <stdbool.h>

void game_setup(GameState *game);
bool is_move_valid(Card chosen_card, Card top_card);

#endif
