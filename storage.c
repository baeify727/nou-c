#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

static cJSON* card_to_json(Card card) {
    cJSON *card_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(card_obj, "color", card.color);
    cJSON_AddNumberToObject(card_obj, "card_type", card.card_type);
    return card_obj;
}

static Card card_from_json(cJSON *card_obj) {
    Card result = {GAME_COLOR_RED, CARD_ZERO};
    if(card_obj == NULL) {
        return result;
    }

    cJSON *col_item = cJSON_GetObjectItemCaseSensitive(card_obj, "color");
    cJSON *type_item = cJSON_GetObjectItemCaseSensitive(card_obj, "card_type");

    if(cJSON_IsNumber(col_item)) {
        result.color = (Color)col_item->valueint;
    }
    if(cJSON_IsNumber(type_item)) {
        result.card_type = (CardType)type_item->valueint;
    }

    return result;
}

static char* read_file_content(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if(buffer == NULL) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

static bool write_file_content(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if(file == NULL) {
        return false;
    }

    fputs(content, file);
    fclose(file);
    return true;
}

bool save_profile(const PlayerProfile *profile, const char *filename) {
    cJSON *root = cJSON_CreateObject();
    if(root == NULL) {
        return false;
    }

    cJSON_AddStringToObject(root, "player_name", profile->player_name);
    cJSON_AddNumberToObject(root, "games_played", profile->games_played);
    cJSON_AddNumberToObject(root, "games_won", profile->games_won);
    cJSON_AddNumberToObject(root, "card_played", profile->card_played);

    char *json_str = cJSON_Print(root);
    bool result = false;

    if(json_str != NULL) {
        result = write_file_content(filename, json_str);
        free(json_str);
    }

    cJSON_Delete(root);
    return result;
}

bool load_profile(PlayerProfile *profile, const char *filename) {
    char *content = read_file_content(filename);
    if(content == NULL) {
        return false;
    }

    cJSON *root = cJSON_Parse(content);
    free(content);

    if(root == NULL) {
        return false;
    }

    cJSON *name_item = cJSON_GetObjectItemCaseSensitive(root, "player_name");
    cJSON *played_item = cJSON_GetObjectItemCaseSensitive(root, "games_played");
    cJSON *won_item = cJSON_GetObjectItemCaseSensitive(root, "games_won");
    cJSON *cards_item = cJSON_GetObjectItemCaseSensitive(root, "card_played");

    if(cJSON_IsString(name_item)) {
        strncpy(profile->player_name, name_item->valuestring, MAX_STR_LEN - 1);
        profile->player_name[MAX_STR_LEN - 1] = '\0';
    }
    if(cJSON_IsNumber(played_item)) {
        profile->games_played = played_item->valueint;
    }
    if(cJSON_IsNumber(won_item)) {
        profile->games_won = won_item->valueint;
    }
    if(cJSON_IsNumber(cards_item)) {
        profile->card_played = cards_item->valueint;
    }

    cJSON_Delete(root);
    return true;
}

bool save_game(const GameState *game, const char *filename) {
    cJSON *root = cJSON_CreateObject();
    if(root == NULL) {
        return false;
    }

    cJSON_AddNumberToObject(root, "player_count", game->player_count);
    cJSON_AddNumberToObject(root, "current_turn", game->current_turn);
    cJSON_AddBoolToObject(root, "game_orientation", game->game_orientation);
    cJSON_AddNumberToObject(root, "penalties", game->penalties);
    cJSON_AddNumberToObject(root, "cards_remaining", game->cards_remaining);
    cJSON_AddNumberToObject(root, "discarded_cards", game->discarded_cards);

    cJSON *draw_pile_arr = cJSON_CreateArray();
    for(int i = 0; i < game->cards_remaining; i++) {
        cJSON_AddItemToArray(draw_pile_arr, card_to_json(game->draw_pile[i]));
    }
    cJSON_AddItemToObject(root, "draw_pile", draw_pile_arr);

    cJSON *discard_pile_arr = cJSON_CreateArray();
    for(int i = 0; i < game->discarded_cards; i++) {
        cJSON_AddItemToArray(discard_pile_arr, card_to_json(game->discard_pile[i]));
    }
    cJSON_AddItemToObject(root, "discard_pile", discard_pile_arr);

    cJSON *players_arr = cJSON_CreateArray();
    for(int i = 0; i < game->player_count; i++) {
        cJSON *player_obj = cJSON_CreateObject();
        const Player *p = &game->players[i];

        cJSON_AddStringToObject(player_obj, "player_name", p->player_name);
        cJSON_AddNumberToObject(player_obj, "player_type", p->player_type);
        cJSON_AddNumberToObject(player_obj, "intelligence", p->intelligence);
        cJSON_AddNumberToObject(player_obj, "bot_state", p->bot_state);
        cJSON_AddNumberToObject(player_obj, "hand_card_count", p->hand_card_count);
        cJSON_AddBoolToObject(player_obj, "said_uno", p->said_uno);

        cJSON *hand_arr = cJSON_CreateArray();
        for(int h = 0; h < p->hand_card_count; h++) {
            cJSON_AddItemToArray(hand_arr, card_to_json(p->hand[h]));
        }
        cJSON_AddItemToObject(player_obj, "hand", hand_arr);
        cJSON_AddItemToArray(players_arr, player_obj);
    }
    cJSON_AddItemToObject(root, "players", players_arr);

    char *json_str = cJSON_Print(root);
    bool result = false;

    if(json_str != NULL) {
        result = write_file_content(filename, json_str);
        free(json_str);
    }

    cJSON_Delete(root);
    return result;
}

bool load_game(GameState *game, const char *filename) {
    char *content = read_file_content(filename);
    if(content == NULL) {
        return false;
    }

    cJSON *root = cJSON_Parse(content);
    free(content);

    if(root == NULL) {
        return false;
    }

    cJSON *pc_item = cJSON_GetObjectItemCaseSensitive(root, "player_count");
    cJSON *ct_item = cJSON_GetObjectItemCaseSensitive(root, "current_turn");
    cJSON *go_item = cJSON_GetObjectItemCaseSensitive(root, "game_orientation");
    cJSON *pen_item = cJSON_GetObjectItemCaseSensitive(root, "penalties");
    cJSON *cr_item = cJSON_GetObjectItemCaseSensitive(root, "cards_remaining");
    cJSON *dc_item = cJSON_GetObjectItemCaseSensitive(root, "discarded_cards");

    if(cJSON_IsNumber(pc_item)) game->player_count = pc_item->valueint;
    if(cJSON_IsNumber(ct_item)) game->current_turn = ct_item->valueint;
    if(cJSON_IsBool(go_item))   game->game_orientation = cJSON_IsTrue(go_item);
    if(cJSON_IsNumber(pen_item)) game->penalties = pen_item->valueint;
    if(cJSON_IsNumber(cr_item))  game->cards_remaining = cr_item->valueint;
    if(cJSON_IsNumber(dc_item))  game->discarded_cards = dc_item->valueint;

    cJSON *draw_pile_arr = cJSON_GetObjectItemCaseSensitive(root, "draw_pile");
    int draw_idx = 0;
    cJSON *card_item = NULL;
    cJSON_ArrayForEach(card_item, draw_pile_arr) {
        if(draw_idx < DECK) {
            game->draw_pile[draw_idx++] = card_from_json(card_item);
        }
    }

    cJSON *discard_pile_arr = cJSON_GetObjectItemCaseSensitive(root, "discard_pile");
    int disc_idx = 0;
    cJSON_ArrayForEach(card_item, discard_pile_arr) {
        if(disc_idx < DECK) {
            game->discard_pile[disc_idx++] = card_from_json(card_item);
        }
    }

    cJSON *players_arr = cJSON_GetObjectItemCaseSensitive(root, "players");
    int player_idx = 0;
    cJSON *player_item = NULL;
    cJSON_ArrayForEach(player_item, players_arr) {
        if(player_idx >= 4) break;
        Player *p = &game->players[player_idx];

        cJSON *name_item = cJSON_GetObjectItemCaseSensitive(player_item, "player_name");
        cJSON *type_item = cJSON_GetObjectItemCaseSensitive(player_item, "player_type");
        cJSON *intel_item = cJSON_GetObjectItemCaseSensitive(player_item, "intelligence");
        cJSON *state_item = cJSON_GetObjectItemCaseSensitive(player_item, "bot_state");
        cJSON *hand_count_item = cJSON_GetObjectItemCaseSensitive(player_item, "hand_card_count");
        cJSON *uno_item = cJSON_GetObjectItemCaseSensitive(player_item, "said_uno");

        if(cJSON_IsString(name_item)) {
            strncpy(p->player_name, name_item->valuestring, MAX_STR_LEN - 1);
            p->player_name[MAX_STR_LEN - 1] = '\0';
        }
        if(cJSON_IsNumber(type_item))       p->player_type = (PlayerType)type_item->valueint;
        if(cJSON_IsNumber(intel_item))      p->intelligence = intel_item->valueint;
        if(cJSON_IsNumber(state_item))      p->bot_state = (BotState)state_item->valueint;
        if(cJSON_IsNumber(hand_count_item)) p->hand_card_count = hand_count_item->valueint;

        if(cJSON_IsBool(uno_item)) {
            p->said_uno = cJSON_IsTrue(uno_item);
        } else {
            p->said_uno = false;
        }

        cJSON *hand_arr = cJSON_GetObjectItemCaseSensitive(player_item, "hand");
        int hand_idx = 0;
        cJSON *hand_card_item = NULL;
        cJSON_ArrayForEach(hand_card_item, hand_arr) {
            if(hand_idx < DECK) {
                p->hand[hand_idx++] = card_from_json(hand_card_item);
            }
        }

        player_idx++;
    }

    cJSON_Delete(root);
    return true;
}

bool save_game_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}
