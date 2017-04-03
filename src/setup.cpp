#include "setup.h"
#include "base.h"

#include <cstdio>
#include <cstring>

void setup() {
	// Allegro Library setup
    if (!al_install_system(ALLEGRO_VERSION_INT, NULL))
        abort_on_error("Fail to start Allegro");

    if (!al_install_keyboard())
        abort_on_error("Fail to start Allegro keyboard drivers");

    //if (!al_install_mouse())
    //    abort_on_error("Fail to start Allegro mouse drivers");

    if (!al_install_joystick())
        warning("Fail to start Allegro mouse drivers");

    // Allegro add-ons setup
    if (!al_init_image_addon())
    	abort_on_error("Fail to start Allegro image add-on");

    al_init_font_addon();

    //if (!al_init_ttf_addon())
    //    abort_on_error("Fail to start Allegro TTF add-on");

    if (!al_install_audio())
        abort_on_error("Fail to start Allegro audio add-on");

    if (!al_init_acodec_addon())
        abort_on_error("Fail to start Allegro audio codec add-on");

    if (!al_reserve_samples(1))
        abort_on_error("Fail to allocate Allegro audio channel");

    if (!al_init_primitives_addon())
        abort_on_error("Fail to start Allegro primitives add-on");

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
        abort_on_error("Fail to create window");
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
    // Sets up bitmap buffer
    int old_format = al_get_new_bitmap_format();
    #ifdef USE_VIDEO_BITMAP_BUFFER
    al_set_new_bitmap_flags(old_format | ALLEGRO_VIDEO_BITMAP);
    #else
    #warning "Not using video bitmap buffer"
    #endif

    buffer = al_create_bitmap(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    if (!buffer)
        abort_on_error("Fail to create video buffer");
    al_set_target_bitmap(buffer);
    al_set_new_bitmap_flags(old_format);

    //al_clear_to_color(HEX_TO_COLOR(0x000000));
    //flush_buffer();

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
}

void shutdown() {
    al_destroy_font(font_large);
    al_destroy_font(font_small);
    al_destroy_bitmap(buffer);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_fps);
    al_destroy_timer(timer_fps);
	al_uninstall_system();
}
