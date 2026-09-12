/**
 * @file storage.h
 * @brief Gestione della persistenza dei dati (salvataggio partita e profilo utente) in formato JSON.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include "structs.h"
#include <stdbool.h>

#define SAVE_FILE "nou_save.json"       /**< Percorso del file di salvataggio dello stato di gioco */
#define PROFILE_FILE "nou_profile.json" /**< Percorso del file con le statistiche persistenti */

/**
 * @brief Serializza e salva lo stato completo della partita corrente su file JSON.
 * @param game Puntatore alla struttura GameState da salvare.
 * @param filename Percorso del file su cui scrivere.
 * @return true se il salvataggio va a buon fine, false altrimenti.
 */
bool save_game(const GameState *game, const char *filename);

/**
 * @brief Carica e deserializza lo stato della partita da un file JSON.
 * @param game Puntatore alla struttura GameState da ripopolare.
 * @param filename Percorso del file JSON da leggere.
 * @return true se il caricamento ha successo, false altrimenti.
 */
bool load_game(GameState *game, const char *filename);

/**
 * @brief Verifica l'esistenza del file di salvataggio della partita.
 * @param filename Percorso del file da verificare.
 * @return true se il file esiste ed e' accessibile, false altrimenti.
 */
bool save_game_exists(const char *filename);

/**
 * @brief Salva su file JSON le statistiche e il nome del profilo giocatore.
 * @param profile Puntatore al profilo utente da salvare.
 * @param filename Percorso del file di destinazione.
 * @return true se il salvataggio va a buon fine, false altrimenti.
 */
bool save_profile(const PlayerProfile *profile, const char *filename);

/**
 * @brief Carica da file JSON le statistiche e il nome del profilo giocatore.
 * @param profile Puntatore alla struttura PlayerProfile da popolare.
 * @param filename Percorso del file da leggere.
 * @return true se il profilo e' stato caricato, false se il file non esiste o e' corrotto.
 */
bool load_profile(PlayerProfile *profile, const char *filename);

#endif
