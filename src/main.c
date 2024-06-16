#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "macro.h"
#include "init.h"
#include "parser.h"
#include "graphics.h"
#include "event_handler.h"
#include "gamming.h"
#include "player.h"
#include "music.h"

// Function to render the login screen
void render_login_screen(SDL_Renderer *renderer) {
    // Load the background image
    SDL_Texture *background = load_texture(settings[0].enter_background, renderer);
    if (!background) {
        // Exit the program if the background image cannot be loaded
        printf("Error: Could not load enter_background.png\n");
        exit(1);
    }

    // Render the background image
    render_texture_fullscreen(background, renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Render buttons
    // render_button(renderer, "設定", 540, 200, 200, 50);
    render_button(renderer, "新遊戲", 540, 500, 200, 50);
    render_button(renderer, "繼續遊戲", 540, 600, 200, 50);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Free the background texture
    SDL_DestroyTexture(background);
}

// Main game loop
void main_loop(SDL_Renderer *renderer, GameState *game_state) {
    int running = 1;
    int current_screen = SCREEN_LOGIN;
    SDL_Event event;

    // Load music files
    Mix_Music *login_bgm = load_music(settings[0].login_bgm);
    Mix_Music *gaming_bgm = load_music(settings[0].gaming_bgm);

     // Play login music initially
    play_music(login_bgm);

    // Render the login screen initially
    render_login_screen(renderer);

    while (running) {
        handle_events(&event, &running, &current_screen, renderer, game_state);

        if (current_screen == SCREEN_LOGIN) {
            if (Mix_PlayingMusic() == 0) {
                play_music(login_bgm); 
            }
            render_login_screen(renderer);
        } else if (current_screen == SCREEN_NEW_GAME) {
            game_state->inventory_visible = 0;
            stop_music();
            current_screen = SCREEN_GAME_LOOP;
        } else if (current_screen == SCREEN_CONTINUE_GAME) {
            game_state->inventory_visible = 0;       
            stop_music();
            render_game_screen(renderer, game_state);
            current_screen = SCREEN_GAME_LOOP;
        } else if (current_screen == SCREEN_GAME_LOOP) {
            play_music(gaming_bgm); 
            // handle_option_buttons(renderer, &event, game_state);
        } else if (current_screen == SCREEN_END ) {
            running = 0;
        }
    }

    // Clean up music
    Mix_FreeMusic(login_bgm);
    Mix_FreeMusic(gaming_bgm);

    // Clean up resources
    if (game_state->current_image) {
        SDL_DestroyTexture(game_state->current_image);
    }
}

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize SDL and create window and renderer
    if (initialize_window(&window, &renderer) != 0) {
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf Initialization failed: %s\n", TTF_GetError());
        cleanup(window, renderer);
        return 1;
    }
    
    int parsed = parse_toml("example-game/script.toml");
    if (parsed != 0) {
        fprintf(stderr, "Error parsing TOML file\n");
        return 1;
    }

    // initialize game_state
    GameState game_state;
    game_state.player_name = malloc(10000);
    game_state.character = malloc(10000);
    game_state.character_avatar = malloc(10000);
    game_state.character_name = malloc(10000);
    game_state.choice_a = malloc(10000);
    game_state.choice_b = malloc(10000);
    game_state.choice_c = malloc(10000);
    game_state.dialogue_text = malloc(10000);
    game_state.option1_event = malloc(10000);
    game_state.option2_event = malloc(10000);
    game_state.option3_event = malloc(10000);
    game_state.option1_required = 0;
    game_state.option2_required = 0;
    game_state.option3_required = 0;
    game_state.option1_required_id = malloc(10000);
    game_state.option2_required_id = malloc(10000);
    game_state.option3_required_id = malloc(10000);
    game_state.option1_affection_change = 0;
    game_state.option2_affection_change = 0;
    game_state.option3_affection_change = 0;
    game_state.option1_character_id = malloc(10000);
    game_state.option2_character_id = malloc(10000);
    game_state.option3_character_id = malloc(10000);
    game_state.scene = malloc(10000);
    game_state.next_event = malloc(10000);
    game_state.have_choice = 0;
    game_state.inventory_visible = 0;

    // Enter the main game loop
    main_loop(renderer, &game_state);

    // Clean up resources and quit
    TTF_Quit();
    cleanup(window, renderer);

    return 0;
}
