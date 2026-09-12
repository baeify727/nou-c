/**
 * @file bot.h
 * @brief Logica decisionale e gestione delle mosse dei bot.
 */

#ifndef BOT_H
#define BOT_H

#include "structs.h"

/**
 * @brief Determina la mossa migliore per il bot in base allo stato attuale del tavolo.
 * @param game Puntatore allo stato globale della partita.
 * @param bot_index Indice del bot nell'array dei giocatori.
 * @return int Indice della carta da giocare all'interno della mano del bot, oppure -1 se deve pescare.
 */
int bot_choose_move(GameState *game, int bot_index);

/**
 * @brief Sceglie il colore da dichiarare quando il bot gioca una carta Jolly o +4.
 * @param game Puntatore allo stato globale della partita.
 * @param bot_index Indice del bot nell'array dei giocatori.
 * @return Color Il colore scelto in base alla maggioranza di carte possedute in mano.
 */
Color bot_choose_wild_card_color(GameState *game, int bot_index);

#endif
