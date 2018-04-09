#include "setup.h"
#include "base.h"
#include "resources.h"

#include <cstdio>
#include <cstring>
#include <ctime>

int get_next_power_of_2(int val) {
    val--;
    int p = 0;
    while (val) { val/=2; p++; }
    val = 1;
    while (p--) val*=2;
    return val;
}

void* display_monitor(ALLEGRO_THREAD *self, void *args);

void setup() {
    // Allegro Library setup
    if (!al_install_system(ALLEGRO_VERSION_INT, NULL))
        abort_on_error("Failed to start Allegro");

    if (!al_install_keyboard())
        abort_on_error("Failed to start Allegro keyboard drivers");

    //if (!al_install_mouse())
    //	abort_on_error("Failed to start Allegro mouse drivers");

    if (!al_install_joystick())
        warning("Failed to start Allegro joystick drivers");

    if (!al_install_touch_input())
        warning("Failed to start Allegro touch input");

    #ifdef ANDROID
    al_android_set_apk_file_interface();
    #endif

    // Allegro add-ons setup
    if (!al_init_image_addon())
        abort_on_error("Failed to start Allegro image add-on");

    al_init_font_addon();

    //if (!al_init_ttf_addon())
    //	abort_on_error("Failed to start Allegro TTF add-on");

    if (!al_install_audio())
        abort_on_error("Failed to start Allegro audio add-on");

    if (!al_init_acodec_addon())
        abort_on_error("Failed to start Allegro audio codec add-on");

    if (!al_reserve_samples(1))
        abort_on_error("Failed to allocate Allegro audio channel");

    if (!al_init_primitives_addon())
        abort_on_error("Failed to start Allegro primitives add-on");

    // Allegro setup ends successfully

    //ALLEGRO_DISPLAY_MODE mode;
    //al_get_display_mode(0, &mode);

    ALLEGRO_MONITOR_INFO monitor;
    al_get_monitor_info(0, &monitor);

    // Create window (ALLEGRO_DISPLAY)
    if ((is_fullscreen || fullscreen_override) && !windowed_override) {
        screen_width  = monitor.x2; /*mode.width*/
        screen_height = monitor.y2; /*mode.height*/
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    }
    else {
        screen_width  = WINDOW_WIDTH;
        screen_height = WINDOW_HEIGHT;
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    }

    display = al_create_display(screen_width, screen_height);
    if (!display)
        abort_on_error("Failed to create window");

    hide_keyboard();
    al_hide_mouse_cursor(display);

    // Loads application icon
    ALLEGRO_BITMAP *icon = al_load_bitmap(RES_ICON);
    if (icon) {
        al_set_display_icon(display, icon);
        al_destroy_bitmap(icon);
    }
    al_set_window_title(display, APPLICATION_NAME);

    // Sets up timer
    timer = al_create_timer(1.0/SYSTEM_BPS);
    al_start_timer(timer);

    timer_fps = al_create_timer(1.0/TARGET_FRAMERATE);
    al_start_timer(timer_fps);

    event_fps = al_create_event_queue();
    al_register_event_source(event_fps, al_get_timer_event_source(timer_fps));

    // Resets the transformation matrix
    reset_transform();

    al_clear_to_color(al_map_rgb(0,0,0));
    flush_buffer();

    if (joystick_is_installed) {
        int i, num_joysticks=al_get_num_joysticks();
        for (i=0; i<num_joysticks; i++) {
            joystick_id = al_get_joystick(i);
            char joy_name[256];
            strcpy(joy_name, al_get_joystick_name(joystick_id));
            remove_space(joy_name);
            if (strcmp(joy_name, joystick_name) == 0)
                break;
        }
        if (i == num_joysticks) {
            joystick_id = NULL;
            joystick_is_installed = false;
        }
    }

    // Sound
    if (mute || mute_override)
        al_set_mixer_gain(al_get_default_mixer(), 0.0);
    else
        al_set_mixer_gain(al_get_default_mixer(), 1.0);

    // Load fonts
    { // Large font
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_LARGE);
        int ranges[] = {0x20,0x7f}, num_ranges=1;
        font_large = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }
    { // Small font
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_SMALL);
        int ranges[] = {'A','Z',
                        ' ','!',
                        '.','.',
                        ',',',',
                        '0','9'}, num_ranges=5;
        font_small = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }
    { // Script font
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_SCRIPT);
        int ranges[] = {0x20,0x7f}, num_ranges=1;
        font_script = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }
    { // Serif font
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_SERIF);
        int ranges[] = {' ',' ',
                        'a','z'}, num_ranges=2;
        font_serif = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }

    thread_display_monitor = al_create_thread(display_monitor, NULL);
    al_start_thread(thread_display_monitor);

    /* Seed the random number generator. */
    srand((unsigned)time(NULL));
}

void shutdown() {

    al_set_thread_should_stop(thread_display_monitor);
    al_join_thread(thread_display_monitor, NULL);
    al_destroy_thread(thread_display_monitor);

    al_destroy_font(font_large);
    al_destroy_font(font_small);
    al_destroy_font(font_script);
    al_destroy_font(font_serif);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_fps);
    al_destroy_timer(timer_fps);
    al_uninstall_system();
}

void* display_monitor(ALLEGRO_THREAD *self, void *args) {
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    // al_register_event_source(event_queue, al_get_display_event_source(display));

    while (!al_get_thread_should_stop(self)) {
        if (!al_wait_for_event_timed(event_queue, &event, 0.1))
            continue;
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // ?
                break;

             case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                background_mode = true;
                al_stop_timer(timer);
                al_stop_timer(timer_fps);
                al_flush_event_queue(event_fps);
                al_set_default_voice(NULL);
                al_acknowledge_drawing_halt(display);
                break;

             case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                al_acknowledge_drawing_resume(display);
                background_mode = false;
                al_start_timer(timer);
                al_start_timer(timer_fps);
                al_restore_default_mixer();
                break;

             case ALLEGRO_EVENT_DISPLAY_RESIZE:
                /*
                screen_width = event.display.width;
                screen_height = event.display.height;
                al_resize_display(display, screen_width, screen_height);
                */
                al_acknowledge_resize(display);
                break;
        }
    }

    al_destroy_event_queue(event_queue);
    return NULL;
}
