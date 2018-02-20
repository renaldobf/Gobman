#include "base.h"

#include <cstdio>
#include <ctype.h>

volatile int screen_width;
volatile int screen_height;
volatile bool is_fullscreen;
volatile bool fullscreen_override = false;
volatile bool windowed_override = false;
volatile bool mute = false;
volatile bool mute_override = false;
volatile bool keyboard_displayed = false;
volatile bool background_mode = false;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *buffer;
ALLEGRO_TIMER *timer, *timer_fps;
ALLEGRO_EVENT_QUEUE *event_fps;
ALLEGRO_FONT *font_large, *font_small;
ALLEGRO_THREAD *thread_display_monitor;

bool joystick_is_installed;
ALLEGRO_JOYSTICK *joystick_id;
char joystick_name[256];
int joystick_vertical_stick;
int joystick_vertical_axis;
int joystick_vertical_positive;
int joystick_horizontal_stick;
int joystick_horizontal_axis;
int joystick_horizontal_positive;


int key_up, key_down, key_left, key_right, key_fire;

void abort_on_error(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(-1);
}

void warning(const char *message) {
    fprintf(stderr, "%s\n", message);
}

void flush_buffer() {
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT,
        0, 0, screen_width, screen_height, 0);
    al_set_target_bitmap(buffer);
    al_wait_for_event(event_fps, NULL);
    al_flush_event_queue(event_fps);
    if (!background_mode) al_flip_display();
}

/*
void flush_buffer2(ALLEGRO_EVENT_QUEUE *event_queue) { // do no wait for fps event queue
    _flush_buffer();
    if (event_queue) {
        ALLEGRO_EVENT event;
        while (al_peek_next_event(event_queue, &event) && event.type == ALLEGRO_EVENT_TIMER)
            al_drop_next_event(event_queue);
    }
}
*/

void fade_in(ALLEGRO_BITMAP* bmp, int steps) {
    for (int i=0; i<=steps; i++) {
        al_draw_tinted_bitmap(bmp,
            al_map_rgb_f((float) i/steps, (float) i/steps, (float) i/steps),
            0, 0, 0);
        flush_buffer();
    }
}

void fade_in(int steps) {
    ALLEGRO_BITMAP *tmp = al_clone_bitmap(buffer);
    fade_in(tmp, steps);
    al_destroy_bitmap(tmp);
}

void fade_out(ALLEGRO_BITMAP* bmp, int steps) {
    for (int i=steps; i>=0; i--) {
        al_draw_tinted_bitmap(bmp,
            al_map_rgb_f((float) i/steps, (float) i/steps, (float) i/steps),
            0, 0, 0);
        flush_buffer();
    }
}

void fade_out(int steps) {
    ALLEGRO_BITMAP *tmp = al_clone_bitmap(buffer);
    fade_out(tmp, steps);
    al_destroy_bitmap(tmp);
}

void draw_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {
    al_draw_line(x1+0.5, y1+0.5, x2+0.5, y2+0.5, color, 1.0);
}

void draw_rect(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {
    al_draw_rectangle(x1+0.5, y1+0.5, x2+0.5, y2+0.5, color, 1.0);
}

void draw_rect_fill(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {
    al_draw_filled_rectangle(x1, y1, x2+1.0, y2+1.0, color);
}

void draw_text_shadow (ALLEGRO_FONT *font, ALLEGRO_COLOR color_text,
    ALLEGRO_COLOR color_shadow, int x, int y, int flags, const char *text) {

    al_draw_text(font, color_shadow, x+1, y+1, flags, text);
    al_draw_text(font, color_text  , x  , y  , flags, text);
}

void upper_case_string(char *str) {
    while (*str != '\0') {
        *str = toupper(*str);
        str++;
    }
}

void remove_space(char *str) {
    int i=0, j=0;
    while( str[j] != '\0' ) {
        if (isspace(str[j])) {
            if (j == 0 || isspace(str[j-1])) {
                j++;
                continue;
            }
            str[i] = ' ';
        }
        else {
            str[i] = str[j];
        }
        i++; j++;
    }
    if (i>0 && str[i-1] == ' ')
        i--;
    str[i] = '\0';
}

void show_keyboard() {
    #ifdef ANDROID
    JNIEnv *env = al_android_get_jni_env();
    jclass class_id = env->GetObjectClass(al_android_get_activity());
    jmethodID method_id = env->GetMethodID(class_id, "openKeyboard", "()V");
    env->CallVoidMethod(al_android_get_activity(), method_id);
    #endif
    keyboard_displayed = true;
}

void hide_keyboard() {
    if (!keyboard_displayed) return;
    #ifdef ANDROID
    JNIEnv *env = al_android_get_jni_env();
    jclass class_id = env->GetObjectClass(al_android_get_activity());
    jmethodID method_id = env->GetMethodID(class_id, "closeKeyboard", "()V");
    env->CallVoidMethod(al_android_get_activity(), method_id);
    #endif
    keyboard_displayed = false;
}

void close_application() {
    #ifdef ANDROID
    JNIEnv *env = al_android_get_jni_env();
    jclass class_id = env->GetObjectClass(al_android_get_activity());
    jmethodID method_id = env->GetMethodID(class_id, "closeApplication", "()V");
    env->CallVoidMethod(al_android_get_activity(), method_id);
    #endif
}
