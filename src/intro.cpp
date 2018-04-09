#include "intro.h"
#include "base.h"
#include "resources.h"

#define SPARKS_COUNT 3
#define SHINE_COUNT 4

void intro() {
    ALLEGRO_BITMAP
        *background = al_load_bitmap(RES_BMP_INTRO),
            *sparks = al_load_bitmap(RES_BMP_SPARKS),
             *shine = al_load_bitmap(RES_BMP_SHINE),
        *sparks_[SPARKS_COUNT],
        *shine_[SHINE_COUNT*2-2];

    for (int i=0; i<SPARKS_COUNT; i++)
        sparks_[i] = GET_SUB_BITMAP(sparks,SPARKS_COUNT,1,i,0);

    for (int i=0; i<SHINE_COUNT; i++)
        shine_[i] = GET_SUB_BITMAP(shine,SHINE_COUNT,1,i,0);

    for (int i=SHINE_COUNT; i<SHINE_COUNT*2-2; i++) // Loops back and forth
        shine_[i] = shine_[SHINE_COUNT*2-2-i];


    const char * rolling_text_str = "(C) 1992 by Filipe Mateus";
    ALLEGRO_BITMAP *rolling_text = al_create_bitmap(
        al_get_text_width(font_script, rolling_text_str),
        al_get_font_line_height(font_script) + 2
    ),
        *gradient = al_load_bitmap(RES_BMP_GRADIENT);

    al_set_target_bitmap(rolling_text);
    al_clear_to_color(HEX_TO_COLOR(0x000000));
    al_draw_text(font_script, HEX_TO_COLOR(0xffffff), 0, 0, 0, rolling_text_str);
    int op, src, dst;
    al_get_blender(&op, &src, &dst);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);
    al_draw_scaled_bitmap(gradient, 0, 0, 1, 13, 0, 0, al_get_bitmap_width(rolling_text), 13, 0);
    al_set_blender(op, src, dst);
    al_destroy_bitmap(gradient);
    al_set_target_backbuffer(display);

    #ifdef ANDROID
        al_draw_bitmap(background, 0, 0, 0);
    #else
        #define UNROLL_STEPS 20
        for (int i=0; i<=UNROLL_STEPS; i++) {
            al_clear_to_color(HEX_TO_COLOR(0x000000));
            al_draw_bitmap_region(background,
                0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT*i/UNROLL_STEPS,
                0, 0, 0
            );
            flush_buffer();
            al_rest(0.3/UNROLL_STEPS);
        }
    #endif

    fade_in(background, DEFAULT_FADE_STEPS);

    ALLEGRO_COLOR sand_color = HEX_TO_COLOR(0xd38251);
    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());

    int sparks_i = 0;
    int shine_i = 0;
    int sand_i = 0;
    int roll_i = 0;
    bool fixed_sand_line = false;

    bool loop_end = false;
    while (!loop_end) {

        while (al_get_next_event(event_queue, &event)) {
            switch (event.type) {
                case ALLEGRO_EVENT_KEY_DOWN:
                case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
                case ALLEGRO_EVENT_TOUCH_END:
                    loop_end = true;
                    break;
                default:
                    continue;
            }
        }

        al_draw_bitmap(background, 0, 0, 0);

        al_draw_bitmap(sparks_[sparks_i], 218, 10, 0);
        sparks_i = BPS_TO_SECS(TICKS) / 0.05; // every each 0.05s
        sparks_i %= SPARKS_COUNT;

        al_draw_bitmap(shine_[shine_i], 16, 16, 0);
        shine_i = BPS_TO_SECS(TICKS) / 0.2;
        shine_i %= SHINE_COUNT*2-2;

        sand_i++;
        if (sand_i >= 4*7) {
            sand_i = 0;
            fixed_sand_line = true;
        }

        if (!fixed_sand_line)
            draw_rect_fill(175, 55, 175, 55+sand_i/4, sand_color);
        else
            draw_rect_fill(175, 55, 175, 61, sand_color);
        if (sand_i%4 != 3)
            draw_rect_fill(
                175, 55+sand_i/4,
                175, 55+sand_i/4,
                HEX_TO_COLOR(0x000000));

        al_draw_bitmap(rolling_text,
            VIRTUAL_SCREEN_WIDTH-roll_i,
            VIRTUAL_SCREEN_HEIGHT-al_get_bitmap_height(rolling_text),
            0);
        roll_i++;
        if (roll_i >= VIRTUAL_SCREEN_WIDTH + al_get_bitmap_width(rolling_text))
            roll_i = 0;

        flush_buffer();
    }

    al_destroy_event_queue(event_queue);

    al_destroy_bitmap(background);
    al_destroy_bitmap(rolling_text);

    for (int i=0; i<SPARKS_COUNT; i++)
        al_destroy_bitmap(sparks_[i]);
    al_destroy_bitmap(sparks);

    for (int i=0; i<SHINE_COUNT; i++)
        al_destroy_bitmap(shine_[i]);
    al_destroy_bitmap(shine);

    fade_out(DEFAULT_FADE_STEPS);
}
