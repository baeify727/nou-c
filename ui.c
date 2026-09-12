#include "ui.h"
#include <ncurses.h>
#include <string.h>
#include <locale.h>

#define MAX_LOG_LINES 16
#define LOG_LINE_LEN 64

static char action_log[MAX_LOG_LINES][LOG_LINE_LEN];
static int log_entry_count = 0;

static int get_color_pair(Color color) {
    switch(color) {
        case GAME_COLOR_RED:
            return 1;
        case GAME_COLOR_BLUE:
            return 2;
        case GAME_COLOR_YELLOW:
            return 3;
        case GAME_COLOR_GREEN:
            return 4;
        default:
            return 5;
    }
}

static const char* get_card_label(CardType type) {
    switch(type) {
        case CARD_ZERO: return " 0 ";
        case CARD_ONE: return " 1 ";
        case CARD_TWO: return " 2 ";
        case CARD_THREE: return " 3 ";
        case CARD_FOUR: return " 4 ";
        case CARD_FIVE: return " 5 ";
        case CARD_SIX: return " 6 ";
        case CARD_SEVEN: return " 7 ";
        case CARD_EIGHT: return " 8 ";
        case CARD_NINE: return " 9 ";
        case CARD_SKIP: return "SKP";
        case CARD_REVERSE: return "REV";
        case CARD_DRAW_TWO: return "+2 ";
        case CARD_WILD: return "WLD";
        case CARD_WILD_DRAW_FOUR: return "+4 ";
        default: return " ? ";
    }
}

static void draw_card_box(int y, int x, Card card, bool is_selected) {
    int pair = get_color_pair(card.color);

    if(is_selected) {
        attron(A_REVERSE);
    }
    attron(COLOR_PAIR(pair) | A_BOLD);

    mvprintw(y,     x, "+-----+");
    mvprintw(y + 1, x, "|     |");
    mvprintw(y + 2, x, "| %s |", get_card_label(card.card_type));
    mvprintw(y + 3, x, "|     |");
    mvprintw(y + 4, x, "+-----+");

    attroff(COLOR_PAIR(pair) | A_BOLD);
    if(is_selected) {
        attroff(A_REVERSE);
    }
}

static void draw_card_back(int y, int x) {
    attron(COLOR_PAIR(5));
    mvprintw(y,     x, "+-----+");
    mvprintw(y + 1, x, "| ### |");
    mvprintw(y + 2, x, "| nou |");
    mvprintw(y + 3, x, "| ### |");
    mvprintw(y + 4, x, "+-----+");
    attroff(COLOR_PAIR(5));
}

void ui_init(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if(has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_GREEN, COLOR_BLACK);
        init_pair(5, COLOR_WHITE, COLOR_BLACK);
    }
}

void ui_cleanup(void) {
    endwin();
}

void ui_add_log(const char *msg) {
    if(log_entry_count < MAX_LOG_LINES) {
        strncpy(action_log[log_entry_count], msg, LOG_LINE_LEN - 1);
        action_log[log_entry_count][LOG_LINE_LEN - 1] = '\0';
        log_entry_count++;
    } else {
        for(int i = 0; i < MAX_LOG_LINES - 1; i++) {
            strcpy(action_log[i], action_log[i + 1]);
        }
        strncpy(action_log[MAX_LOG_LINES - 1], msg, LOG_LINE_LEN - 1);
        action_log[MAX_LOG_LINES - 1][LOG_LINE_LEN - 1] = '\0';
    }
}

void ui_register_profile(PlayerProfile *profile) {
    clear();
    echo();
    curs_set(1);

    mvprintw(7, 10, "========================================");
    mvprintw(8, 10, "             NUOVO PROFILO          ");
    mvprintw(9, 10, "========================================");
    mvprintw(11, 10, "Inserisci il tuo nome: ");
    refresh();

    getnstr(profile->player_name, MAX_STR_LEN - 1);
    profile->games_played = 0;
    profile->games_won = 0;
    profile->card_played = 0;

    noecho();
    curs_set(0);
}

int ui_main_menu(bool has_save) {
    const char *options[] = {
        "Nuova Partita",
        "Continua Partita",
        "Statistiche Profilo",
        "Esci"
    };
    int total_options = 4;
    int selected = 0;

    const char *ascii_logo[] = {
        " ⣴⡦⠤⣤⣀⣤⣶⠶⠶⢤⣄     ⢀⣤⣴⠶⠶⠶⠦⣤⡀           ⢰⡶⠤⢤⣶   ⢰⣶⠤⣴⡆",
        "⠘⢸⠃ ⢿⣯⣧⢤⣤⡀ ⠙⣧  ⢀⣴⠏⠻⣿⣶⣤⣤⣇ ⠻⣷⡀        ⢸⡇ ⢀⣿   ⣸⡟ ⢸⡇",
        " ⢺  ⢰⠟⠁ ⠈⣧  ⢸  ⣼⠃ ⢰⠟⠉⡃⠈⠻⣦ ⠈⣷        ⢸⡇ ⢸⣿   ⢸⡇ ⢸⡇",
        " ⣸  ⣺    ⣿   ⢸  ⡏ ⢸⣿    ⠲⣿  ⢹⡄       ⢸⡇ ⢸⣿    ⡇ ⢸⡇",
        " ⢻⡀ ⣿    ⣿  ⢸  ⢿⡀⠸⢹⣦    ⢀⡿ ⢀⣿⠁      ⢸⣇  ⣿    ⣼⠇ ⢸⡇",
        " ⢸⡆ ⣿    ⣿  ⣸   ⠘⢷⡄ ⠙⠳⠦⠶⠋ ⣀⣾⠃       ⠈⣿⣤⡄⠙⢷⣤⣾⣯⣤ ⢸⡇",
        " ⢸⣿⣦⣿⠂   ⣿⣿⣿⣿    ⠙⠳⣦⣤⣤⣤⣼⠿⠛           ⠈⠻⣧⣤⣭⣬⠿⠛⣿⣾⣿⠇",
    };
    int logo_lines = 7;
    int menu_start_x = 10;
    int menu_start_y = 10;

    while(true) {
        clear();

        for(int i = 0; i < logo_lines; i++) {
            mvprintw(2 + i, 2, "%s", ascii_logo[i]);
        }

        for(int i = 0; i < total_options; i++) {
            if(i == 1 && !has_save) {
                attron(A_DIM);
                mvprintw(menu_start_y + i, menu_start_x, "  [X] %s (Nessun salvataggio)", options[i]);
                attroff(A_DIM);
                continue;
            }

            if(i == selected) {
                attron(A_REVERSE);
                mvprintw(menu_start_y + i, menu_start_x, "  ->  %s  ", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(menu_start_y + i, menu_start_x, "      %s  ", options[i]);
            }
        }

        mvprintw(menu_start_y + total_options + 2, menu_start_x, "[Frecce per spostarsi, Enter per selezionare]");
        refresh();

        int ch = getch();
        if(ch == KEY_UP) {
            if(selected > 0) {
                selected--;
                if(selected == 1 && !has_save) {
                    selected--;
                }
            }
        } else if(ch == KEY_DOWN) {
            if(selected < total_options - 1) {
                selected++;
                if(selected == 1 && !has_save) {
                    selected++;
                }
            }
        } else if(ch == '\n' || ch == KEY_ENTER) {
            return selected;
        }
    }
}

void ui_show_stats(PlayerProfile *profile) {
    clear();
    mvprintw(6, 10, "========================================");
    mvprintw(7, 10, "          STATISTICHE PROFILO           ");
    mvprintw(8, 10, "========================================");
    mvprintw(10, 10, "Giocatore:        %s", profile->player_name);
    mvprintw(11, 10, "Partite giocate:  %d", profile->games_played);
    mvprintw(12, 10, "Partite vinte:    %d", profile->games_won);
    mvprintw(13, 10, "Carte giocate:    %d", profile->card_played);

    float winrate = 0.0f;
    if(profile->games_played > 0) {
        winrate = ((float)profile->games_won / profile->games_played) * 100.0f;
    }
    mvprintw(14, 10, "Win rate:  %.1f%%", winrate);

    mvprintw(17, 10, "Premi un tasto per tornare al menu...");
    refresh();
    getch();
}

void ui_render_board(GameState *game, int selected_index) {
    clear();


    for(int r = 0; r < 25; r++) {
        mvprintw(r, 66, "|");
    }
    mvprintw(1, 69, "=== LOG PARTITA ===");
    for(int i = 0; i < log_entry_count; i++) {
        mvprintw(3 + i, 69, "%s", action_log[i]);
    }


    Player *top_bot = &game->players[2];
    mvprintw(1, 25, " %s: %d carte ", top_bot->player_name, top_bot->hand_card_count);
    if(game->current_turn == 2) {
        mvprintw(2, 27, ">> TURNO <<");
    }


    Player *left_bot = &game->players[1];
    mvprintw(8, 2, "%s", left_bot->player_name);
    mvprintw(9, 2, "%d carte", left_bot->hand_card_count);
    if(game->current_turn == 1) {
        mvprintw(10, 2, ">> TURNO <<");
    }


    Player *right_bot = &game->players[3];
    mvprintw(8, 52, "%s", right_bot->player_name);
    mvprintw(9, 52, "%d carte", right_bot->hand_card_count);
    if(game->current_turn == 3) {
        mvprintw(10, 52, ">> TURNO <<");
    }


    draw_card_back(6, 23);
    mvprintw(11, 23, "Mazzo: %d", game->cards_remaining);

    if(game->discarded_cards > 0) {
        Card top_card = game->discard_pile[game->discarded_cards - 1];
        draw_card_box(6, 34, top_card, false);
        mvprintw(11, 35, "Scarti");
    }


    mvprintw(13, 20, "Verso: %s",
             game->game_orientation ? "Orario (->)" : "Antiorario (<-)");


    Player *player = &game->players[0];
    mvprintw(15, 2, "Tua Mano: %s (%d carte)", player->player_name, player->hand_card_count);
    if(game->current_turn == 0) {
        mvprintw(15, 45, ">> IL TUO TURNO <<");
    }

    int start_x = 2;
        int max_visible = 8;
        int scroll_offset = 0;
        if(selected_index >= max_visible) {
            scroll_offset = selected_index - max_visible + 1;
        }

        if (scroll_offset > 0) {
            mvprintw(17, 0, "<");
        } else {
            mvprintw(17, 0, " ");
        }

        for(int i = 0; i < max_visible && (i + scroll_offset) < player->hand_card_count; i++) {
            int card_idx = i + scroll_offset;
            bool is_sel = (card_idx == selected_index) && (game->current_turn == 0);
            int pos_y = is_sel ? 16 : 17;
            draw_card_box(pos_y, start_x + (i * 8), player->hand[card_idx], is_sel);
        }

        if (scroll_offset + max_visible < player->hand_card_count) {
            mvprintw(17, 65, ">");
        } else {
            mvprintw(17, 65, " ");
        }

    mvprintw(23, 2, "[Frecce]: Scorri | [Invio]: Gioca | [D]: Pesca | [S]: Salva");
    mvprintw(24, 2, "[U]: NOU! | [C] Call out");
    refresh();
}

Color ui_prompt_wild_color(void) {
    mvprintw(25, 2, "Colore carta: [1] Rosso | [2] Blu | [3] Giallo | [4] Verde: ");
    refresh();

    while(true) {
        int ch = getch();
        if(ch == '1') return GAME_COLOR_RED;
        if(ch == '2') return GAME_COLOR_BLUE;
        if(ch == '3') return GAME_COLOR_YELLOW;
        if(ch == '4') return GAME_COLOR_GREEN;
    }
}

void ui_show_winner(GameState *game, int winner_index) {
    clear();
    mvprintw(8, 12, "========================================");
    mvprintw(9, 12, "           PARTITA CONCLUSA             ");
    mvprintw(10, 12, "========================================");
    mvprintw(12, 12, "Vincitore: %s!", game->players[winner_index].player_name);
    mvprintw(15, 12, "Premi un tasto per continuare...");
    refresh();
    getch();
}

void ui_configure_bots(GameState *game) {
    int selected = 0;
    int total_rows = 4;

    while(true) {
        clear();
        mvprintw(4, 12, "=============================");
        mvprintw(5, 12, "      CONFIGURAZIONE BOT     ");
        mvprintw(6, 12, "=============================");

        for(int i = 0; i < 3; i++) {
            char buf[64];
            snprintf(buf, sizeof(buf), "%s (Intelligenza: %3d)", game->players[i + 1].player_name, game->players[i + 1].intelligence);
            if(selected == i) {
                attron(A_REVERSE);
                mvprintw(8 + i, 12, "  ->  %-30s  ", buf);
                attroff(A_REVERSE);
            } else {
                mvprintw(8 + i, 12, "      %-30s  ", buf);
            }
        }

        if(selected == 3) {
            attron(A_REVERSE);
            mvprintw(12, 12, "  ->  Avvia Partita  ");
            attroff(A_REVERSE);
        } else {
            mvprintw(12, 12, "      Avvia Partita  ");
        }

        mvprintw(15, 12, "[Frecce]: Seleziona | [dx/sx]: +/- 5 | [Enter]: Conferma");
        refresh();

        int ch = getch();
        if(ch == KEY_UP) {
            if(selected > 0) selected--;
        } else if(ch == KEY_DOWN) {
            if(selected < total_rows - 1) selected++;
        } else if(ch == KEY_RIGHT) {
            if(selected < 3) {
                game->players[selected + 1].intelligence += 5;
                if(game->players[selected + 1].intelligence > 100)
                    game->players[selected + 1].intelligence = 100;
            }
        } else if(ch == KEY_LEFT) {
            if(selected < 3) {
                game->players[selected + 1].intelligence -= 5;
                if(game->players[selected + 1].intelligence < 0)
                    game->players[selected + 1].intelligence = 0;
            }
        } else if(ch == '\n' || ch == KEY_ENTER) {
            if(selected == 3) {
                break;
            }
        }
    }
}

const char* color_to_string(Color c) {
    switch(c) {
        case GAME_COLOR_RED: return "Rosso";
        case GAME_COLOR_BLUE: return "Blu";
        case GAME_COLOR_YELLOW: return "Giallo";
        case GAME_COLOR_GREEN: return "Verde";
        default: return "Speciale";
    }
}

const char* type_to_string(CardType t) {
    switch(t) {
        case CARD_ZERO: return "0";
        case CARD_ONE: return "1";
        case CARD_TWO: return "2";
        case CARD_THREE: return "3";
        case CARD_FOUR: return "4";
        case CARD_FIVE: return "5";
        case CARD_SIX: return "6";
        case CARD_SEVEN: return "7";
        case CARD_EIGHT: return "8";
        case CARD_NINE: return "9";
        case CARD_SKIP: return "Stop";
        case CARD_REVERSE: return "Cambio";
        case CARD_DRAW_TWO: return "+2";
        case CARD_WILD: return "Jolly";
        case CARD_WILD_DRAW_FOUR: return "+4";
        default: return "?";
    }
}
