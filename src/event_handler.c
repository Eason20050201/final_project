#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "graphics.h"
#include "event_handler.h"
#include "gamming.h"
#include "player.h"
#include "music.h"

// Function to handle events
void handle_events(SDL_Event *event, int *running, int *current_screen, SDL_Renderer *renderer, GameState *game_state) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            save_game("record.txt", game_state);
            *running = 0;
        } else if (event->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (*current_screen == SCREEN_LOGIN) {
                if (x >= 540 && x <= 740) {
                    if (y >= 500 && y <= 550) {
                        *current_screen = SCREEN_NEW_GAME;

                        int parsed = parse_toml("example-game/script.toml");
                        if (parsed != 0) {
                            fprintf(stderr, "Error parsing TOML file\n");
                            return;
                        }

                        set_player_name(renderer, game_state->player_name, running);
                        if( *running == 0 ) {
                            return;
                        }
                        // new game
                        game_state->event = "START";
                        render_game_screen(renderer, game_state);
                    } else if (y >= 600 && y <= 650) {
                        // continue game
                        if(load_game("record.txt", game_state) == 1) {
                            *current_screen = SCREEN_NEW_GAME;

                            int parsed = parse_toml("example-game/script.toml");
                            if (parsed != 0) {
                                fprintf(stderr, "Error parsing TOML file\n");
                                return;
                            }

                            set_player_name(renderer, game_state->player_name, running);
                            if( *running == 0 ) {
                                return;
                            }
                            // new game
                            game_state->event = "START";
                            render_game_screen(renderer, game_state);
                        }
                        else {
                            *current_screen = SCREEN_CONTINUE_GAME;
                            is_continue = 2;
                            game_state->event = save_event_id;
                            render_game_screen(renderer, game_state);
                        }
                    }
                }
            } else if (*current_screen == SCREEN_GAME_LOOP) {
                if( strcmp(game_state->event, "END") == 0 ) {
                    *current_screen = SCREEN_END;
                }
                if (x >= 25 && x <= 100 && y >= 20 && y <= 95) {
                    fade_out(renderer, game_state->current_image, 50);
                    SDL_DestroyTexture(game_state->current_image);
                    fade_in(renderer, load_texture(settings[0].enter_background, renderer), 50);
                    stop_music();
                    save_game("record.txt", game_state);
                    *current_screen = SCREEN_LOGIN;
                } else if (x >= 1180 && x <= 1255 && y >= 20 && y <= 95) {
                    handle_inventory_icon_click(renderer, game_state);
                } else if( !game_state->inventory_visible ) {
                    //render_game_screen(renderer, game_state);
                    handle_option_buttons(renderer, event, game_state);
                } 
            }
        }
    }
}

void render_game_screen(SDL_Renderer *renderer, GameState *game_state) {
    SDL_Texture *old_texture = game_state->current_image;
    char last_scene[200];
    strcpy(last_scene, game_state->scene);

    search_event( game_state );
    replaceSubstring(game_state->character_name, "{玩家}", game_state->player_name);
    replaceSubstring(game_state->dialogue_text, "{玩家}", game_state->player_name);

    // get the image of scene
    game_state->current_image = load_texture(game_state->scene, renderer);
    game_state->character_image = load_texture(game_state->character, renderer);

    SDL_RenderClear(renderer);  // Clear the renderer before drawing new content

    if ( strcmp( last_scene, game_state->scene ) != 0 ) {
        fade_out(renderer, old_texture, 50);
        SDL_DestroyTexture(old_texture);
        fade_in(renderer, game_state->current_image, 50);

        // crossfade(renderer, old_texture, game_state->current_image, 50);
        // SDL_DestroyTexture(old_texture);
    }
    else {
        render_texture_fullscreen(game_state->current_image, renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    render_texture_fullscreen(game_state->character_image, renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    render_dialog_box(renderer, game_state->dialogue_text, 50, WINDOW_HEIGHT - 150, WINDOW_WIDTH - 100, 150);
    render_name_box(renderer, game_state->character_name, 60, WINDOW_HEIGHT - 220, 150, 60);
    
    if( game_state->have_choice ) {
        render_button(renderer, game_state->choice_a, 350, 200, 600, 50);
        render_button(renderer, game_state->choice_b, 350, 300, 600, 50);
        render_button(renderer, game_state->choice_c, 350, 400, 600, 50);
    }
    render_inventory_icon(renderer, 1180, 20); // Render inventory icon
    render_avatar(renderer, 60, WINDOW_HEIGHT - 140, game_state->character_avatar);// Render avatar
    
    // not yet
    // if (game_state->inventory_visible) {
    //     const char *items[] = {"道具1", "道具2", "道具3"};
    //     int num_items = sizeof(items) / sizeof(items[0]);
    //     render_inventory(renderer, 100, 100, 400, 300, items, num_items); // Example position and size
    // }

    // Render character affinity in the top left corner
    // render_character_affinity(renderer, "A.png", game_state->affect_change, 10, 10, 50);

    render_home_icon(renderer, 25, 20); // Render hom icon

    SDL_RenderPresent(renderer);
}

void refresh_game_screen(SDL_Renderer *renderer, GameState *game_state) {
    // get the image of scene
    game_state->current_image = load_texture(game_state->scene, renderer);
    game_state->character_image = load_texture(game_state->character, renderer);

    SDL_RenderClear(renderer);  // Clear the renderer before drawing new content

    render_texture_fullscreen(game_state->current_image, renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    render_texture_fullscreen(game_state->character_image, renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    render_dialog_box(renderer, game_state->dialogue_text, 50, WINDOW_HEIGHT - 150, WINDOW_WIDTH - 100, 150);
    render_name_box(renderer, game_state->character_name, 60, WINDOW_HEIGHT - 220, 150, 60);
    
    if( game_state->have_choice ) {
        render_button(renderer, game_state->choice_a, 350, 200, 600, 50);
        render_button(renderer, game_state->choice_b, 350, 300, 600, 50);
        render_button(renderer, game_state->choice_c, 350, 400, 600, 50);
    }
    render_inventory_icon(renderer, 1180, 20); // Render inventory icon
    
    render_home_icon(renderer, 25, 20); // Render hom icon
    render_avatar(renderer, 60, WINDOW_HEIGHT - 140, game_state->character_avatar);// Render avatar

    SDL_RenderPresent(renderer);
}

// Function to handle option buttons
void handle_option_buttons(SDL_Renderer *renderer, SDL_Event *event, GameState *game_state) {
    Mix_Chunk *choice_effect = load_sound(settings[0].choice);
    Mix_Chunk *space_effect = load_sound(settings[0].space);

    int x, y;
    SDL_GetMouseState(&x, &y);

    if (event->type == SDL_MOUSEBUTTONDOWN ) {
        int change_event = 0;
        
        if (x >= 350 && x <= 950 && y >= 200 && y <= 250 && game_state->have_choice) {
            play_sound(choice_effect);

            if(game_state->option1_affection_change != 0) {
                for(int i = 0; i < character_count; i++) {
                    if(strcmp(game_state->option1_character_id, characters[i].id) == 0) {
                        characters[i].affection += game_state->option1_affection_change;
                        game_state->option1_affection_change = 0;
                        game_state->option2_affection_change = 0;
                        game_state->option3_affection_change = 0;
                        break;
                    }
                }
            }
            if(game_state->option1_required > 0) {
                for(int i = 0; i < item_count; i++) {
                    if(strcmp(game_state->option1_required_id, items[i].id) == 0 && items[i].quantity >= game_state->option1_required) {
                        items[i].quantity -= game_state->option1_required;
                        game_state->option1_required = 0;
                        game_state->option2_required = 0;
                        game_state->option3_required = 0;
                        change_event = 1;
                        game_state->event = game_state->option1_event;
                        break;
                    }
                }
            }
            else {
                change_event = 1;
                // strcpy(game_state->event, game_state->option1_event);
                game_state->event = game_state->option1_event;
                game_state->option1_affection_change = 0;
                game_state->option2_affection_change = 0;
                game_state->option3_affection_change = 0;
                game_state->option1_required = 0;
                game_state->option2_required = 0;
                game_state->option3_required = 0;
            }
        } else if (x >= 350 && x <= 950 && y >= 300 && y <= 350 && game_state->have_choice) {
            play_sound(choice_effect);

            if(game_state->option2_affection_change != 0) {
                for(int i = 0; i < character_count; i++) {
                    if(strcmp(game_state->option2_character_id, characters[i].id) == 0) {
                        characters[i].affection += game_state->option2_affection_change;
                        game_state->option1_affection_change = 0;
                        game_state->option2_affection_change = 0;
                        game_state->option3_affection_change = 0;
                        break;
                    }
                }
            }
            if(game_state->option2_required > 0) {
                for(int i = 0; i < item_count; i++) {
                    if(strcmp(game_state->option2_required_id, items[i].id) == 0 && items[i].quantity >= game_state->option2_required) {
                        items[i].quantity -= game_state->option2_required;
                        game_state->option1_required = 0;
                        game_state->option2_required = 0;
                        game_state->option3_required = 0;
                        change_event = 1;
                        game_state->event = game_state->option2_event;
                        break;
                    }
                }
            }
            else {
                change_event = 1;
                // strcpy(game_state->event, game_state->option2_event);
                game_state->event = game_state->option2_event;
                game_state->option1_affection_change = 0;
                game_state->option2_affection_change = 0;
                game_state->option3_affection_change = 0;
                game_state->option1_required = 0;
                game_state->option2_required = 0;
                game_state->option3_required = 0;
            }
        } else if (x >= 350 && x <= 950 && y >= 400 && y <= 450 && game_state->have_choice) {
            play_sound(choice_effect);

            if(game_state->option3_affection_change != 0) {
                for(int i = 0; i < character_count; i++) {
                    if(strcmp(game_state->option3_character_id, characters[i].id) == 0) {
                        characters[i].affection += game_state->option3_affection_change;
                        game_state->option1_affection_change = 0;
                        game_state->option2_affection_change = 0;
                        game_state->option3_affection_change = 0;
                        break;
                    }
                }
            }
            if(game_state->option3_required > 0) {
                for(int i = 0; i < item_count; i++) {
                    if(strcmp(game_state->option3_required_id, items[i].id) == 0 && items[i].quantity >= game_state->option3_required) {
                        items[i].quantity -= game_state->option3_required;
                        game_state->option1_required = 0;
                        game_state->option2_required = 0;
                        game_state->option3_required = 0;
                        change_event = 1;
                        game_state->event = game_state->option3_event;
                        break;
                    }
                }
            }
            else {
                change_event = 1;
                // strcpy(game_state->event, game_state->option3_event);
                game_state->event = game_state->option3_event;
                game_state->option1_affection_change = 0;
                game_state->option2_affection_change = 0;
                game_state->option3_affection_change = 0;
                game_state->option1_required = 0;
                game_state->option2_required = 0;
                game_state->option3_required = 0;
            }
        }
        else if ( !game_state->have_choice ) {
            play_sound(space_effect);         

            change_event = 1;
            // strcpy(game_state->event, game_state->next_event);
            game_state->event = game_state->next_event;
        }

        if (change_event) {
            render_game_screen(renderer, game_state);
        }
    }
}

// not yet
void handle_inventory_icon_click(SDL_Renderer *renderer, GameState *game_state) {
    Mix_Chunk *open_bag_effect = load_sound(settings[0].open_bag);
    Mix_Chunk *close_bag_effect = load_sound(settings[0].close_bag);

    if( game_state->inventory_visible == 0 )
        play_sound(open_bag_effect);
    else
        play_sound(close_bag_effect);

    game_state->inventory_visible = !game_state->inventory_visible; // Toggle visibility

    if (game_state->inventory_visible) {
        render_inventory(renderer, 290, 10, 400, 300, 5); // Example position and size
    }
    else {
        refresh_game_screen(renderer, game_state);
    }

    SDL_RenderPresent(renderer);
}    
