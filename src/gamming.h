#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

extern char *save_event_id;
extern int is_continue;

void search_event( GameState *game_state );
void save_game(const char *filename, GameState *game_state);
int load_game(const char *filename, GameState *game_state);