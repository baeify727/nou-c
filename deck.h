#ifndef DECK_H
#define DECK_H

#include "structs.h"

void initialize_deck(Card draw_pile[], int *cards_remaining);
void shuffle_deck(Card draw_pile[], int cards_remaining);
Card draw_card(Card draw_pile[], int *cards_remaining);


#endif
