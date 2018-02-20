#include "hiscore.h"
#include "base.h"
#include "resources.h"

#include <cstdio>
#include <cstring>

#define CARET_CHAR 0x7f
#define MAX_SCORES 10
#define MAX_CHARS  10

char names[MAX_SCORES][MAX_CHARS+1] =
    {"GobMan", "GobMan", "GobMan", "GobMan", "GobMan",
     "GobMan", "GobMan", "GobMan", "GobMan", "GobMan"};
int scores[MAX_SCORES] = {0,0,0,0,0,0,0,0,0,0};
int levels[MAX_SCORES] = {0,0,0,0,0,0,0,0,0,0};

void load_scores();
void save_scores();

void hiscore(int new_score, int level) {
    ALLEGRO_BITMAP *background = al_load_bitmap(RES_BMP_HISCORE);
    ALLEGRO_COLOR text = HEX_TO_COLOR(0xdff769), shadow = HEX_TO_COLOR(0x000000);
    int index = MAX_SCORES, caret = 0;
    char new_name[MAX_CHARS+1+1];

    load_scores();

    if (new_score >= 0) {
        for (index = 0; index < MAX_SCORES; index++)
            if (new_score > scores[index]) {
                for (int i=MAX_SCORES-1; i>index; i--) {
                    strcpy(names[i], names[i-1]);
                    scores[i] =  scores[i-1];
                    levels[i] =  levels[i-1];
                }
                names[index][0] = '\0';
                scores[index] = new_score;
                levels[index] = level;
                new_name[caret = 0] = CARET_CHAR;
                new_name[1] = '\0';
                break;
            }
    }

    al_draw_bitmap(background, 0, 0, 0);
    for (int i=0; i<MAX_SCORES; i++) {
        char str[20];
        draw_text_shadow(font_large, text, shadow, 45, 66+10*i, ALLEGRO_ALIGN_LEFT, names[i]);

        sprintf(str, "%d", scores[i]);
        draw_text_shadow(font_large, text, shadow, 218, 66+10*i, ALLEGRO_ALIGN_RIGHT, str);

        sprintf(str, "%02d", levels[i]);
        draw_text_shadow(font_large, text, shadow, 265, 66+10*i, ALLEGRO_ALIGN_RIGHT, str);
    }

    fade_in(DEFAULT_FADE_STEPS);

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());

    if (index < MAX_SCORES) {
        draw_rect_fill(45-1,               66+10*index-1,
                       45+9*(MAX_CHARS+1), 66+10*index+8, HEX_TO_COLOR(0x965918));
        draw_text_shadow(font_large, text, shadow, 45, 66+10*index, ALLEGRO_ALIGN_LEFT, new_name);

        show_keyboard();
    }

    bool end_loop = false;
    while(!end_loop) {

        flush_buffer();

        while (al_get_next_event(event_queue, &event)) {
        
            if (index < MAX_SCORES) {
                if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                        if (caret > 0) {
                            caret--;
                            new_name[caret] = CARET_CHAR;
                            new_name[caret+1] = '\0';
                        }
                    }
                    int c = event.keyboard.unichar;
                    if (c >= ' ' && c <= '~' && caret < MAX_CHARS) {
                        new_name[caret] = (char) c;
                        caret++;
                        new_name[caret] = CARET_CHAR;
                        new_name[caret+1] = '\0';
                    }
                }
                else if (event.type == ALLEGRO_EVENT_KEY_DOWN &&
                        (event.keyboard.keycode == ALLEGRO_KEY_ENTER
                        || event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER
                        || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                        || event.keyboard.keycode == ALLEGRO_KEY_BACK)
                        ) {
                    end_loop = true;
                }
                // else if (event.type == ALLEGRO_EVENT_TOUCH_END)

                draw_rect_fill(45-1,               66+10*index-1,
                               45+9*(MAX_CHARS+1), 66+10*index+8, HEX_TO_COLOR(0x965918));
                draw_text_shadow(font_large, text, shadow, 45, 66+10*index, ALLEGRO_ALIGN_LEFT, new_name);
            }
            else {
                switch (event.type) {
                    case ALLEGRO_EVENT_KEY_DOWN:
                        if (event.type == ALLEGRO_EVENT_KEY_DOWN &&
                        (event.keyboard.keycode == ALLEGRO_KEY_ENTER
                        || event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER
                        || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                        || event.keyboard.keycode == ALLEGRO_KEY_BACK)
                        )
                            end_loop = true;
                        break;
                    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
                    case ALLEGRO_EVENT_TOUCH_END:
                        end_loop = true;
                        break;
                }
            }
        }
    }

    al_destroy_event_queue(event_queue);

    hide_keyboard();

    if (index < MAX_SCORES) {
        new_name[caret] = '\0';
        strcpy(names[index], new_name);
    }

    save_scores();

    al_destroy_bitmap(background);
    fade_out(DEFAULT_FADE_STEPS);
}

void hiscore() {
    hiscore(-1, 0);
}

void load_scores() {
    ALLEGRO_FILE *file;
    #ifdef ANDROID
        al_set_standard_file_interface();
        ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
        al_set_path_filename(path, RES_SCORE_HISTORY);
        file = al_fopen(al_path_cstr(path, '/'), "rb");
        al_android_set_apk_file_interface();
    #else
        file = al_fopen(RES_SCORE_HISTORY, "rb");
    #endif
    
    if (!file) return;

    for (int i=0; i<MAX_SCORES; i++) {
        al_fread(file, names[i], MAX_CHARS+1);
        scores[i] = al_fread32le(file);
        al_fread(file, &levels[i], 1);
    }

    al_fclose(file);
}

void save_scores() {
    ALLEGRO_FILE *file;
    #ifdef ANDROID
        al_set_standard_file_interface();
        ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
        al_set_path_filename(path, RES_SCORE_HISTORY);
        file = al_fopen(al_path_cstr(path, '/'), "wb");
        al_android_set_apk_file_interface();
    #else
        file = al_fopen(RES_SCORE_HISTORY, "wb");
    #endif
    
    if (!file) return;

    for (int i=0; i<MAX_SCORES; i++) {
        al_fwrite(file, names[i], MAX_CHARS+1);
        al_fwrite32le(file, scores[i]);
        al_fwrite(file, &levels[i], 1);
    }

    al_fclose(file);
}
