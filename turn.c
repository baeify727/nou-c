#include "turn.h"
#include "structs.h"

void next_turn(GameState *game) {
    if(game->game_orientation == true) {
        game->current_turn = (game->current_turn + 1) % game->player_count;
    } else {
        if(game->current_turn > 0){
                    game->current_turn--;
        } else {
            game->current_turn = game->player_count - 1;
        }
    }
}

void apply_card_effect(GameState *game, Card played_card) {
    switch(played_card.card_type) {
        case CARD_SKIP:
            next_turn(game);
            break;
        case CARD_REVERSE:
            game->game_orientation = !game->game_orientation;
            break;
        case CARD_DRAW_TWO:
            game->penalties += 2;
            break;
        case CARD_WILD_DRAW_FOUR:
            game->penalties += 4;
            break;
        default:
            break;
    }
}
