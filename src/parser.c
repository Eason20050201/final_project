#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// 定義全局變數來存儲解析的數據
Scene* scenes;
int scene_count;
Character* characters;
int character_count;
Item* items;
int item_count;
Event* events;
int event_count;
Setting* settings;
int setting_count;

// 函數原型
void parse_settings(toml_table_t* conf);
void parse_scenes(toml_table_t* conf);
void parse_characters(toml_table_t* conf);
void parse_items(toml_table_t* conf);
void parse_events(toml_table_t* conf);
void parse_choices(toml_table_t* event, Event* evt);
void parse_judge_event(toml_table_t* event, Event* evt);
void parse_avatar_characters(toml_table_t* characters, Character* cha);
void parse_sprite_characters(toml_table_t* characters, Character* cha);
void update_affection(const char* character_id, int affection_change);
void handle_choice(Event* evt, int choice_index);

// 解析TOML文件的函數
int parse_toml(const char* filename) {
    char error[1024] = {0};
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open %s\n", filename);
        return 1;
    }

    toml_table_t* conf = toml_parse_file(fp, error, 0);
    fclose(fp);
    if (!conf) {
        printf("Failed to parse %s\n", filename);
        return 1;
    }

    parse_scenes(conf);
    parse_characters(conf);
    parse_items(conf);
    parse_events(conf);
    parse_settings(conf);

    toml_free(conf);
    return 0;
}

void parse_settings(toml_table_t* conf) {
    toml_array_t* settings_arr = toml_array_in(conf, "settings");
    setting_count = toml_array_nelem(settings_arr);
    settings = malloc(setting_count * sizeof(Setting));

    for (int i = 0; i < setting_count; i++) {
        toml_table_t* setting = toml_table_at(settings_arr, i);
        char* enter_name;
        char* enter_background;
        char* home_icon;
        char* backpack;
        char* choice;
        char* close_bag;
        char* gaming_bgm;
        char* login_bgm;
        char* open_bag;
        char* space;
        char* inventory_icon;
        settings[i].backpack = NULL;
        settings[i].choice = NULL;
        settings[i].close_bag = NULL;
        settings[i].gaming_bgm = NULL;
        settings[i].login_bgm = NULL;
        settings[i].open_bag = NULL;
        settings[i].space = NULL;
        settings[i].enter_background = NULL;
        settings[i].enter_name = NULL;
        settings[i].home_icon = NULL;

        if (toml_rtos(toml_raw_in(setting, "enter_name"), &enter_name) == 0){
            settings[i].enter_name = strdup(enter_name);
        }
        if (toml_rtos(toml_raw_in(setting, "enter_background"), &enter_background) == 0){
            settings[i].enter_background = strdup(enter_background);
        }
        if (toml_rtos(toml_raw_in(setting, "home_icon"), &home_icon) == 0){
            settings[i].home_icon = strdup(home_icon);
        }
        if (toml_rtos(toml_raw_in(setting, "backpack"), &backpack) == 0){
            settings[i].backpack = strdup(backpack);
        }
        if (toml_rtos(toml_raw_in(setting, "choice"), &choice) == 0){
            settings[i].choice = strdup(choice);
        }
        if (toml_rtos(toml_raw_in(setting, "close_bag"), &close_bag) == 0){
            settings[i].close_bag = strdup(close_bag);
        }
        if (toml_rtos(toml_raw_in(setting, "gaming_bgm"), &gaming_bgm) == 0){
            settings[i].gaming_bgm = strdup(gaming_bgm);
        }
        if (toml_rtos(toml_raw_in(setting, "login_bgm"), &login_bgm) == 0){
            settings[i].login_bgm = strdup(login_bgm);
        }
        if (toml_rtos(toml_raw_in(setting, "open_bag"), &open_bag) == 0){
            settings[i].open_bag = strdup(open_bag);
        }
        if (toml_rtos(toml_raw_in(setting, "space"), &space) == 0){
            settings[i].space = strdup(space);
        }
        if (toml_rtos(toml_raw_in(setting, "inventory_icon"), &inventory_icon) == 0){
            settings[i].inventory_icon = strdup(inventory_icon);
        }
    }
}

void parse_scenes(toml_table_t* conf) {
    toml_array_t* scenes_arr = toml_array_in(conf, "scenes");
    scene_count = toml_array_nelem(scenes_arr);
    scenes = malloc(scene_count * sizeof(Scene));

    for (int i = 0; i < scene_count; i++) {
        toml_table_t* scene = toml_table_at(scenes_arr, i);
        char* id;
        char* background;

        if (toml_rtos(toml_raw_in(scene, "id"), &id) == 0 &&
            toml_rtos(toml_raw_in(scene, "background"), &background) == 0) {
            scenes[i].id = strdup(id);
            scenes[i].background = strdup(background);
        }
    }
}

void parse_characters(toml_table_t* conf) {
    toml_array_t* characters_arr = toml_array_in(conf, "characters");
    character_count = toml_array_nelem(characters_arr);
    characters = malloc(character_count * sizeof(Character));

    for (int i = 0; i < character_count; i++) {
        toml_table_t* character = toml_table_at(characters_arr, i);
        char* id;
        char* name;
        char* avatar;
        char* sprite;
        int64_t affection;

        if (toml_rtos(toml_raw_in(character, "id"), &id) == 0 &&
            toml_rtos(toml_raw_in(character, "name"), &name) == 0 &&
            toml_rtoi(toml_raw_in(character, "affection"), &affection) == 0) {
            characters[i].id = strdup(id);
            characters[i].name = strdup(name);
            characters[i].affection = affection;
            parse_avatar_characters(character, &characters[i]);
            parse_sprite_characters(character, &characters[i]);
        }
    }
}

void parse_items(toml_table_t* conf) {
    toml_array_t* items_arr = toml_array_in(conf, "items");
    item_count = toml_array_nelem(items_arr);
    items = malloc(item_count * sizeof(Item));

    for (int i = 0; i < item_count; i++) {
        toml_table_t* item = toml_table_at(items_arr, i);
        char* id;
        char* name;
        char* icon;
        char* description;
        int64_t quantity;

        if (toml_rtos(toml_raw_in(item, "id"), &id) == 0 &&
            toml_rtos(toml_raw_in(item, "name"), &name) == 0 &&
            toml_rtos(toml_raw_in(item, "icon"), &icon) == 0 &&
            toml_rtoi(toml_raw_in(item, "quantity"), &quantity) == 0 &&
            toml_rtos(toml_raw_in(item, "description"), &description) == 0) {
            items[i].id = strdup(id);
            items[i].name = strdup(name);
            items[i].icon = strdup(icon);
            items[i].description = strdup(description);
            items[i].quantity = quantity;
        } else {
            items[i].quantity = 0;
        }
    }
}

void parse_events(toml_table_t* conf) {
    toml_array_t* events_arr = toml_array_in(conf, "events");
    event_count = toml_array_nelem(events_arr);
    events = malloc(event_count * sizeof(Event));

    for (int i = 0; i < event_count; i++) {
        toml_table_t* event = toml_table_at(events_arr, i);
        char* scene = NULL;
        char* character = NULL;
        char* dialogue = NULL;
        char* next_event = NULL;
        char* id = NULL;
        char* obtain_id = NULL;
        int64_t obtain = 0;
        events[i].obtain_id = NULL;
        events[i].obtain = 0;
        events[i].choice_count = 0;
        events[i].judge_event_count = 0;
        events[i].next_event = NULL;

        if (toml_rtos(toml_raw_in(event, "scene"), &scene) == 0 &&
            toml_rtos(toml_raw_in(event, "character"), &character) == 0 &&
            toml_rtos(toml_raw_in(event, "dialogue"), &dialogue) == 0 &&
            toml_rtos(toml_raw_in(event, "id"), &id) == 0){
            events[i].scene = strdup(scene);
            events[i].character = strdup(character);
            events[i].dialogue = strdup(dialogue);
            events[i].id = strdup(id);
            if (toml_rtos(toml_raw_in(event, "obtain_id"), &obtain_id) == 0 &&
                toml_rtoi(toml_raw_in(event, "obtain"), &obtain) == 0){
                events[i].obtain_id = strdup(obtain_id);
                events[i].obtain = obtain;
            }
            if(toml_rtos(toml_raw_in(event, "next_event"), &next_event) == 0){
                events[i].next_event = strdup(next_event);
            }
            else if(strstr(events[i].id, "JUDGE") != NULL){
                parse_judge_event(event, &events[i]);
            }
            else{
                parse_choices(event, &events[i]);
            }
            
        }
    }
}

void parse_judge_event(toml_table_t* event, Event* evt){
    toml_array_t* judge_event_arr = toml_array_in(event, "judge_event");
    evt->judge_event_count = toml_array_nelem(judge_event_arr);
    evt->judge_event = malloc(evt->judge_event_count * sizeof(Judge_Event));

    for (int j = 0; j < evt->judge_event_count; j++) {
        toml_table_t* judge_event = toml_table_at(judge_event_arr, j);
        char* next_event = NULL;
        char* character_id = NULL;
        int64_t required_affection = 0;
        if (toml_rtos(toml_raw_in(judge_event, "character_id"), &character_id) == 0 &&
            toml_rtos(toml_raw_in(judge_event, "next_event"), &next_event) == 0 && 
            toml_rtoi(toml_raw_in(judge_event, "required_affection"), &required_affection) == 0) {
            evt->judge_event[j].character_id = strdup(character_id);
            evt->judge_event[j].next_event = strdup(next_event);
            evt->judge_event[j].required_affection = required_affection;
        }
    }
}

void parse_avatar_characters(toml_table_t* characters, Character* cha){
    toml_array_t* characters_arr = toml_array_in(characters, "avatar");
    cha->avater_count = toml_array_nelem(characters_arr);
    cha->avater_struct = malloc(cha->avater_count * sizeof(Avatar));

    for (int j = 0; j < cha->avater_count; j++) {
        toml_table_t* avatar_parse = toml_table_at(characters_arr, j);
        char* avatar_id = NULL;
        int64_t required_affection = 0;
        cha->avater_struct[j].avatar_id = NULL;
        cha->avater_struct[j].required_affection = -10000;
        if(toml_rtos(toml_raw_in(avatar_parse, "avatar_id"), &avatar_id) == 0){
            cha->avater_struct[j].avatar_id = strdup(avatar_id);
        }
        if(toml_rtoi(toml_raw_in(avatar_parse, "required_affection"), &required_affection) == 0){
            cha->avater_struct[j].required_affection = required_affection;
        }

    }
}

void parse_sprite_characters(toml_table_t* characters, Character* cha){
    toml_array_t* characters_arr = toml_array_in(characters, "sprite");
    cha->sprite_count = toml_array_nelem(characters_arr);
    cha->sprite_struct = malloc(cha->sprite_count * sizeof(Avatar));

    for (int j = 0; j < cha->sprite_count; j++) {
        toml_table_t* sprite_parse = toml_table_at(characters_arr, j);
        char* sprite_id = NULL;
        int64_t required_affection = 0;
        cha->sprite_struct[j].sprite_id = NULL;
        cha->sprite_struct[j].required_affection = -10000;
        if(toml_rtos(toml_raw_in(sprite_parse, "sprite_id"), &sprite_id) == 0){
            cha->sprite_struct[j].sprite_id = strdup(sprite_id);
        }
        if(toml_rtoi(toml_raw_in(sprite_parse, "required_affection"), &required_affection) == 0){
            cha->sprite_struct[j].required_affection = required_affection;
        }
    }
}

void parse_choices(toml_table_t* event, Event* evt) {
    toml_array_t* choices_arr = toml_array_in(event, "choices");
    evt->choice_count = toml_array_nelem(choices_arr);
    evt->choices = malloc(evt->choice_count * sizeof(Choice));

    for (int j = 0; j < evt->choice_count; j++) {
        toml_table_t* choice = toml_table_at(choices_arr, j);
        char* text;
        char* next_event = NULL;
        char* character_id = NULL;
        char* required_id = NULL;
        int64_t affection_changes = 0;
        int64_t required = 0;

        if (toml_rtos(toml_raw_in(choice, "text"), &text) == 0 &&
            toml_rtos(toml_raw_in(choice, "next_event"), &next_event) == 0) {
            evt->choices[j].text = strdup(text);
            evt->choices[j].next_event = strdup(next_event);
            if (toml_rtos(toml_raw_in(choice, "character_id"), &character_id) == 0) {
                evt->choices[j].character_id = strdup(character_id);
            } else {
                evt->choices[j].character_id = NULL;
            }
            if (toml_rtoi(toml_raw_in(choice, "affection_changes"), &affection_changes) == 0) {
                evt->choices[j].affection_changes = affection_changes;
            } else {
                evt->choices[j].affection_changes = 0;
            }
            if (toml_rtoi(toml_raw_in(choice, "required"), &required) == 0) {
                evt->choices[j].required = required;
            } else {
                evt->choices[j].required = 0;
            }
            if (toml_rtos(toml_raw_in(choice, "required_id"), &required_id) == 0) {
                evt->choices[j].required_id = strdup(required_id);
            } else {
                evt->choices[j].required_id = NULL;
            }
        }
    }
}
