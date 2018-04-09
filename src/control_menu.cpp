#include "control_menu.h"
#include "base.h"
#include "resources.h"

#include <cmath>
#include <cstdio>
#include <ctype.h>

#define MENU_INNER_PADDING_TOP 13
#define MENU_INNER_PADDING_BOTTOM 6

#define ITEM_HEIGHT 17
#define ITEM_COUNT ((int) (sizeof(menu_items)/sizeof(menu_items[0])))

#define MENU_WIDTH 122
#define MENU_HEIGHT (MENU_INNER_PADDING_TOP + ITEM_COUNT * ITEM_HEIGHT + MENU_INNER_PADDING_BOTTOM)

#define MENU_PADDING_LEFT  ((VIRTUAL_SCREEN_WIDTH  - MENU_WIDTH) / 2 )
#define MENU_PADDING_TOP ((VIRTUAL_SCREEN_HEIGHT - MENU_HEIGHT) / 2)

#define ARROW_PAD_Y  (11  + MENU_PADDING_TOP)
#define ARROW_PAD_X  (5   + MENU_PADDING_LEFT)
#define ARROW_PAD_X2 (100 + MENU_PADDING_LEFT)

const char *menu_items[] = {
    "Keyboard"
    ,"Joystick"
#ifdef ANDROID
    ,"Swipe"
    ,"D-Pad"
#endif
};

void control_menu() {
    static int selected = 0;

    switch(controller_type) {
        case JOYSTICK:
            selected = joystick_is_installed ? 1 : 0;
            break;
        case TOUCH_SWIPE:
            selected = 2;
            break;
        case TOUCH_DPAD:
            selected = 3;
            break;
    }

    ALLEGRO_BITMAP *box = al_create_bitmap(MENU_WIDTH, MENU_HEIGHT);
    ALLEGRO_BITMAP *joy = al_create_bitmap(203,65);
    ALLEGRO_BITMAP *arrow = al_load_bitmap(RES_BMP_ARROW);

    al_set_target_bitmap(box);
    draw_rect_frame(0,0,al_get_bitmap_width(box)-1, al_get_bitmap_height(box)-1);

    int x = 32, y = MENU_INNER_PADDING_TOP, dx = 2, dy = 1;
    int i;
    for (i=0; i<ITEM_COUNT; i++) {
        ALLEGRO_COLOR
            font_color = HEX_TO_COLOR(0xffffff),
            shadow_color = HEX_TO_COLOR(0x555555);
        draw_text_shadow(font_script, font_color, shadow_color,
            x, y, dx, dy, 0, menu_items[i]);
        y+=17;
    }

    al_set_target_bitmap(joy);
    draw_rect_frame(0,0,al_get_bitmap_width(joy)-1, al_get_bitmap_height(joy)-1);
    draw_text_shadow(font_script, HEX_TO_COLOR(0xffffff), HEX_TO_COLOR(0x555555),
        21, 10, 0, "Move stick to up and left");
    draw_text_shadow(font_serif, HEX_TO_COLOR(0xffff55), HEX_TO_COLOR(0x555555),
        52, 50, 0, "and press button");
    al_set_target_backbuffer(display);

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    int arrow_pos = ARROW_PAD_Y + ITEM_HEIGHT * selected;
    int animation = 0;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer_fps));

    al_draw_bitmap(box, MENU_PADDING_LEFT, MENU_PADDING_TOP, 0);
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
                    controller_type = JOYSTICK; // fallback
                    break;
                }
                else if (selected == 1) { // JOYSTICK
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
                    controller_type = JOYSTICK;
                    break;
                }
                else if (selected == 2) {
                    controller_type = TOUCH_SWIPE;
                    joystick_is_installed = false;
                    break;
                }
                else if (selected == 3) {
                    controller_type = TOUCH_DPAD;
                    joystick_is_installed = false;
                    break;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP && selected > 0) {
                selected--;
                animation = -1;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && selected < ITEM_COUNT-1) {
                selected++;
                animation = 1;
            }

            continue;
        }

        if (event.type != ALLEGRO_EVENT_TIMER) {
            continue;
        }

        if (animation != 0) {
            if (arrow_pos == ARROW_PAD_Y + ITEM_HEIGHT * selected)
                animation = 0;
            else {
                arrow_pos += 2*animation;
                if (animation * (arrow_pos - (ARROW_PAD_Y + ITEM_HEIGHT * selected)) > 0)
                    arrow_pos = ARROW_PAD_Y + ITEM_HEIGHT * selected;
            }
        }

        al_draw_bitmap(box, MENU_PADDING_LEFT, MENU_PADDING_TOP, 0);
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
