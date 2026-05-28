#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#define MAX_STR_LEN 15
#define MAZZO 108

/**
 * Enum che racchiude tutti i colori delle carte, da rosso a speciale.
 */
typedef enum Colore {
    ROSSO, BLU, GIALLO, VERDE, SPECIALE
} Colore;

/*
 * Enum che descrive il tipo che una certa carta può assumere: numero da 0 a 9, e carte speciali come il +2 o il cambio colore.
 */
typedef enum TipoCarta {
    ZERO, UNO, DUE, TRE, QUATTRO, CINQUE, SEI, SETTE, OTTO, NOVE,
    SKIP, PIUDUE, REVERSE, CAMBIOCOLORE, PIUQUATTRO
} TipoCarta;

/*
 * Enum che descrive il tipo del giocatore, che può essere un umano (player) o bot.
 */
typedef enum TipoGiocatore {
    PLAYER, EASY_BOT, NORMAL_BOT
} TipoGiocatore;

/*
 * Struttura della singola carta, che possiede un colore e un tipo.
 */
typedef struct Carta {
    Colore colore;
    TipoCarta tipo_carta;
} Carta;

/*
 * Profilo del giocatore per mantenere stats in modo permanente.
 */
typedef struct ProfiloGiocatore {
    char nome_giocatore[MAX_STR_LEN];
    int partite_giocate;
    int partite_vinte;
    int carte_giocate;
} ProfiloGiocatore;

/*
 * Struttura di un giocatore attualmente in campo. Ha un nome, la sua mano attuale, e un TipoGiocatore.
 */
typedef struct Player {
    char nome_giocatore[MAX_STR_LEN];
    Carta mano[MAZZO];
    int numero_carte_mano;
    TipoGiocatore tipo_giocatore;
} Player;

/*
 * Stato della partita attuale, per salvare eventualmente una partita quando è in corso, o semplicemente per fare dei check in tempo reale.
 */
typedef struct StatoPartita {
    Player giocatori_in_gioco[4];
    int numero_giocatori;
    int turno_attuale;          /*< Indice per indicare di che giocatore è il turno attuale, da 0 a numero_giocatori - 1. */
    Carta mazzo_in_campo[MAZZO];
    int carte_rimaste;
    Carta pila_scarti[MAZZO];
    int carte_scartate;
    bool verso_gioco;           /*< True per senso orario, false per senso antiorario. */
    int penalita_cumulabili;
} StatoPartita;

#endif
