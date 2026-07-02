#include "game.h"
#include "deck.h"
#include "structs.h"
#include "turn.h"
#include <string.h>

void game_setup(GameState *game) {
    initialize_deck(game->draw_pile, &game->cards_remaining);
    shuffle_deck(game->draw_pile, game->cards_remaining);

    game->player_count = 4;

    strcpy(game->players[0].player_name, "Player 1");
    game->players[0].player_type = PLAYER;
    game->players[0].hand_card_count = 0;

    strcpy(game->players[1].player_name, "Bot 1");
    game->players[1].player_type = BOT;
    game->players[1].hand_card_count = 0;
    game->players[1].intelligence = 0;

    strcpy(game->players[2].player_name, "Bot 2");
    game->players[2].player_type = BOT;
    game->players[2].hand_card_count = 0;
    game->players[2].intelligence = 50;

    strcpy(game->players[3].player_name, "Bot 3");
    game->players[3].player_type = BOT;
    game->players[3].hand_card_count = 0;
    game->players[3].intelligence = 100;

    for(int player = 0; player < 4; player++){
        for(int i = 0; i < 7; i++) {
            game->players[player].hand[i] = draw_card(game->draw_pile, &game->cards_remaining);
            game->players[player].hand_card_count++;
        }
    }

    game->current_turn = 0;
    game->game_orientation = true;
    game->penalties = 0;

    do {
        game->discard_pile[0] = draw_card(game->draw_pile, &game->cards_remaining);
        if (game->discard_pile[0].color == COLOR_WILD) {
            game->draw_pile[game->cards_remaining] = game->discard_pile[0];
            game->cards_remaining++;
        }
    } while (game->discard_pile[0].color == COLOR_WILD);

    game->discarded_cards = 1;

    if(game->discard_pile[0].card_type == CARD_DRAW_TWO ||
        game->discard_pile[0].card_type == CARD_SKIP ||
        game->discard_pile[0].card_type == CARD_REVERSE) {

            apply_card_effect(game, game->discard_pile[0]);
        }

}

bool is_move_valid(Card chosen_card, Card top_card) {
    bool is_valid = false;
    if((chosen_card.color == top_card.color) ||
        chosen_card.card_type == top_card.card_type ||
        chosen_card.color == COLOR_WILD) {

            is_valid = true;
        }
    return is_valid;
}
