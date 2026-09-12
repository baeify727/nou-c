/**
 * @file main.c
 * @brief Punto di ingresso principale e gestione del ciclo di gioco.
 */

#include "structs.h"
#include "game.h"
#include "turn.h"
#include "bot.h"
#include "storage.h"
#include "ui.h"
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Esegue il ciclo principale della partita, gestendo i turni, gli input dell'utente e le mosse dei bot.
 * @param game Puntatore allo stato globale della partita.
 * @param profile Puntatore al profilo del giocatore umano per l'aggiornamento delle statistiche.
 */
static void play_game(GameState *game, PlayerProfile *profile) {
    int selected_card = 0;
    bool in_game = true;
    int winner = -1;
    char log_entry[64];
    bool has_drawn = false;
    int last_turn = -1;

    ui_add_log("Partita avviata");

    while(in_game) {
        for(int i = 0; i < game->player_count; i++) {
            if(game->players[i].hand_card_count == 0) {
                winner = i;
                in_game = false;
                break;
            }
        }
        if(!in_game) {
            break;
        }

        ui_render_board(game, selected_card);

        if(game->current_turn == 0) {
            if(game->current_turn != last_turn) {
                has_drawn = false;
                last_turn = game->current_turn;
            }

            if(game->penalties > 0) {
                apply_penalties_if_any(game, 0);
                continue;
            }

            int ch = getch();
            if(ch == KEY_LEFT) {
                if(selected_card > 0) {
                    selected_card--;
                }
            } else if(ch == KEY_RIGHT) {
                if(selected_card < game->players[0].hand_card_count - 1) {
                    selected_card++;
                }
            } else if(ch == 'u' || ch == 'U') {
                if(game->players[0].hand_card_count == 2 || game->players[0].hand_card_count == 1) {
                    game->players[0].said_uno = true;
                    ui_add_log("Tu: NOU!");
                }
            } else if(ch == 'c' || ch == 'C') {
                bool caught = false;
                for(int b = 1; b < game->player_count; b++) {
                    if(game->players[b].hand_card_count == 1 && !game->players[b].said_uno) {
                        char msg[64];
                        snprintf(msg, sizeof(msg), "Hai fatto un callout a %s! +2 carte", game->players[b].player_name);
                        ui_add_log(msg);
                        for(int p = 0; p < 2; p++) {
                            draw_from_pile(game, b);
                        }
                        caught = true;
                        break;
                    }
                }
                if(!caught) {
                    ui_add_log("Nessuno ha fatto una mossa sbagliata.");
                }
            } else if(ch == 'd' || ch == 'D') {
                if(!has_drawn) {
                    draw_from_pile(game, 0);
                    has_drawn = true;
                    Player *player = &game->players[0];
                    Card drawn = player->hand[player->hand_card_count - 1];
                    Card top = game->discard_pile[game->discarded_cards - 1];

                    ui_add_log("Tu: Hai pescato");

                    if(is_move_valid(drawn, top)) {
                        ui_add_log("Carta giocabile! [Invio] per giocarla o [P] per passare");
                        selected_card = player->hand_card_count - 1;
                    } else {
                        ui_add_log("Carta non giocabile. Premi [P] per passare.");
                    }
                } else {
                    ui_add_log("Hai gia' pescato in questo turno!");
                }
            } else if(ch == 'p' || ch == 'P') {
                if(has_drawn) {
                    ui_add_log("Tu: Hai passato il turno");
                    has_drawn = false;
                    next_turn(game);
                } else {
                    ui_add_log("Devi prima pescare una carta!");
                }
            } else if(ch == 's' || ch == 'S') {
                save_game(game, SAVE_FILE);
                save_profile(profile, PROFILE_FILE);
                ui_add_log("Partita salvata");
                in_game = false;
            } else if(ch == '\n' || ch == KEY_ENTER) {
                Player *player = &game->players[0];
                if(player->hand_card_count > 0) {
                    Card chosen = player->hand[selected_card];
                    Card top = game->discard_pile[game->discarded_cards - 1];

                    if(is_move_valid(chosen, top)) {
                        for(int i = selected_card; i < player->hand_card_count - 1; i++) {
                            player->hand[i] = player->hand[i + 1];
                        }
                        player->hand_card_count--;

                        if(selected_card >= player->hand_card_count && selected_card > 0) {
                            selected_card--;
                        }

                        if(chosen.card_type == CARD_WILD || chosen.card_type == CARD_WILD_DRAW_FOUR) {
                            Color new_color = ui_prompt_wild_color();
                            chosen.color = new_color;
                            snprintf(log_entry, sizeof(log_entry), "Tu: %s %s",
                                     color_to_string(new_color), type_to_string(chosen.card_type));
                        } else {
                            snprintf(log_entry, sizeof(log_entry), "Tu: %s %s",
                                     color_to_string(chosen.color), type_to_string(chosen.card_type));
                        }

                        game->discard_pile[game->discarded_cards] = chosen;
                        game->discarded_cards++;
                        profile->card_played++;
                        ui_add_log(log_entry);

                        if(player->hand_card_count == 1) {
                            if(!player->said_uno) {
                                ui_add_log("Non hai detto NOU! +2 carte");
                                draw_from_pile(game, 0);
                                draw_from_pile(game, 0);
                            }
                        } else {
                            player->said_uno = false;
                        }

                        apply_card_effect(game, chosen);
                        has_drawn = false;
                        next_turn(game);
                    } else {
                        ui_add_log("Mossa non valida!");
                    }
                }
            }
        } else {
            int bot_idx = game->current_turn;

            ui_render_board(game, selected_card);
            napms(2000);

            if(game->penalties > 0) {
                apply_penalties_if_any(game, bot_idx);
                continue;
            }

            int move = bot_choose_move(game, bot_idx);

            if(move >= 0 && move < game->players[bot_idx].hand_card_count) {
                Player *bot = &game->players[bot_idx];
                Card chosen = bot->hand[move];

                for(int i = move; i < bot->hand_card_count - 1; i++) {
                    bot->hand[i] = bot->hand[i + 1];
                }
                bot->hand_card_count--;

                if(bot->hand_card_count == 1) {
                    int roll = rand() % 100;
                    if(roll < bot->intelligence) {
                        bot->said_uno = true;
                        snprintf(log_entry, sizeof(log_entry), "%s: NOU!", bot->player_name);
                        ui_add_log(log_entry);
                    } else {
                        bot->said_uno = false;
                    }
                } else {
                    bot->said_uno = false;
                }

                if(chosen.card_type == CARD_WILD || chosen.card_type == CARD_WILD_DRAW_FOUR) {
                    Color chosen_color = bot_choose_wild_card_color(game, bot_idx);
                    chosen.color = chosen_color;
                    snprintf(log_entry, sizeof(log_entry), "%s: %s %s",
                             bot->player_name, color_to_string(chosen_color), type_to_string(chosen.card_type));
                } else {
                    snprintf(log_entry, sizeof(log_entry), "%s: %s %s",
                             bot->player_name, color_to_string(chosen.color), type_to_string(chosen.card_type));
                }

                game->discard_pile[game->discarded_cards] = chosen;
                game->discarded_cards++;
                ui_add_log(log_entry);

                apply_card_effect(game, chosen);
            } else {
                draw_from_pile(game, bot_idx);
                Player *bot = &game->players[bot_idx];
                Card drawn = bot->hand[bot->hand_card_count - 1];
                Card top = game->discard_pile[game->discarded_cards - 1];

                snprintf(log_entry, sizeof(log_entry), "%s: Ha pescato", bot->player_name);
                ui_add_log(log_entry);

                if(is_move_valid(drawn, top) && (rand() % 100 < bot->intelligence)) {
                    bot->hand_card_count--;

                    if(bot->hand_card_count == 1) {
                        int roll = rand() % 100;
                        if(roll < bot->intelligence) {
                            bot->said_uno = true;
                            snprintf(log_entry, sizeof(log_entry), "%s: NOU!", bot->player_name);
                            ui_add_log(log_entry);
                        } else {
                            bot->said_uno = false;
                        }
                    } else {
                        bot->said_uno = false;
                    }

                    if(drawn.card_type == CARD_WILD || drawn.card_type == CARD_WILD_DRAW_FOUR) {
                        Color chosen_color = bot_choose_wild_card_color(game, bot_idx);
                        drawn.color = chosen_color;
                        snprintf(log_entry, sizeof(log_entry), "%s gioca pescata: %s %s",
                                 bot->player_name, color_to_string(chosen_color), type_to_string(drawn.card_type));
                    } else {
                        snprintf(log_entry, sizeof(log_entry), "%s gioca pescata: %s %s",
                                 bot->player_name, color_to_string(drawn.color), type_to_string(drawn.card_type));
                    }

                    game->discard_pile[game->discarded_cards] = drawn;
                    game->discarded_cards++;
                    ui_add_log(log_entry);

                    apply_card_effect(game, drawn);
                }
            }

            ui_render_board(game, selected_card);
            napms(2000);

            next_turn(game);
        }
    }

    if(winner != -1) {
        ui_render_board(game, selected_card);
        napms(3000);

        profile->games_played++;
        if(winner == 0) {
            profile->games_won++;
        }
        save_profile(profile, PROFILE_FILE);
        remove(SAVE_FILE);
        ui_show_winner(game, winner);
    }
}

/**
 * @brief Punto di ingresso del programma: inizializza l'ambiente curses, gestisce il profilo e il menu principale.
 * @return int Codice di terminazione dell'applicazione (0 su uscita corretta).
 */
int main(void) {
    PlayerProfile profile;
    GameState game;
    srand(time(NULL));

    ui_init();

    if(!load_profile(&profile, PROFILE_FILE)) {
        ui_register_profile(&profile);
        save_profile(&profile, PROFILE_FILE);
    }

    bool app_running = true;
    while(app_running) {
        bool has_save = save_game_exists(SAVE_FILE);
        int choice = ui_main_menu(has_save);

        switch(choice) {
            case 0:
                game_setup(&game);
                ui_configure_bots(&game);
                strncpy(game.players[0].player_name, profile.player_name, MAX_STR_LEN - 1);
                game.players[0].player_name[MAX_STR_LEN - 1] = '\0';
                play_game(&game, &profile);
                break;

            case 1:
                if(has_save && load_game(&game, SAVE_FILE)) {
                    play_game(&game, &profile);
                }
                break;

            case 2:
                ui_show_stats(&profile);
                break;

            case 3:
                app_running = false;
                break;

            default:
                break;
        }
    }

    ui_cleanup();
    return 0;
}
