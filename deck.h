/**
 * @file deck.h
 * @brief Modulo per l'inizializzazione, mescolamento e gestione del mazzo di carte.
 */

#ifndef DECK_H
#define DECK_H

#include "structs.h"

/**
 * @brief Inizializza il mazzo con le 108 carte.
 * @param draw_pile Array destinato a contenere il mazzo di pesca.
 * @param cards_remaining Puntatore all'intero per memorizzare il totale delle carte inserite.
 */
void initialize_deck(Card draw_pile[], int *cards_remaining);

/**
 * @brief Mescola casualmente l'array di carte usando l'algoritmo di Fisher-Yates.
 * @param draw_pile Array delle carte da mescolare.
 * @param cards_remaining Numero di carte presenti da mescolare.
 */
void shuffle_deck(Card draw_pile[], int cards_remaining);

/**
 * @brief Estrae la carta in cima al mazzo di pesca.
 * @param draw_pile Array del mazzo di pesca.
 * @param cards_remaining Puntatore al conteggio delle carte residue (viene decrementato).
 * @return Card La carta pescata.
 */
Card draw_card(Card draw_pile[], int *cards_remaining);

#endif
