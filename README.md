# NOU

Implementazione modulare in C del gioco di carte UNO, sviluppata con interfaccia a terminale (TUI) interattiva basata su **ncurses** e persistenza dei dati in formato JSON tramite la libreria **cJSON**.

---

## Panoramica

Il progetto replica le meccaniche del gioco da tavolo ufficiale integrando:
- Gestione completa del mazzo con rimescolamento automatico degli scarti ad esaurimento.
- Supporto alle carte azione e speciali: Salto turno, Inversione, +2, Cambio colore (Wild) e +4 (Wild Draw Four).
- IA configurabile per i bot avversari, dotata di livelli di intelligenza differenziati e logica per la selezione delle mosse.
- Meccanica di chiamata e contesa della regola "NOU!" (dichiarazione personale o penalizzazione per dimenticanza degli avversari, basato anche sull'intelligenza).
- Salvataggio e caricamento dello stato della partita e persistenza delle statistiche profilo utente.

---

## Architettura del Progetto

Il codice sorgente è organizzato secondo una struttura strettamente modulare:

| **Data Types** | `structs.h` | Definizione dei tipi enumerati (`Color`, `CardType`, `PlayerType`, `BotState`) e delle strutture dati centrali (`Card`, `Player`, `GameState`, `PlayerProfile`). |
| **Deck** | `deck.h`, `deck.c` | Inizializzazione del mazzo da 108 carte, algoritmo di shuffle (Fisher-Yates) e rimozione carte dalla pila. |
| **Game Logic** | `game.h`, `game.c` | Setup del tavolo, distribuzione iniziale, validazione delle mosse legali e gestione del mazzo di pesca. |
| **Turn Engine** | `turn.h`, `turn.c` | Rotazione oraria/antioraria dei turni, risoluzione degli effetti delle carte speciali e applicazione delle penalità di accumulo. |
| **Bot AI** | `bot.h`, `bot.c` | Logica di decisione mossa per i bot, selezione del colore ottimale per le carte jolly e gestione probabilistica del callout. |
| **Storage** | `storage.h`, `storage.c` | Serializzazione e deserializzazione JSON per lo stato di gioco (`uno_save.json`) e il profilo giocatore (`uno_profile.json`). |
| **Interface** | `ui.h`, `ui.c` | Rendering a schermo intero con colori ncurses, finestre informative, menu interattivi e log eventi. |
| **Entry Point** | `main.c` | Game loop, gestione degli input utente asincroni e verifica condizioni di vittoria. |

---

## Dipendenze

Per la compilazione del progetto sono necessari i seguenti pacchetti:

- Compilatore C standard compatibile C99/C11 (`gcc` o `clang`)
- Libreria **ncurses**
- Libreria **cJSON**
