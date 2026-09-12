/**
 * @file structs.h
 * @brief Definizioni delle strutture dati, enumerazioni e costanti globali del gioco.
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

#define MAX_STR_LEN 15 /**< Lunghezza massima per stringhe di testo (es. nomi giocatori) */
#define DECK 108        /**< Dimensione standard del mazzo */

/**
 * @brief Colori disponibili per le carte.
 */
typedef enum {
    GAME_COLOR_RED,     /**< Colore Rosso */
    GAME_COLOR_BLUE,    /**< Colore Blu */
    GAME_COLOR_YELLOW,  /**< Colore Giallo */
    GAME_COLOR_GREEN,   /**< Colore Verde */
    GAME_COLOR_WILD     /**< Carta speciale senza colore iniziale (Jolly / +4) */
} Color;

/**
 * @brief Valori numerici ed effetti speciali delle carte.
 */
typedef enum CardType {
    CARD_ZERO, CARD_ONE, CARD_TWO, CARD_THREE, CARD_FOUR,
    CARD_FIVE, CARD_SIX, CARD_SEVEN, CARD_EIGHT, CARD_NINE,
    CARD_SKIP,           /**< Salta turno */
    CARD_REVERSE,        /**< Inverti verso di gioco */
    CARD_DRAW_TWO,       /**< Pesca due carte (+2) */
    CARD_WILD,           /**< Jolly cambia colore */
    CARD_WILD_DRAW_FOUR  /**< Pesca quattro carte (+4) */
} CardType;

/**
 * @brief Tipologia di giocatore.
 */
typedef enum PlayerType {
    PLAYER, /**< Giocatore umano */
    BOT     /**< Giocatore controllato dal computer */
} PlayerType;

/**
 * @brief Stato psicologico/strategico del bot in base all'andamento della mano.
 */
typedef enum BotState {
    BOT_STATE_CLUELESS,   /**< Gioca a caso senza strategia */
    BOT_STATE_STANDARD,   /**< Giocata standard bilanciata */
    BOT_STATE_AGGRESSIVE, /**< Tenta di ostacolare il giocatore successivo */
    BOT_STATE_PANIC,      /**< Gioca carte di contrasto rapido per fermare chi è a UNO */
    BOT_STATE_WIN_CHASE   /**< Bot a poche carte: cerca la chiusura veloce */
} BotState;

/**
 * @brief Rappresentazione di una singola carta di gioco.
 */
typedef struct Card {
    Color color;          /**< Colore assegnato alla carta */
    CardType card_type;   /**< Tipologia o valore numerico della carta */
} Card;

/**
 * @brief Statistiche persistenti salvate per il giocatore umano.
 */
typedef struct PlayerProfile {
    char player_name[MAX_STR_LEN]; /**< Nome utente registrato */
    int games_played;              /**< Partite totali disputate */
    int games_won;                 /**< Partite vinte */
    int card_played;               /**< Carte totali giocate */
} PlayerProfile;

/**
 * @brief Dati associati a un singolo giocatore al tavolo.
 */
typedef struct Player {
    char player_name[MAX_STR_LEN]; /**< Nome identificativo a schermo */
    Card hand[DECK];               /**< Carte attualmente tenute in mano */
    int hand_card_count;           /**< Numero effettivo di carte in mano */
    PlayerType player_type;        /**< Ruolo (umano o bot) */
    int intelligence;              /**< Livello intelligenza percentuale (0 - 100) */
    BotState bot_state;            /**< Stato euristico corrente */
    bool said_uno;                 /**< Flag che traccia se il giocatore ha dichiarato UNO */
} Player;

/**
 * @brief Stato globale di una sessione di gioco.
 */
typedef struct GameState {
    Player players[4];             /**< Partecipanti alla partita */
    int player_count;              /**< Numero complessivo di giocatori al tavolo */
    int current_turn;              /**< Indice del giocatore di turno (0..player_count - 1) */
    Card draw_pile[DECK];          /**< Mazzo di pesca */
    int cards_remaining;           /**< Carte residue nel mazzo di pesca */
    Card discard_pile[DECK];       /**< Mazzo degli scarti */
    int discarded_cards;           /**< Numero di carte presenti negli scarti */
    bool game_orientation;         /**< true: orario, false: antiorario */
    int penalties;                 /**< Carte cumulative di penalita' (+2 / +4) in sospeso */
} GameState;

#endif
