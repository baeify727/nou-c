/**
 * @file turn.h
 * @brief Gestione dei turni, senso di rotazione ed effetti delle carte speciali.
 */

#ifndef TURN_H
#define TURN_H

#include "structs.h"

/**
 * @brief Passa il turno al giocatore successivo in base alla direzione corrente (oraria o antioraria).
 * @param game Puntatore allo stato della partita.
 */
void next_turn(GameState *game);

/**
 * @brief Applica gli effetti delle carte speciali (Stop, Cambio giro, +2, +4).
 * @param game Puntatore allo stato della partita.
 * @param card Carta appena giocata sul tavolo.
 */
void apply_card_effect(GameState *game, Card card);

/**
 * @brief Verifica se ci sono penalita' di carte (+2 / +4) pendenti; se presenti, le fa pescare e salta il turno del giocatore.
 * @param game Puntatore allo stato della partita.
 * @param player_index Indice del giocatore di turno.
 */
void apply_penalties_if_any(GameState *game, int player_index);

#endif
