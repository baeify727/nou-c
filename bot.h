#ifndef BOT_H
#define BOT_H

#include "structs.h"

int bot_choose_move(GameState *game, int bot_index);
Color bot_choose_wild_card_color(GameState *game, int bot_index);

#endif
