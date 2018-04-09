#ifndef BASE_H
#define BASE_H

#if defined _WIN32 || defined __WIN32 || defined __WIN32__ || defined WIN32 || defined __MINGW32__
#define WINDOWS 1
#elif defined __ANDROID__ && !defined ANDROID
#define ANDROID 1
#endif

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
// #include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#ifdef ANDROID
    #define ALLEGRO_UNSTABLE
    #include <allegro5/allegro_android.h>
#endif

#define SYSTEM_BPS 70
#ifdef WINDOWS
#define TARGET_FRAMERATE 60
#else
#define TARGET_FRAMERATE 60
#endif

#define SECS_TO_BPS(x) ((int)((x)*SYSTEM_BPS))
#define BPS_TO_SECS(x) ((double)(x)/SYSTEM_BPS)

#define TICKS al_get_timer_count(timer)

#define EXIT_PROGRAM -1

#define VIRTUAL_SCREEN_WIDTH  320
#define VIRTUAL_SCREEN_HEIGHT 200

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

#define HEX_TO_RGB(x) (((x)>>16)&0xff), (((x)>>8)&0xff), ((x)&0xff)
#define HEX_TO_COLOR(x) al_map_rgb(HEX_TO_RGB(x))

#define GET_SUB_BITMAP(b,x,y,i,j) al_create_sub_bitmap(b,(i)*al_get_bitmap_width(b)/(x),(j)*al_get_bitmap_height(b)/(y),al_get_bitmap_width(b)/(x),al_get_bitmap_height(b)/(y))

extern volatile int screen_width;
extern volatile int screen_height;
extern volatile bool is_fullscreen;
extern volatile bool fullscreen_override;
extern volatile bool windowed_override;
extern volatile bool mute;
extern volatile bool mute_override;
extern volatile bool keyboard_displayed;
extern volatile bool background_mode;
extern ALLEGRO_TIMER *timer, *timer_fps;
extern ALLEGRO_EVENT_QUEUE *event_fps;
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_FONT *font_large, *font_small, *font_script, *font_serif;
extern ALLEGRO_THREAD *thread_display_monitor;
extern ALLEGRO_TRANSFORM transform;

extern bool joystick_is_installed;
extern ALLEGRO_JOYSTICK *joystick_id;
extern char joystick_name[256];
extern int joystick_vertical_stick;
extern int joystick_vertical_axis;
extern int joystick_vertical_positive;
extern int joystick_horizontal_stick;
extern int joystick_horizontal_axis;
extern int joystick_horizontal_positive;

typedef enum  {
    JOYSTICK,
    TOUCH_SWIPE,
    TOUCH_DPAD
} controller_type_t;

extern controller_type_t controller_type;

extern int key_up, key_down, key_left, key_right, key_fire;

void abort_on_error(const char *message);
void warning(const char *message);

void unset_transform();
void reset_transform();
void flush_buffer();
ALLEGRO_BITMAP * clone_backbuffer();

#define DEFAULT_FADE_STEPS 25
void fade_in(ALLEGRO_BITMAP* bmp, int steps);
void fade_in(int steps);
void fade_out(ALLEGRO_BITMAP* bmp, int steps);
void fade_out(int steps);

void draw_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void draw_rect(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void draw_rect_fill(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void draw_rect_texture(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP *texture);
void draw_rect_frame(int x1, int y1, int x2, int y2);

void draw_text_shadow (ALLEGRO_FONT *font, ALLEGRO_COLOR color_text,
    ALLEGRO_COLOR color_shadow, int x, int y, int dx, int dy,
    int flags, const char *text);
void draw_text_shadow (ALLEGRO_FONT *font, ALLEGRO_COLOR color_text,
    ALLEGRO_COLOR color_shadow, int x, int y, int flags, const char *text);

void upper_case_string(char *str);
void remove_space(char *str);

void show_keyboard();
void hide_keyboard();

#endif // BASE_H
