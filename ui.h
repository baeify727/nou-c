/**
 * @file ui.h
 * @brief Gestione dell'interfaccia utente a riga di comando con ncurses.
 */

#ifndef UI_H
#define UI_H

#include "structs.h"
#include <stdbool.h>

/**
 * @brief Inizializza la modalita' curses, i colori e il supporto locale UTF-8.
 */
void ui_init(void);

/**
 * @brief Ripristina il terminale e termina la modalita' curses.
 */
void ui_cleanup(void);

/**
 * @brief Mostra il modulo di registrazione e acquisisce il nome del giocatore.
 * @param profile Puntatore al profilo utente da inizializzare.
 */
void ui_register_profile(PlayerProfile *profile);

/**
 * @brief Renderizza il menu principale con ASCII art e gestisce la selezione.
 * @param has_save Indica se esiste un file di salvataggio valido da caricare.
 * @return int Indice dell'opzione selezionata (0: Nuova Partita, 1: Continua, 2: Statistiche, 3: Esci).
 */
int ui_main_menu(bool has_save);

/**
 * @brief Mostra a schermo le statistiche del giocatore (vittorie, partite giocate, win rate).
 * @param profile Puntatore al profilo del giocatore da visualizzare.
 */
void ui_show_stats(PlayerProfile *profile);

/**
 * @brief Aggiunge un messaggio al log degli eventi a scorrimento laterale.
 * @param msg Stringa contenente il messaggio dell'evento.
 */
void ui_add_log(const char *msg);

/**
 * @brief Renderizza le mani visibili, la carta a terra e il log.
 * @param game Puntatore allo stato della partita.
 * @param selected_index Indice della carta attualmente selezionata nella mano del giocatore.
 */
void ui_render_board(GameState *game, int selected_index);

/**
 * @brief Chiede all'utente di selezionare un colore per una carta Jolly o +4.
 * @return Color Il colore scelto dall'utente.
 */
Color ui_prompt_wild_color(void);

/**
 * @brief Mostra la schermata finale con il nome del vincitore.
 * @param game Puntatore allo stato del gioco.
 * @param winner_index Indice del giocatore vincitore.
 */
void ui_show_winner(GameState *game, int winner_index);

/**
 * @brief Mostra il menu interattivo per regolare il livello di intelligenza dei bot (0-100).
 * @param game Puntatore allo stato di gioco contenente i bot.
 */
void ui_configure_bots(GameState *game);

/**
 * @brief Converte l'enum Color nella relativa rappresentazione testuale in italiano.
 * @param c Valore enumerato del colore.
 * @return const char* Stringa del colore (es. "Rosso", "Blu").
 */
const char* color_to_string(Color c);

/**
 * @brief Converte l'enum CardType nella relativa rappresentazione testuale (es. "Stop", "+2").
 * @param t Tipo di carta da convertire.
 * @return const char* Stringa identificativa della carta.
 */
const char* type_to_string(CardType t);

#endif
