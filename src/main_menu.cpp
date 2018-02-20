#include "main_menu.h"
#include "base.h"
#include "resources.h"
#include "control_menu.h"
#include "hidden_menu.h"

#define MENU_HEIGHT 73
#define ITEM_HEIGHT 17
#define ARROW_PAD_X 80
#define ARROW_PAD_X2 (ARROW_PAD_X + 141)

#define ACTION_KEY_UP {\
    if (selected > 0) {\
        selected--;\
        animation = -1;\
    }\
}

#define ACTION_KEY_DOWN {\
    if (selected < 4) {\
        selected++;\
        animation = 1;\
    }\
}

#define ACTION_KEY_ENTER {\
    if (selected == 2) {\
        control_menu();\
        al_flush_event_queue(event_queue);\
        continue;\
    }\
    else {\
        end_loop = true;\
    }\
}

int main_menu() {
    static int selected = 0;

    ALLEGRO_BITMAP *background = al_load_bitmap(RES_BMP_MENU);
    ALLEGRO_BITMAP *arrow = al_load_bitmap(RES_BMP_ARROW);

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    int arrow_pos = MENU_HEIGHT + ITEM_HEIGHT * selected;
    int animation = 0;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());
    int touch_x = 0, touch_y = 0;

    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(arrow, ARROW_PAD_X,  arrow_pos, 0);
    al_draw_bitmap(arrow, ARROW_PAD_X2, arrow_pos, ALLEGRO_FLIP_HORIZONTAL);
    fade_in(DEFAULT_FADE_STEPS);

    al_flush_event_queue(event_queue);
    bool end_loop = false;
    while (!end_loop) {

        while(al_get_next_event(event_queue, &event)) {

            if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                if (event.keyboard.keycode == ALLEGRO_KEY_F1) {
                    hidden_menu();
                    al_flush_event_queue(event_queue);
                }
                else if ((event.keyboard.keycode == ALLEGRO_KEY_ENTER
                        ||event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER
                        ||event.keyboard.keycode == key_fire))
                    ACTION_KEY_ENTER
                else if (event.keyboard.keycode == ALLEGRO_KEY_UP
                     ||  event.keyboard.keycode == key_up)
                    ACTION_KEY_UP
                else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN
                     ||  event.keyboard.keycode == key_down)
                    ACTION_KEY_DOWN
                else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                     ||  event.keyboard.keycode == ALLEGRO_KEY_BACK) {
                    if (selected == 4)
                        ACTION_KEY_ENTER
                    else {
                        selected = 4;
                        animation = 1;
                    }

                }

                continue;
            }
            else if (event.type == ALLEGRO_EVENT_TOUCH_BEGIN) {
                if (!event.touch.primary) continue;
                touch_x = event.touch.x;
                touch_y = event.touch.y;
            }
            else if (event.type == ALLEGRO_EVENT_TOUCH_END) {
                if (!event.touch.primary) continue;
                touch_x = event.touch.x - touch_x;
                touch_y = event.touch.y - touch_y;
                if (touch_x == 0 && touch_y == 0) {
                    ACTION_KEY_ENTER
                }
                if (abs(touch_x) < abs(touch_y)) {
                    if (abs(touch_y) > 0) {
                        if (touch_y > 0)
                            ACTION_KEY_DOWN
                        else
                            ACTION_KEY_UP
                    }
                }
            }

            else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
                ACTION_KEY_ENTER
                continue;
            }

            else if (joystick_is_installed && animation == 0
                 && event.type == ALLEGRO_EVENT_JOYSTICK_AXIS
                 &&  event.joystick.stick == joystick_vertical_stick
                 &&  event.joystick.axis == joystick_vertical_axis ) {

                if (event.joystick.pos*joystick_vertical_positive > 0.5)
                    ACTION_KEY_UP
                else if (event.joystick.pos*joystick_vertical_positive < -0.5)
                    ACTION_KEY_DOWN

                continue;
            }
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

        al_draw_bitmap(background, 0, 0, 0);
        al_draw_bitmap(arrow, ARROW_PAD_X,  arrow_pos, 0);
        al_draw_bitmap(arrow, ARROW_PAD_X2, arrow_pos, ALLEGRO_FLIP_HORIZONTAL);

        flush_buffer();

    }

    al_destroy_event_queue(event_queue);

    al_destroy_bitmap(background);
    al_destroy_bitmap(arrow);

    if (selected != 4) { // Fades on leaving menu
        fade_out(DEFAULT_FADE_STEPS);
        return selected+1;
    }
    else { // Don't fade on quit
        return EXIT_PROGRAM;
    }
}
