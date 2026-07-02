#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#define MAX_STR_LEN 15
#define DECK 108

/**
 * Enum che racchiude tutti i colori delle carte, da rosso a speciale.
 */
typedef enum Color {
    COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_WILD
} Color;

/*
 * Enum che descrive il tipo che una certa carta può assumere: numero da 0 a 9, e carte speciali come il +2 o il cambio colore.
 */
typedef enum CardType {
    CARD_ZERO, CARD_ONE, CARD_TWO, CARD_THREE, CARD_FOUR, CARD_FIVE, CARD_SIX, CARD_SEVEN, CARD_EIGHT, CARD_NINE,
    CARD_SKIP, CARD_REVERSE, CARD_DRAW_TWO, CARD_WILD, CARD_WILD_DRAW_FOUR
} CardType;

/*
 * Enum che descrive il tipo del giocatore, che può essere un umano (player) o bot.
 */
typedef enum PlayerType {
    PLAYER, BOT
} PlayerType;

typedef enum BotState {
    BOT_STATE_CLUELESS, BOT_STATE_STANDARD, BOT_STATE_AGGRESSIVE, BOT_STATE_PANIC, BOT_STATE_WIN_CHASE
} BotState;

/*
 * Struttura della singola carta, che possiede un colore e un tipo.
 */
typedef struct Card {
    Color color;
    CardType card_type;
} Card;

/*
 * Profilo del giocatore per mantenere stats in modo permanente.
 */
typedef struct PlayerProfile {
    char player_name[MAX_STR_LEN];
    int games_played;
    int games_won;
    int card_played;
} PlayerProfile;

/*
 * Struttura di un giocatore attualmente in campo. Ha un nome, la sua mano attuale, e un TipoGiocatore.
 */
typedef struct Player {
    char player_name[MAX_STR_LEN];
    Card hand[DECK];
    int hand_card_count;
    PlayerType player_type;
    int intelligence;
    BotState bot_state;
} Player;

/*
 * Stato della partita attuale, per salvare eventualmente una partita quando è in corso, o semplicemente per fare dei check in tempo reale.
 */
typedef struct GameState {
    Player players[4];
    int player_count;
    int current_turn;          /*< Indice per indicare di che giocatore è il turno attuale, da 0 a numero_giocatori - 1. */
    Card draw_pile[DECK];
    int cards_remaining;
    Card discard_pile[DECK];
    int discarded_cards;
    bool game_orientation;           /*< True per senso orario, false per senso antiorario. */
    int penalties;
} GameState;

#endif
