#ifndef TURN_H
#define TURN_H

#include "structs.h"

void next_turn(GameState *game);
void apply_card_effect(GameState *game, Card played_card);


#endif
