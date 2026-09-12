/**
 * @file game.h
 * @brief Configurazione iniziale del tavolo, regole di validazione e pesca delle carte.
 */

#ifndef GAME_H
#define GAME_H

#include "structs.h"
#include <stdbool.h>

/**
 * @brief Prepara la partita: mescola il mazzo, distribuisce 7 carte ai giocatori e scopre la prima carta valida.
 * @param game Puntatore alla struttura GameState da inizializzare.
 */
void game_setup(GameState *game);

/**
 * @brief Verifica se una carta selezionata puo' essere giocata sopra quella in cima agli scarti.
 * @param chosen_card La carta che il giocatore desidera giocare.
 * @param top_card La carta attualmente visibile in cima al mazzo degli scarti.
 * @return true se colore o valore coincidono oppure se la carta e' speciale/jolly, false altrimenti.
 */
bool is_move_valid(Card chosen_card, Card top_card);

/**
 * @brief Fa pescare una carta a un giocatore, rimescolando gli scarti se il mazzo e' esaurito.
 * @param game Puntatore allo stato della partita.
 * @param player_index Indice del giocatore che pesca.
 */
void draw_from_pile(GameState *game, int player_index);

#endif
