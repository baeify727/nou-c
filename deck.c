#include "deck.h"
#include "structs.h"
#include <stdlib.h>
#include <time.h>

void initialize_deck(Card draw_pile[], int *cards_remaining) {
    int count = 0;

    for(Color color = COLOR_RED; color < COLOR_WILD; color++) {
        draw_pile[count].color = color;
        draw_pile[count].card_type = CARD_ZERO;
        count++;
        for (CardType type = CARD_ONE; type <= CARD_DRAW_TWO; type++) {
            draw_pile[count].color = color;
            draw_pile[count].card_type = type;
            draw_pile[count + 1].color = color;
            draw_pile[count + 1].card_type = type;
            count += 2;
        }
    }

    for(CardType type = CARD_WILD; type <= CARD_WILD_DRAW_FOUR; type++) {
        for(int j = 0; j < 4; j++) {
            draw_pile[count].color = COLOR_WILD;
            draw_pile[count].card_type = type;
            count++;
        }
    }

    *cards_remaining = count;

}

void shuffle_deck(Card draw_pile[], int cards_remaining) {
    int last_index = cards_remaining - 1;
    Card temp_card;
    int random_index;

    while(last_index > 0) {
        random_index = rand() % (last_index + 1);
        temp_card = draw_pile[random_index];
        draw_pile[random_index] = draw_pile[last_index];
        draw_pile[last_index] = temp_card;
        last_index--;
    }
}

Card draw_card(Card draw_pile[], int *cards_remaining) {
    Card card_drawn = draw_pile[*cards_remaining - 1];
    (*cards_remaining)--;
    return card_drawn;
}
