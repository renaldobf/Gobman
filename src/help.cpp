#include "help.h"
#include "base.h"
#include "resources.h"
#include "level/level.h" // Tile width/height

#define GO_NEXT 0
#define GO_END  1

int wait_input(ALLEGRO_EVENT_QUEUE *event_queue) {
    ALLEGRO_EVENT event;
    al_flush_event_queue(event_queue);
    while (true) {
        while (al_get_next_event(event_queue, &event)) {
            switch (event.type) {
                case ALLEGRO_EVENT_KEY_DOWN:
                    if (event.keyboard.keycode == ALLEGRO_KEY_ENTER
                        || event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER
                        || event.keyboard.keycode == key_fire
                    ) {
                        return GO_NEXT;
                    }
                    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                    ||  event.keyboard.keycode == ALLEGRO_KEY_BACK)
                        return GO_END;
                    break;
                case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
                case ALLEGRO_EVENT_TOUCH_END:
                    return GO_NEXT;
                    break;
            }
        }
        flush_buffer();
    }
    return GO_NEXT;
}

void help() {

    ALLEGRO_BITMAP *gobman = al_load_bitmap(RES_BMP_GOBMAN),
                   *ghosts = al_load_bitmap(RES_BMP_GHOSTS),
                  *objects = al_load_bitmap(RES_BMP_OBJECTS),
                  *candies = al_load_bitmap(RES_BMP_CANDIES);

    ALLEGRO_COLOR color_text = HEX_TO_COLOR(0xffff55),
                color_shadow = HEX_TO_COLOR(0x9e9e00),
                       white = HEX_TO_COLOR(0xffffff);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());

    al_clear_to_color(HEX_TO_COLOR(0x000000));

    al_draw_bitmap_region(gobman, TILE_WIDTH*2, 0, TILE_WIDTH, TILE_HEIGHT, 105, 18, 0);

    for (int i=0; i<4; i++) {
        al_draw_bitmap_region(ghosts, TILE_WIDTH*i, 0, TILE_WIDTH, TILE_HEIGHT, 141+18*i, 18, 0);
        draw_rect_fill(144+18*i,20, 145+18*i,21,HEX_TO_COLOR(0x000000));
        draw_rect_fill(149+18*i,20, 150+18*i,21,HEX_TO_COLOR(0x000000));
    }

    al_draw_bitmap_region(objects, TILE_WIDTH*1, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*0, 0);
    al_draw_bitmap_region(candies, TILE_WIDTH*0, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*1, 0);
    al_draw_bitmap_region(objects, TILE_WIDTH*3, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*2, 0);
    al_draw_bitmap_region(objects, TILE_WIDTH*5, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*3, 0);

    draw_text_shadow(font_large, color_text, color_shadow, 97, 5, ALLEGRO_ALIGN_LEFT, "You");
    draw_text_shadow(font_large, color_text, color_shadow, 144, 5, ALLEGRO_ALIGN_LEFT, "Enemies");

    draw_text_shadow(font_large, color_text, color_shadow, 54,  42, ALLEGRO_ALIGN_LEFT, "Power pill");
    draw_text_shadow(font_large, color_text, color_shadow, 54,  74, ALLEGRO_ALIGN_LEFT, "Object");
    draw_text_shadow(font_large, color_text, color_shadow, 54, 106, ALLEGRO_ALIGN_LEFT, "Bomb");
    draw_text_shadow(font_large, color_text, color_shadow, 54, 138, ALLEGRO_ALIGN_LEFT, "Hourglass");

    al_draw_text(font_small, white, 61, 54, ALLEGRO_ALIGN_LEFT, "GIVES YOU THE POWER TO KILL ENEMIES");
    al_draw_text(font_small, white, 61, 60, ALLEGRO_ALIGN_LEFT, "BY EATING THEM.");

    al_draw_text(font_small, white, 61, 86, ALLEGRO_ALIGN_LEFT, "A DIFFERENT ONE APPEARS AT EACH LEVEL,");
    al_draw_text(font_small, white, 61, 92, ALLEGRO_ALIGN_LEFT, "AND IS WORTH 200 POINTS.");

    al_draw_text(font_small, white, 61, 118, ALLEGRO_ALIGN_LEFT, "ACCUMULATE THEM TO KILL ALL ENEMIES");
    al_draw_text(font_small, white, 61, 124, ALLEGRO_ALIGN_LEFT, "BY PRESSING FIRE BUTTON OR SPACEBAR.");

    al_draw_text(font_small, white, 61, 150, ALLEGRO_ALIGN_LEFT, "CAUSES THE ENEMIES TO STOP FOR 20 SECS");
    al_draw_text(font_small, white, 61, 156, ALLEGRO_ALIGN_LEFT, "BUT THEY STILL REMAIN LETHAL!");

    draw_text_shadow(font_small,
        HEX_TO_COLOR(0x55ff55),
        HEX_TO_COLOR(0x00aa00),
        VIRTUAL_SCREEN_WIDTH/2-4, 180, ALLEGRO_ALIGN_CENTER,
        "PRESS FIRE BUTTON OR ENTER");

    fade_in(DEFAULT_FADE_STEPS);

    if (wait_input(event_queue) == GO_END) goto leave_screen;

    al_clear_to_color(HEX_TO_COLOR(0x000000));

    al_draw_bitmap_region(objects, TILE_WIDTH*2, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*0, 0);
    al_draw_bitmap_region(objects, TILE_WIDTH*4, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*1, 0);
    al_draw_bitmap_region(objects, TILE_WIDTH*6, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*2, 0);
    al_draw_bitmap_region(objects, TILE_WIDTH*7, 0, TILE_WIDTH, TILE_HEIGHT, 35, 40+32*3, 0);

    draw_text_shadow(font_large, color_text, color_shadow, 54,  42, ALLEGRO_ALIGN_LEFT, "Red pill");
    draw_text_shadow(font_large, color_text, color_shadow, 54,  74, ALLEGRO_ALIGN_LEFT, "Case");
    draw_text_shadow(font_large, color_text, color_shadow, 54, 106, ALLEGRO_ALIGN_LEFT, "Door");
    draw_text_shadow(font_large, color_text, color_shadow, 54, 138, ALLEGRO_ALIGN_LEFT, "1 Up");

    al_draw_text(font_small, white, 61, 54, ALLEGRO_ALIGN_LEFT, "CAUSES THE WALLS OF THE MAZE TO");
    al_draw_text(font_small, white, 61, 60, ALLEGRO_ALIGN_LEFT, "DISAPPEAR FOR A SHORT TIME.");

    al_draw_text(font_small, white, 61, 86, ALLEGRO_ALIGN_LEFT, "GIVES YOU 5 BOMBS.");
    //al_draw_text(font_small, white, 61, 92, ALLEGRO_ALIGN_LEFT, "");

    al_draw_text(font_small, white, 61, 118, ALLEGRO_ALIGN_LEFT, "GETS YOU TO THE NEXT LEVEL.");
    //al_draw_text(font_small, white, 61, 124, ALLEGRO_ALIGN_LEFT, "");

    al_draw_text(font_small, white, 61, 150, ALLEGRO_ALIGN_LEFT, "GIVES YOU AN EXTRA LIFE.");
    //al_draw_text(font_small, white, 61, 156, ALLEGRO_ALIGN_LEFT, "");

    draw_text_shadow(font_small,
        HEX_TO_COLOR(0x55ff55),
        HEX_TO_COLOR(0x00aa00),
        VIRTUAL_SCREEN_WIDTH/2-4, 180, ALLEGRO_ALIGN_CENTER,
        "PRESS FIRE BUTTON OR ENTER");

    if (wait_input(event_queue) == GO_END) goto leave_screen;

    al_clear_to_color(HEX_TO_COLOR(0x000000));

    al_draw_text(font_large, color_text, 50, 30, ALLEGRO_ALIGN_LEFT,
        "You earn an extra life for");
    al_draw_text(font_large, color_text, 50, 42, ALLEGRO_ALIGN_LEFT,
        "every 10000 points.");

    al_draw_text(font_large, white, 50, 70, ALLEGRO_ALIGN_LEFT,
        "Hotkeys during the game:");

    draw_line(50, 80, 256, 80, HEX_TO_COLOR(0xaa0000));

    draw_text_shadow(font_large, color_text, color_shadow, 60,  90, ALLEGRO_ALIGN_LEFT, "S");
    draw_text_shadow(font_large, color_text, color_shadow, 60, 100, ALLEGRO_ALIGN_LEFT, "P");
    draw_text_shadow(font_large, color_text, color_shadow, 60, 110, ALLEGRO_ALIGN_LEFT, "ESC");
    draw_text_shadow(font_large, color_text, color_shadow, 60, 120, ALLEGRO_ALIGN_LEFT, "SpaceBar");
    draw_text_shadow(font_large, color_text, color_shadow, 60, 140, ALLEGRO_ALIGN_LEFT, "FireButton");

    al_draw_text(font_large, color_text,  78,  90, ALLEGRO_ALIGN_LEFT, "Toggle Sound");
    al_draw_text(font_large, color_text,  78, 100, ALLEGRO_ALIGN_LEFT, "Pause");
    al_draw_text(font_large, color_text,  96, 110, ALLEGRO_ALIGN_LEFT, "Return to Main Menu");
    al_draw_text(font_large, color_text,  87, 130, ALLEGRO_ALIGN_LEFT, "or");
    al_draw_text(font_large, color_text, 168, 140, ALLEGRO_ALIGN_LEFT, "Use bomb");

    draw_text_shadow(font_small,
        HEX_TO_COLOR(0x55ff55),
        HEX_TO_COLOR(0x00aa00),
        VIRTUAL_SCREEN_WIDTH/2-4, 180, ALLEGRO_ALIGN_CENTER,
        "PRESS FIRE BUTTON OR ENTER");

    if (wait_input(event_queue) == GO_END) goto leave_screen;

    leave_screen:

    al_destroy_bitmap(gobman);
    al_destroy_bitmap(ghosts);
    al_destroy_bitmap(objects);
    al_destroy_bitmap(candies);

    fade_out(DEFAULT_FADE_STEPS);
}
