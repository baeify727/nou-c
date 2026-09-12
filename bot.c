#include "bot.h"
#include "structs.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

static int bot_logic(GameState *game, int bot_index);
static bool has_wild_card(GameState *game, int bot_index);

int bot_choose_move(GameState *game, int bot_index) {
    Player *bot = &game->players[bot_index];
    int int_needed = rand() % 100;
    bot->bot_state = BOT_STATE_STANDARD;

    if(bot->intelligence < int_needed) {
        bot->bot_state = BOT_STATE_CLUELESS;
    }

    return bot_logic(game, bot_index);
}

Color bot_choose_wild_card_color(GameState *game, int bot_index) {
    Player *bot = &game->players[bot_index];

    if(bot->bot_state == BOT_STATE_CLUELESS) {
        return (Color)(rand() % 4);
    }

    int color_count[4] = {0};
    for(int card_index = 0; card_index < bot->hand_card_count; card_index++) {
        if(bot->hand[card_index].color < GAME_COLOR_WILD) {
            color_count[bot->hand[card_index].color]++;
        }
    }

    Color highest_color_count = GAME_COLOR_RED;
    int max_count = color_count[0];
    for(int i = 1; i < 4; i++) {
        if(color_count[i] > max_count) {
            max_count = color_count[i];
            highest_color_count = (Color)i;
        }
    }

    return highest_color_count;
}

static bool has_wild_card(GameState *game, int bot_index) {
    bool has_wild = false;
    Player *bot = &game->players[bot_index];

    for(int card_index = 0; card_index < bot->hand_card_count; card_index++) {
        if(bot->hand[card_index].color == GAME_COLOR_WILD) {
            has_wild = true;
        }
    }

    return has_wild;
}

static int bot_logic(GameState *game, int bot_index) {
    Player *bot = &game->players[bot_index];
    Card top_card = game->discard_pile[game->discarded_cards - 1];

    if(bot->bot_state == BOT_STATE_CLUELESS) {
        for(int hand_index = 0; hand_index < bot->hand_card_count; hand_index++) {
            if(is_move_valid(bot->hand[hand_index], top_card)) {
                return hand_index;
            }
        }
        return -1;
    }

    int next_turn_index;
    if(game->game_orientation == true) {
        next_turn_index = (game->current_turn + 1) % game->player_count;
    } else {
        if(game->current_turn > 0) {
            next_turn_index = (game->current_turn - 1);
        } else {
            next_turn_index = game->player_count - 1;
        }
    }
    Player next_player = game->players[next_turn_index];

    int previous_turn_index;
    if(game->game_orientation == true) {
        if(game->current_turn > 0) {
            previous_turn_index = (game->current_turn - 1);
        } else {
            previous_turn_index = game->player_count - 1;
        }
    } else {
        previous_turn_index = (game->current_turn + 1) % game->player_count;
    }
    Player previous_player = game->players[previous_turn_index];

    int lowest_card_count = 999;
    int dangerous_player_index = -1;
    bool anyone_at_uno = false;
    for(int player_index = 0; player_index < game->player_count; player_index++) {
        if(player_index == bot_index) {
            continue;
        }
        if(game->players[player_index].hand_card_count < lowest_card_count) {
            lowest_card_count = game->players[player_index].hand_card_count;
            dangerous_player_index = player_index;
        }
        if(game->players[player_index].hand_card_count == 1) {
            anyone_at_uno = true;
            dangerous_player_index = player_index;
        }
    }

    int *valid_neutral_move_indexes = malloc(sizeof(int));
    if(valid_neutral_move_indexes == NULL) {
        return -2;
    }
    int valid_neutral_moves_count = 0;

    int *valid_attack_move_indexes = malloc(sizeof(int));
    if(valid_attack_move_indexes == NULL) {
        free(valid_neutral_move_indexes);
        return -3;
    }
    int valid_attack_moves_count = 0;

    for(int card_index = 0; card_index < bot->hand_card_count; card_index++) {
        if(is_move_valid(bot->hand[card_index], top_card) && bot->hand[card_index].color != GAME_COLOR_WILD && bot->hand[card_index].card_type != CARD_DRAW_TWO
            && bot->hand[card_index].card_type != CARD_SKIP && bot->hand[card_index].card_type != CARD_REVERSE) {
            if(valid_neutral_moves_count == 0) {
                valid_neutral_move_indexes[0] = card_index;
                valid_neutral_moves_count++;
            } else {
                int *temp = realloc(valid_neutral_move_indexes, (valid_neutral_moves_count + 1) * sizeof(int));
                if(temp == NULL) {
                    free(valid_neutral_move_indexes);
                    free(valid_attack_move_indexes);
                    return -2;
                }
                valid_neutral_move_indexes = temp;
                valid_neutral_move_indexes[valid_neutral_moves_count] = card_index;
                valid_neutral_moves_count++;
            }
        } else if(is_move_valid(bot->hand[card_index], top_card) && (bot->hand[card_index].color == GAME_COLOR_WILD || bot->hand[card_index].card_type == CARD_DRAW_TWO
                    || bot->hand[card_index].card_type == CARD_SKIP || bot->hand[card_index].card_type == CARD_REVERSE)) {
            if(valid_attack_moves_count == 0) {
                valid_attack_move_indexes[0] = card_index;
                valid_attack_moves_count++;
            } else {
                int *temp = realloc(valid_attack_move_indexes, (valid_attack_moves_count + 1) * sizeof(int));
                if(temp == NULL) {
                    free(valid_neutral_move_indexes);
                    free(valid_attack_move_indexes);
                    return -3;
                }
                valid_attack_move_indexes = temp;
                valid_attack_move_indexes[valid_attack_moves_count] = card_index;
                valid_attack_moves_count++;
            }
        }
    }

    if(bot->hand_card_count <= 2) {
        bot->bot_state = BOT_STATE_WIN_CHASE;
    } else if(next_player.hand_card_count <= 2) {
        bot->bot_state = BOT_STATE_AGGRESSIVE;
    } else if(anyone_at_uno == true || lowest_card_count <= 2) {
        bot->bot_state = BOT_STATE_PANIC;
    } else {
        bot->bot_state = BOT_STATE_STANDARD;
    }

    int chosen_move = -1;
    switch(bot->bot_state) {
        case BOT_STATE_WIN_CHASE: {
            if(has_wild_card(game, bot_index)) {
                int wild_card_index = -1;
                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].color == GAME_COLOR_WILD) {
                        wild_card_index = valid_attack_move_indexes[i];
                    }
                }
                if(wild_card_index != -1) {
                    chosen_move = wild_card_index;
                    break;
                }
            }

            if(chosen_move != -1) break;

            for(int i = 0; i < valid_attack_moves_count; i++) {
                if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_SKIP || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_REVERSE
                    || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_DRAW_TWO) {
                    chosen_move = valid_attack_move_indexes[i];
                    break;
                }
            }

            if(chosen_move != -1) break;

            if(valid_neutral_moves_count > 0) {
                chosen_move = valid_neutral_move_indexes[0];
                break;
            }
            break;
        }

        case BOT_STATE_AGGRESSIVE: {
            if(valid_attack_moves_count > 0) {
                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_WILD_DRAW_FOUR) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;

                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_DRAW_TWO || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_SKIP) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;

                if(previous_player.hand_card_count > 2) {
                    for(int i = 0; i < valid_attack_moves_count; i++) {
                        if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_REVERSE) {
                            chosen_move = valid_attack_move_indexes[i];
                            break;
                        }
                    }
                }

                if(chosen_move != -1) break;

                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].color == GAME_COLOR_WILD) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;
            }

            if(valid_neutral_moves_count > 0) {
                chosen_move = valid_neutral_move_indexes[0];
                break;
            }
            break;
        }

        case BOT_STATE_PANIC: {
            if(valid_attack_moves_count > 0) {
                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_WILD_DRAW_FOUR) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;

                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].color == GAME_COLOR_WILD) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;

                if(previous_player.hand_card_count > 2) {
                    for(int i = 0; i < valid_attack_moves_count; i++) {
                        if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_REVERSE) {
                            chosen_move = valid_attack_move_indexes[i];
                            break;
                        }
                    }
                }
            }

            if(chosen_move != -1) break;

            if(valid_neutral_moves_count > 0) {
                chosen_move = valid_neutral_move_indexes[0];
                break;
            }

            if(chosen_move != -1) break;

            if(valid_attack_moves_count > 0) {
                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_SKIP || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_DRAW_TWO) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }
            }
            break;
        }

        case BOT_STATE_STANDARD: {
            if(valid_neutral_moves_count > 0) {
                chosen_move = valid_neutral_move_indexes[0];
                break;
            }

            if(chosen_move != -1) break;

            if(valid_attack_moves_count > 0) {
                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_SKIP || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_DRAW_TWO
                        || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_REVERSE) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }

                if(chosen_move != -1) break;

                for(int i = 0; i < valid_attack_moves_count; i++) {
                    if(bot->hand[valid_attack_move_indexes[i]].card_type == CARD_WILD || bot->hand[valid_attack_move_indexes[i]].card_type == CARD_WILD_DRAW_FOUR) {
                        chosen_move = valid_attack_move_indexes[i];
                        break;
                    }
                }
            }
            break;
        }
    }

    free(valid_attack_move_indexes);
    free(valid_neutral_move_indexes);
    return chosen_move;
}
