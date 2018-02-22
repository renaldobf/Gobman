#include "hidden_menu.h"
#include "base.h"
#include <cstdio>

#define RECUO_LATERAL  30
#define RECUO_VERTICAL 50
#define ALTURA_ITEM    20

#define ITEM_COUNT 6
const char *ITENS[] = {
    "FULLSCREEN",
    "UP KEY",
    "DOWN KEY",
    "LEFT KEY",
    "RIGHT KEY",
    "BOMB KEY"};
#define NUM_KEYS 5

void hidden_menu() {
    static bool needs_restart=false;
    int selected = 0;
    int keys[NUM_KEYS];
    char key_name[NUM_KEYS][30];

    keys[0] = key_up;
    keys[1] = key_down;
    keys[2] = key_left;
    keys[3] = key_right;
    keys[4] = key_fire;

    for (int i=0; i<NUM_KEYS; i++) {
        strcpy(key_name[i], al_keycode_to_name(keys[i]));
        upper_case_string(key_name[i]);
    }

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer_fps));
    al_flush_event_queue(event_queue);

    while (true) {

        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            al_flush_event_queue(event_queue);

            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                break;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                if (selected > 0) {
                    selected--;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                if (selected < 6-1) {
                    selected++;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER
                 ||  event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER) {

                draw_rect(RECUO_LATERAL, RECUO_VERTICAL-5 + ALTURA_ITEM * selected,
                    RECUO_LATERAL+220, RECUO_VERTICAL+10  + ALTURA_ITEM * selected,
                    HEX_TO_COLOR(0xff0000));
                flush_buffer();
                if (selected == 0) {
                    is_fullscreen = is_fullscreen ? false : true;
                    needs_restart = needs_restart ? false : true;
                }
                else if (selected >= 1 && selected <= NUM_KEYS+1) {
                    while (true) {
                        al_wait_for_event(event_queue, &event);

                        if (event.type != ALLEGRO_EVENT_KEY_DOWN)
                            continue;

                        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                            keys[selected-1] = 0;
                            break;
                        }

                        if (event.keyboard.keycode == ALLEGRO_KEY_UP
                        ||  event.keyboard.keycode == ALLEGRO_KEY_DOWN
                        ||  event.keyboard.keycode == ALLEGRO_KEY_LEFT
                        ||  event.keyboard.keycode == ALLEGRO_KEY_RIGHT
                        ||  event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                            continue;

                        keys[selected-1] = event.keyboard.keycode;
                        strcpy(key_name[selected-1],
                               al_keycode_to_name(keys[selected-1]));
                        upper_case_string(key_name[selected-1]);
                        break;
                    }
                    al_flush_event_queue(event_queue);
                }
            }
            continue;
        }

        if (event.type != ALLEGRO_EVENT_TIMER) {
            continue;
        }

        // TILE
        al_clear_to_color(HEX_TO_COLOR(0x00000));
        draw_text_shadow(font_large, HEX_TO_COLOR(0xffffff), HEX_TO_COLOR(0x444444),
            20, 20, ALLEGRO_ALIGN_LEFT, "HIDDEN MENU");
        draw_line(20, 30, 250, 30, HEX_TO_COLOR(0xff0000));

        // VARIABLES' NAMES

        for (int i=0; i<ITEM_COUNT; i++) {
        al_draw_textf(font_small, HEX_TO_COLOR(0xffffff),
            RECUO_LATERAL+70, RECUO_VERTICAL + ALTURA_ITEM * i,
            ALLEGRO_ALIGN_RIGHT, "%s", ITENS[i]);
        }

        // VARIABLES' STATUSES

        al_draw_text(font_small, HEX_TO_COLOR(0xffffff),
            RECUO_LATERAL+90, RECUO_VERTICAL + ALTURA_ITEM * 0,
            ALLEGRO_ALIGN_LEFT, is_fullscreen ? "ON" : "OFF");

        for (int i=0; i<NUM_KEYS; i++)
        al_draw_text(font_small, HEX_TO_COLOR(0xffffff),
            RECUO_LATERAL+90, RECUO_VERTICAL + ALTURA_ITEM * (1+i),
            ALLEGRO_ALIGN_LEFT, keys[i] == 0 ? "PRESS ENTER TO SELECT" : key_name[i]);

        // SELECTION BOX

        draw_rect(RECUO_LATERAL, RECUO_VERTICAL-5 + ALTURA_ITEM * selected,
              RECUO_LATERAL+220, RECUO_VERTICAL+10  + ALTURA_ITEM * selected,
              HEX_TO_COLOR(0x00ff00));

        al_draw_text(font_small, HEX_TO_COLOR(0xffff00),
            VIRTUAL_SCREEN_WIDTH/2, VIRTUAL_SCREEN_HEIGHT-28,
            ALLEGRO_ALIGN_CENTER, "PRESS ESCAPE TO EXIT");

        if (needs_restart)
        al_draw_text(font_small, HEX_TO_COLOR(0xff0000),
            VIRTUAL_SCREEN_WIDTH/2, VIRTUAL_SCREEN_HEIGHT-14,
            ALLEGRO_ALIGN_CENTER, "CHANGES WILL TAKE EFFECT UPON RESTART!");

        flush_buffer();
    }

    al_destroy_event_queue(event_queue);
    al_set_target_backbuffer(display);


    key_up    = keys[0];
    key_down  = keys[1];
    key_left  = keys[2];
    key_right = keys[3];
    key_fire  = keys[4];
}
