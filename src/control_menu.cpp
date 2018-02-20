#include "control_menu.h"
#include "base.h"
#include "resources.h"

#include <cmath>
#include <cstdio>
#include <ctype.h>

#define BOX_WIDTH  ((VIRTUAL_SCREEN_WIDTH  - al_get_bitmap_width(box)) / 2 )
#define BOX_HEIGHT ((VIRTUAL_SCREEN_HEIGHT - al_get_bitmap_height(box)) / 2)

#define MENU_HEIGHT  (11  + BOX_HEIGHT)
#define ARROW_PAD_X  (5   + BOX_WIDTH)
#define ARROW_PAD_X2 (100 + BOX_WIDTH)

#define ITEM_HEIGHT 17

void control_menu() {
    static int selected = 0;

    ALLEGRO_BITMAP *box = al_load_bitmap(RES_BMP_CONTROL);
    ALLEGRO_BITMAP *joy = al_load_bitmap(RES_BMP_JOYSTICK);
    ALLEGRO_BITMAP *arrow = al_load_bitmap(RES_BMP_ARROW);

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    int arrow_pos = MENU_HEIGHT + ITEM_HEIGHT * selected;
    int animation = 0;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer_fps));

    al_draw_bitmap(box, BOX_WIDTH, BOX_HEIGHT, 0);
    al_draw_bitmap(arrow, ARROW_PAD_X,  arrow_pos, 0);
    al_draw_bitmap(arrow, ARROW_PAD_X2, arrow_pos, ALLEGRO_FLIP_HORIZONTAL);

    al_flush_event_queue(event_queue);
    while (true) {

        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TOUCH_END) {
            if (!joystick_is_installed)
                selected = 0;
            break;
        }

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
            ||  event.keyboard.keycode == ALLEGRO_KEY_BACK) {
                if (!joystick_is_installed)
                    selected = 0;
                break;
            }
            else if ((event.keyboard.keycode == ALLEGRO_KEY_ENTER
                    ||event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER)) {

                if (selected == 0) { // KEYBOARD
                    joystick_is_installed = false;
                    break;
                }
                else { // JOYSTICK
                    al_reconfigure_joysticks();
                    al_draw_bitmap(joy,
                        (VIRTUAL_SCREEN_WIDTH - al_get_bitmap_width(joy))/2,
                        (VIRTUAL_SCREEN_HEIGHT - al_get_bitmap_height(joy))/2,
                        0);
                    flush_buffer();
                    while (true) {
                        al_wait_for_event(event_queue, &event);
                        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                                joystick_id = NULL;
                                break;
                            }
                            continue;
                        }
                        if (event.type != ALLEGRO_EVENT_JOYSTICK_AXIS)
                            continue;
                        #ifdef LAME_JOYSTICK
                        if (event.joystick.stick == 0) continue;
                        #endif
                        if (fabs(event.joystick.pos) < 0.5)
                            continue;
                        joystick_id = event.joystick.id;
                        joystick_vertical_stick = event.joystick.stick;
                        joystick_vertical_axis = event.joystick.axis;
                        joystick_vertical_positive = event.joystick.pos > 0 ? 1 : -1;
                        break;
                    }
                    if (joystick_id == NULL)
                        break;
                    {
                        strcpy(joystick_name, al_get_joystick_name(joystick_id));
                        remove_space(joystick_name);
                        char joy_name[256];
                        strcpy(joy_name, joystick_name);
                        upper_case_string(joy_name);
                        al_draw_text(font_small, HEX_TO_COLOR(0x00ffff),
                            VIRTUAL_SCREEN_WIDTH/2,
                            VIRTUAL_SCREEN_HEIGHT/2,
                            ALLEGRO_ALIGN_CENTER,
                            joy_name
                            );
                    }
                    flush_buffer();
                    while (true) {
                        al_wait_for_event(event_queue, &event);
                        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                                joystick_id = NULL;
                                break;
                            }
                            continue;
                        }
                        if (event.type != ALLEGRO_EVENT_JOYSTICK_AXIS)
                            continue;
                        if (joystick_id != event.joystick.id)
                            continue;
                        #ifdef LAME_JOYSTICK
                        if (event.joystick.stick == 0) continue;
                        #endif
                        if (event.joystick.stick == joystick_vertical_stick
                        &&  event.joystick.axis == joystick_vertical_axis)
                            continue;
                        if (fabs(event.joystick.pos) < 0.5)
                            continue;
                        joystick_horizontal_stick = event.joystick.stick;
                        joystick_horizontal_axis = event.joystick.axis;
                        joystick_horizontal_positive = event.joystick.pos > 0 ? 1 : -1;
                        break;
                    }
                    if (joystick_id == NULL)
                        break;
                    while (true) {
                        al_wait_for_event(event_queue, &event);
                        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                                joystick_id = NULL;
                                break;
                            }
                            continue;
                        }
                        if (event.type != ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
                            continue;
                        if (joystick_id != event.joystick.id)
                            continue;
                        break;
                    }
                    if (joystick_id == NULL)
                        break;
                    joystick_is_installed = true;
                    break;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP && selected > 0) {
                selected--;
                animation = -1;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && selected < 1) {
                selected++;
                animation = 1;
            }

            continue;
        }

        if (event.type != ALLEGRO_EVENT_TIMER) {
            continue;
        }

        if (animation != 0) {
            if (arrow_pos == MENU_HEIGHT+ITEM_HEIGHT*selected)
                animation = 0;
            else {
                arrow_pos += 2*animation;
                if (animation * (arrow_pos - (MENU_HEIGHT+ITEM_HEIGHT*selected)) > 0)
                    arrow_pos = MENU_HEIGHT + ITEM_HEIGHT * selected;
            }
        }

        al_draw_bitmap(box, BOX_WIDTH, BOX_HEIGHT, 0);
        al_draw_bitmap(arrow, ARROW_PAD_X,  arrow_pos, 0);
        al_draw_bitmap(arrow, ARROW_PAD_X2, arrow_pos, ALLEGRO_FLIP_HORIZONTAL);

        flush_buffer();
    }

    if (joystick_id == NULL) {
        selected = 0;
        joystick_is_installed = false;
    }

    al_destroy_event_queue(event_queue);

    al_destroy_bitmap(box);
    al_destroy_bitmap(joy);
    al_destroy_bitmap(arrow);
}
