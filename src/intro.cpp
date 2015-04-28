#include "intro.h"
#include "base.h"

#define SPARKS_COUNT 3
#define SHINE_COUNT 4

void intro() {
    ALLEGRO_BITMAP
        *background = al_load_bitmap(RES_BMP_INTRO),
            *sparks = al_load_bitmap(RES_BMP_SPARKS),
             *shine = al_load_bitmap(RES_BMP_SHINE),
                *by = al_load_bitmap("res/intro_by.png"),
        *sparks_[SPARKS_COUNT],
        *shine_[SHINE_COUNT*2-2];

    for (int i=0; i<SPARKS_COUNT; i++)
        sparks_[i] = GET_SUB_BITMAP(sparks,SPARKS_COUNT,1,i,0);

    for (int i=0; i<SHINE_COUNT; i++)
        shine_[i] = GET_SUB_BITMAP(shine,SHINE_COUNT,1,i,0);

    for (int i=SHINE_COUNT; i<SHINE_COUNT*2-2; i++) // BACK AND FORTH
        shine_[i] = shine_[SHINE_COUNT*2-2-i];

    #define TIRO_LIRO 20
    for (int i=0; i<TIRO_LIRO; i++) {
        al_clear_to_color(HEX_TO_COLOR(0x000000));
        al_draw_bitmap_region(background, 0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT*i/TIRO_LIRO, 0, 0, 0);
        flush_buffer();
        al_rest(0.3/TIRO_LIRO);
    }

    fade_in(background, DEFAULT_FADE_STEPS);

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;
    ALLEGRO_COLOR sand;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer_fps));


    int sparks_i = 0;
    int shine_i = 0;
    int sand_i = 0;
    int sand_start = 0;
    int roll_i = 0;
	al_flush_event_queue(event_queue);
    while (true) {

        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
            break;

        if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
            break;

        if (event.type != ALLEGRO_EVENT_TIMER)
            continue;

        al_draw_bitmap(background, 0, 0, 0);

        al_draw_bitmap(sparks_[sparks_i], 218, 10, 0);
        sparks_i = BPS_TO_SECS(TICKS) / 0.05; // every each 0,05 s
        sparks_i %= SPARKS_COUNT;

        al_draw_bitmap(shine_[shine_i], 16, 16, 0);
        shine_i = BPS_TO_SECS(TICKS) / 0.2;
        shine_i %= SHINE_COUNT*2-2;

        if (BPS_TO_SECS(TICKS-sand_start) >= 0.01) {// every each 0,05 s
            sand_i = (sand_i+1)%(4*7);
            sand_start = TICKS;
        }

        if (sand_i%4==3)
            sand = HEX_TO_COLOR(0xd38251);
        else
            sand = HEX_TO_COLOR(0x000000);

        al_set_target_bitmap(background);
        al_put_pixel(175, 55+sand_i/4, sand);
        al_set_target_bitmap(buffer);

        al_draw_bitmap(by,
            VIRTUAL_SCREEN_WIDTH-roll_i,
            VIRTUAL_SCREEN_HEIGHT-al_get_bitmap_height(by),
            0);
        roll_i++;
        roll_i %= VIRTUAL_SCREEN_WIDTH + al_get_bitmap_width(by);

        flush_buffer2();
    }

    al_destroy_event_queue(event_queue);

    al_destroy_bitmap(background);
    al_destroy_bitmap(by);

    for (int i=0; i<SPARKS_COUNT; i++)
        al_destroy_bitmap(sparks_[i]);
    al_destroy_bitmap(sparks);

    for (int i=0; i<SHINE_COUNT; i++)
        al_destroy_bitmap(shine_[i]);
    al_destroy_bitmap(shine);

    fade_out(DEFAULT_FADE_STEPS);
}
