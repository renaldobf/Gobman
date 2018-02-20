#include "level.h"
#include "load.h"
#include "update_logics.h"
#include "../hiscore.h"

#include <cmath>
#include <cstdio>

// GOBMAN VARS
ALLEGRO_BITMAP *gobman_sprite;
ALLEGRO_BITMAP *gobman[GOBMAN_FRAMES];
ALLEGRO_BITMAP *gobmanv[GOBMAN_FRAMES];
struct pair gobman_pos;
struct pair gobman_coord;
direction   gobman_dir;
direction   next_dir;
int         gobman_frame;

// GHOSTS VARS
ALLEGRO_BITMAP *ghosts_sprite;
ALLEGRO_BITMAP *ghosts[GHOST_COUNT+1];
struct pair ghosts_pos[GHOST_COUNT];
struct pair ghosts_coord[GHOST_COUNT];
direction   ghosts_dir[GHOST_COUNT];
bool        is_ghost_dead[GHOST_COUNT];
int         num_ghosts_alive;
int         next_ghost_out;

// GRAPHICS AND SFX
ALLEGRO_BITMAP *label_score;
ALLEGRO_BITMAP *label_level;

ALLEGRO_SAMPLE *sfx_dot,    *sfx_power, *sfx_death, *sfx_ghost,   *sfx_candy,
               *sfx_object, *sfx_spawn, *sfx_oneup, *sfx_redpill, *sfx_bomb;
ALLEGRO_FONT *font_score, *font_points;
ALLEGRO_COLOR black, white;

ALLEGRO_BITMAP *objects, *object[OBJECT_COUNT], *candies, *candy;
ALLEGRO_BITMAP *walls, *tinted_walls, *explosion;
ALLEGRO_BITMAP *map_bitmap;
ALLEGRO_BITMAP *tileset[TILE_COUNT];

signed char *map;

// TIMING VARS
float time_power;
float time_next_ghost;

// TIMING VARS
int ticks_power;
int ticks_freeze;
int ticks_next_ghost;
int ticks_object[OBJECT_COUNT];
int ticks_candy;
int ticks_redpill;

struct pair candy_pos, object_pos[OBJECT_COUNT];

// SCORE VARS
int level;
int lives;
int bombs;
int score;
int ghost_multiplier;

// LOGIC FLAGS
bool is_running;
bool flag_game_over;
bool flag_update_score;
bool flag_lost_life;
bool flag_stop_logics;
bool flag_abort;
bool flag_redpill;
bool flag_bomb;
bool flag_door;

bool is_free_gobman(int x, int y, direction dir) {
    switch (dir) {
        case    UP: y--; break;
        case  DOWN: y++; break;
        case  LEFT: x--; break;
        case RIGHT: x++; break;
    }
    if (x < 0 || x > MAP_WIDTH-1) return true;
    if (y < 0 || y > MAP_HEIGHT-1) return true;

    return map[y*MAP_WIDTH + x] < 3;
}

bool is_free_gobman(struct pair pos, direction dir) {
    return is_free_gobman(pos.x, pos.y, dir);
}

bool is_free_ghosts(int x, int y, direction dir) {
    switch (dir) {
        case    UP: y--; break;
        case  DOWN: y++; break;
        case  LEFT: x--; break;
        case RIGHT: x++; break;
    }

    if (x < 0 || x > MAP_WIDTH-1) return false;
    if (y < 0 || y > MAP_HEIGHT-1) return false;

    return map[y*MAP_WIDTH + x] >= 0 && map[y*MAP_WIDTH + x] < 3;
}

bool is_free_ghosts(struct pair pos, direction dir) {
    return is_free_ghosts(pos.x, pos.y, dir);
}

void draw_gobman() {
    #define frame abs((gobman_frame/GOBMAN_FRAME_STEP) - (GOBMAN_FRAMES*2-2)/2)
    #define gx (gobman_coord.x)
    #define gy (gobman_coord.y)
    switch (gobman_dir) {
        case UP:
            al_draw_bitmap(gobmanv[frame], gx, gy, 0);
            break;
        case DOWN:
            al_draw_bitmap(gobmanv[frame], gx, gy, ALLEGRO_FLIP_VERTICAL);
            break;
        case LEFT:
            al_draw_bitmap(gobman[frame],  gx, gy, ALLEGRO_FLIP_HORIZONTAL);
            break;
        case RIGHT:
            al_draw_bitmap(gobman[frame],  gx, gy, 0);
            break;
    }
    #undef frame
    #undef gx
    #undef gy
}

void draw_ghosts() {
    int sprite;
    for (int i=0; i<GHOST_COUNT; i++) {
        // IDDLE ANIMATION -- fix
        if (is_ghost_dead[i]) {
            sprite = i;
        }
        // ALIVE GHOSTS
        else if (ticks_power == 0)
            sprite = i;
        // POWER ENDING BLINKY GHOSTS
        else if (ticks_power < SECS_TO_BPS(TIME_BLINK_GHOST)) {
            if (ticks_power % (2*SECS_TO_BPS(TIME_BLINK_INTERVAL)) < SECS_TO_BPS(TIME_BLINK_INTERVAL))
                sprite = GHOST_COUNT;
            else
                sprite = i;
        }
        // WEAK GHOSTS
        else {
            sprite = GHOST_COUNT;
        }

        // DRAW GHOST
        if (ticks_freeze == 0 || !is_ghost_dead[i])
            al_draw_bitmap(ghosts[sprite], ghosts_coord[i].x, ghosts_coord[i].y, 0);

        // DRAW EYES
        if ((ticks_power == 0 && ticks_freeze == 0) || is_ghost_dead[i]) {
            int eye_x = ghosts_coord[i].x+3,
                eye_y = ghosts_coord[i].y+3;
            switch (ghosts_dir[i]) {
                case    UP: eye_y--; break;
                case  DOWN: eye_y++; break;
                case  LEFT: eye_x--; break;
                case RIGHT: eye_x++; break;
            }
            draw_rect_fill(eye_x,   eye_y, eye_x+1, eye_y+1, black) ;
            draw_rect_fill(eye_x+5, eye_y, eye_x+6, eye_y+1, black);
        }
    }
}

void draw_score() {
    // Cover lives and bombs getting smaller
    draw_rect_fill(230, 0, VIRTUAL_SCREEN_WIDTH, TILE_HEIGHT, black);
    // SCORE
    al_draw_bitmap(label_score,  25, 0, 0);
    al_draw_textf(font_score, white, 65, 3, ALLEGRO_ALIGN_LEFT, "%d", score);

    // LEVEL
    al_draw_bitmap(label_level, 120, 0, 0);
    al_draw_textf(font_large, HEX_TO_COLOR(0x555555), 188, 3, ALLEGRO_ALIGN_RIGHT, "%02d-", level);
    al_draw_textf(font_large, HEX_TO_COLOR(0xaaaaaa), 187, 2, ALLEGRO_ALIGN_RIGHT, "%02d-", level);
    al_draw_bitmap(candy, 187, 0, 0);

    // LIVES
    al_draw_bitmap(gobman[0], 230, 0, 0);
    al_draw_textf(font_score, white, 250, 2, ALLEGRO_ALIGN_LEFT,"%d", lives);

    // BOMBS
    al_draw_bitmap(object[BOMB], 270, 0, 0);
    al_draw_textf(font_score, white, 290, 2, ALLEGRO_ALIGN_LEFT, "%d", bombs);
}

void draw_score(int points) {
    if (score % EXTRA_LIFE_POINTS > (score + points) % EXTRA_LIFE_POINTS) {
        al_stop_samples();
        al_play_sample(sfx_oneup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        lives++;
    }
    score += points;
    flag_update_score = true;
}

void draw_tiles() {
    signed char *tile=map-1;
    if (ticks_redpill == 0 && !flag_redpill)
    for (int y=0; y<MAP_HEIGHT*TILE_HEIGHT; y+=TILE_HEIGHT)
    for (int x=0; x<MAP_WIDTH*TILE_WIDTH;   x+=TILE_WIDTH) {
        tile++;
        if (*tile >= 18 || *tile <= 0) continue; // ignores monster releaser
        al_draw_bitmap(tileset[*tile], x, y, 0);
    }
    else // redpill effect
    for (int y=0; y<MAP_HEIGHT*TILE_HEIGHT; y+=TILE_HEIGHT)
    for (int x=0; x<MAP_WIDTH*TILE_WIDTH;   x+=TILE_WIDTH) {
        tile++;
        if (*tile > 2 || *tile <= 0) continue;
        al_draw_bitmap(tileset[*tile], x, y, 0);
    }
}

void draw_objects() {
    int i;
    if (ticks_candy > 0) {
        al_draw_bitmap(candy,
            candy_pos.x * TILE_WIDTH,
            candy_pos.y * TILE_HEIGHT,
            0);
    }
    for (i=0; i<OBJECT_COUNT; i++)
        if (ticks_object[i] > 0)
        al_draw_bitmap(object[i],
            object_pos[i].x * TILE_WIDTH,
            object_pos[i].y * TILE_HEIGHT,
            0);
}

void animation_death() {
    al_stop_samples();
    al_play_sample(sfx_death, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

    al_clear_to_color(black);
    draw_tiles();
    draw_gobman();

    int time_limit = TICKS + SECS_TO_BPS(1.5);
    while (TICKS < time_limit) {
        al_set_target_bitmap(map_bitmap);
        al_put_pixel(
            gobman_coord.x + rand()%TILE_WIDTH,
            gobman_coord.y + rand()%TILE_HEIGHT, black);
        flush_buffer();
    }
}

void animation_eat(int set_x, int set_y, int set_points) {
    static int x, y, points;
    if (set_points > 0) {
        x = set_x;
        y = set_y;
        points = set_points;
        return;
    }
    // The animation is set prior to ghosts redraw, thus we have to cover the
    // case of the ghost has moved in one of the four directions
    draw_rect_fill(x-1, y-1, x+TILE_WIDTH+1, y+TILE_HEIGHT+1, black);
    draw_gobman();

    al_draw_textf(font_points, HEX_TO_COLOR(0xffffff),
        x+TILE_WIDTH/2+1, y+TILE_WIDTH/2-4,
        ALLEGRO_ALIGN_CENTER, "%d", points);
    flush_buffer();
    al_rest(0.5);
}

void animation_eat(struct pair pos, int points) {
    animation_eat(pos.x, pos.y, points);
}

void animation_eat() {
    animation_eat(0,0,0);
}

void reset_ghosts();

#define EXPLOSION_FRAMES 4

void animation_bomb() {
    ALLEGRO_BITMAP *tmp = al_clone_bitmap(buffer);
    for (int i=0; i<GHOST_COUNT; i++) {
        if (is_ghost_dead[i]) continue;
        al_stop_samples();
        al_play_sample(sfx_bomb, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

        al_set_target_bitmap(tmp);
        draw_rect_fill(
            MAP_OFFSET_X + ghosts_coord[i].x -1,
            MAP_OFFSET_Y + ghosts_coord[i].y -1 ,
            MAP_OFFSET_X + ghosts_coord[i].x + TILE_WIDTH + 1,
            MAP_OFFSET_Y + ghosts_coord[i].y + TILE_HEIGHT + 1,
            black);
        for (int j=0; j<EXPLOSION_FRAMES*2-1; j++) {
            al_set_target_bitmap(tmp);
            al_draw_bitmap_region(explosion,
                (EXPLOSION_FRAMES-1-abs(j-EXPLOSION_FRAMES+1))*11, 0, 11, 11,
                MAP_OFFSET_X + ghosts_coord[i].x + 1,
                MAP_OFFSET_Y + ghosts_coord[i].y,
                0);
            al_set_target_bitmap(buffer);

            al_draw_bitmap(tmp, 0, 0, 0);
            flush_buffer();
            al_rest(0.6/3/(EXPLOSION_FRAMES*2-1));

            al_draw_bitmap(tmp,-4, 0, 0);
            flush_buffer();
            al_rest(0.6/3/(EXPLOSION_FRAMES*2-1));

            al_draw_bitmap(tmp, 0, 0, 0);
            flush_buffer();
            al_rest(0.6/3/(EXPLOSION_FRAMES*2-1));
        }
        al_set_target_bitmap(tmp);
        draw_rect_fill(
            MAP_OFFSET_X + ghosts_coord[i].x,
            MAP_OFFSET_Y + ghosts_coord[i].y,
            MAP_OFFSET_X + ghosts_coord[i].x + TILE_WIDTH,
            MAP_OFFSET_Y + ghosts_coord[i].y + TILE_HEIGHT, black);
    }
    al_destroy_bitmap(tmp);
}

void animation_redpill() {
    ALLEGRO_BITMAP *on, *off;
    on = al_clone_bitmap(buffer);
    al_set_target_bitmap(map_bitmap);
    signed char *tile = map-1;
    for (int y=0; y<MAP_HEIGHT*TILE_HEIGHT; y+=TILE_HEIGHT)
    for (int x=0; x<MAP_WIDTH*TILE_WIDTH;   x+=TILE_WIDTH) {
        tile++;
        if (*tile <= 2 || *tile >= 18) continue; // draw only walls
        draw_rect_fill(x, y, x+TILE_WIDTH-1, y+TILE_HEIGHT-1, black);
    }
    draw_ghosts();
    off = al_clone_bitmap(buffer);
    al_set_target_bitmap(buffer);
    float times[] = {0.20, 0.17, 0.17, 0.14, 0.15, 0.12, 0.12, 0.10, 0.10, 0.08, 0.07, 0.05, 0.05, 0.03, 0.02};
    for (int i=0; i<15; i++) {
        al_draw_bitmap(i%2 ? on : off, 0, 0, 0);
        flush_buffer();
        al_rest(times[i]);
    }
    al_destroy_bitmap(on);
    al_destroy_bitmap(off);
}

void animation_door() {
    ALLEGRO_BITMAP *flash = al_clone_bitmap(buffer);
    ALLEGRO_BITMAP *dark  = al_clone_bitmap(buffer);
    ALLEGRO_COLOR color;
    unsigned char r, g, b;

    al_lock_bitmap(flash, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    al_set_target_bitmap(flash);
    for (int y=0; y<VIRTUAL_SCREEN_HEIGHT; y++) {
    for (int x=0; x<VIRTUAL_SCREEN_WIDTH;  x++) {
        color = al_get_pixel(flash, x, y);
        al_unmap_rgb(color, &r, &g, &b);
        if (r==0 && g==0 && b==0)
            al_put_pixel(x, y, white);
    }
    }
    al_unlock_bitmap(flash);
    al_set_target_bitmap(buffer);

    #define SHINING_STEPS (TARGET_FRAMERATE/4)
    for (int i=0; i<8; i++) {
        for (int steps=0; steps<=SHINING_STEPS; steps++) {
            al_draw_bitmap(dark, 0, 0, 0);
            al_draw_tinted_bitmap(
                flash,
                al_map_rgba_f(steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS),
                0, 0, 0);
            flush_buffer();
        }
        for (int steps=SHINING_STEPS; steps>=0; steps--)  {
            al_draw_bitmap(dark, 0, 0, 0);
            al_draw_tinted_bitmap(
                flash,
                al_map_rgba_f(steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS,
                              steps/(float)SHINING_STEPS),
                0, 0, 0);
            flush_buffer();
        }
    }
    al_destroy_bitmap(flash);
    al_destroy_bitmap(dark);
}

bool operator==(const struct pair& a, const struct pair& b){
    return a.x == b.x && a.y == b.y;
}

struct pair vacant_random() {
    signed char *tile;
    while (true) {
        tile = map + rand()%(MAP_WIDTH*MAP_HEIGHT);
        if (*tile >=0 && *tile <= 2)
            break;
    }
    struct pair ret = {(int) (tile-map)%MAP_WIDTH, (int) (tile-map)/MAP_WIDTH};
    return ret;
}

void reset_gobman() {
    gobman_pos.x = GOBMAN_START_POS_X;
    gobman_pos.y = GOBMAN_START_POS_Y;
    gobman_coord.x = GOBMAN_START_POS_X * TILE_WIDTH;
    gobman_coord.y = GOBMAN_START_POS_Y * TILE_HEIGHT;
    gobman_dir = next_dir = RIGHT;
    gobman_frame = GOBMAN_FRAME_STEP*(GOBMAN_FRAMES*2-1)/2;
}

void reset_ghosts() {
    for (int i=0; i<GHOST_COUNT; i++) {
        ghosts_pos[i].x = GHOSTS_START_POS_X;
        ghosts_pos[i].y = GHOSTS_START_POS_Y;
        ghosts_coord[i].x = GHOSTS_RESTING_COORD_X;
        ghosts_coord[i].y = GHOSTS_RESTING_COORD_Y + rand()%3 - 1;
        ghosts_dir[i] = i%2 ? UP : DOWN;
        is_ghost_dead[i] = true;
    }
    num_ghosts_alive = 0;
}

void reset_status() {
    ticks_power  = 0;
    ticks_freeze = 0;
    ticks_candy = - RAND_TIME(TIME_CANDY);
    candy_pos = vacant_random();
    for (int i=0; i<OBJECT_COUNT; i++) {
        ticks_object[i] = - RAND_TIME(TIME_OBJECT[i]);
        object_pos[i] = vacant_random();
    }
    ticks_redpill = 0;
    ticks_next_ghost = 0;
    num_ghosts_alive = 0;
    next_ghost_out = 0;
    ghost_multiplier = 1;
    flag_abort = false;
}

bool warn_abort() {
         draw_rect(83, 80, 236, 103, HEX_TO_COLOR(0x8e8e8e));
         draw_rect(84, 81, 235, 102, HEX_TO_COLOR(0xaaaaaa));
         draw_rect(85, 82, 234, 101, HEX_TO_COLOR(0x8e8e8e));
    draw_rect_fill(86, 83, 233, 100, HEX_TO_COLOR(0x000000));

    draw_text_shadow(font_large,
        HEX_TO_COLOR(0xffffff),
        HEX_TO_COLOR(0xaaaaaa),90,88,ALLEGRO_ALIGN_LEFT,
        "ABORT GAME (Y/N)");

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    //al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());

    bool loop_end = false;
    bool return_state = true;
    while(!loop_end) {

        while (al_get_next_event(event_queue, &event)) {
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_Y
                ||  event.keyboard.keycode == ALLEGRO_KEY_BACK
                ||  event.keyboard.keycode == ALLEGRO_KEY_ENTER
                ||  event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER
                ) {
                    return_state = true;
                    loop_end = true;
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_N
                ||  event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                ) {
                    return_state = false;
                    loop_end = true;
                }
            }
            if (event.type == ALLEGRO_EVENT_TOUCH_END) {
                return_state = false;
                loop_end = true;
            }
        }

        flush_buffer();
    }
    al_destroy_event_queue(event_queue);
    return return_state;
}

void* input_monitor(ALLEGRO_THREAD *self, void *args) {
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_joystick_event_source());
    al_register_event_source(event_queue, al_get_touch_input_event_source());
    int touch_x=0, touch_y=0;

    while (!al_get_thread_should_stop(self)) {
        if (!al_wait_for_event_timed(event_queue, &event, 0.1))
            continue;
        if (flag_abort)
            continue;
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            /**/ if (event.keyboard.keycode == key_up
                 ||  event.keyboard.keycode == ALLEGRO_KEY_UP)
                     next_dir = UP;
            else if (event.keyboard.keycode == key_down
                 ||  event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                     next_dir = DOWN;
            else if (event.keyboard.keycode == key_left
                 ||  event.keyboard.keycode == ALLEGRO_KEY_LEFT)
                     next_dir = LEFT;
            else if (event.keyboard.keycode == key_right
                 ||  event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                     next_dir = RIGHT;
            else if (event.keyboard.keycode == key_fire
                 ||  event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                     flag_bomb = true;
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE
                 ||  event.keyboard.keycode == ALLEGRO_KEY_BACK)
                     flag_abort = true;
        }

        else if (joystick_is_installed) {
            if (event.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
                ALLEGRO_JOYSTICK_STATE js;
                al_get_joystick_state(joystick_id, &js);
                float ver = js.stick[joystick_vertical_stick].axis[joystick_vertical_axis];
                float hor = js.stick[joystick_horizontal_stick].axis[joystick_horizontal_axis];
                if (fabs(ver) > fabs(hor)) {
                    if (fabs(ver) < 0.1)
                        continue;
                    if (ver / joystick_vertical_positive > 0)
                        next_dir = UP;
                    else
                        next_dir = DOWN;
                }
                else {
                    if (fabs(hor) < 0.1)
                        continue;
                    if (hor / joystick_horizontal_positive > 0)
                        next_dir = RIGHT;
                    else
                        next_dir = LEFT;
                }
            }
            else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
                if (event.joystick.id != joystick_id)
                    continue;
                flag_bomb = true;
            }
        }
        if (event.type == ALLEGRO_EVENT_TOUCH_BEGIN) {
            if (!event.touch.primary) continue;
            touch_x = event.touch.x;
            touch_y = event.touch.y;
        }
        else if (event.type == ALLEGRO_EVENT_TOUCH_END) {
            if (!event.touch.primary) continue;
            touch_x = event.touch.x - touch_x;
            touch_y = event.touch.y - touch_y;
            if (touch_x == 0 && touch_y == 0) {
                flag_bomb = true;
            }
            else if (abs(touch_x) > abs(touch_y)) {
                if (abs(touch_x) > 0) {
                    if (touch_x > 0)
                        next_dir = RIGHT;
                    else
                        next_dir = LEFT;
                }
            }
            else {
                if (abs(touch_y) > 0) {
                    if (touch_y > 0)
                        next_dir = DOWN;
                    else
                        next_dir = UP;
                }
            }
        }
    }

    al_destroy_event_queue(event_queue);
    return NULL;
}

bool run_level() {
    signed char level_map[MAP_WIDTH*MAP_HEIGHT];

    switch(level%NUM_LEVELS) {
    case 1: LOAD_MAP(LEVEL1_MAP); break;
    case 2: LOAD_MAP(LEVEL2_MAP); break;
    case 3: LOAD_MAP(LEVEL3_MAP); break;
    case 4: LOAD_MAP(LEVEL4_MAP); break;
    case 5: LOAD_MAP(LEVEL5_MAP); break;
    case 6: LOAD_MAP(LEVEL6_MAP); break;
    case 7: LOAD_MAP(LEVEL7_MAP); break;
    case 8: LOAD_MAP(LEVEL8_MAP); break;
    case 9: LOAD_MAP(LEVEL9_MAP); break;
    case 0: LOAD_MAP(LEVEL10_MAP); break;
    }

    map = level_map;

    // 10 is the width of the sprite sheet
    candy = GET_SUB_BITMAP(candies, 10, (NUM_LEVELS+10-1)/10,
        (level-1)%NUM_LEVELS, (level-1)/NUM_LEVELS);
    al_set_target_bitmap(tinted_walls);
    al_draw_tinted_bitmap(walls, HEX_TO_COLOR(LEVEL_COLOR[level-1]), 0, 0, 0);
    al_set_target_bitmap(buffer);

    ALLEGRO_THREAD *thread_inputs, *thread_logics;

    is_running = false;
    flag_game_over = false;
    flag_update_score = true;
    flag_lost_life = false;
    flag_stop_logics = true;
    flag_abort = false;
    flag_redpill = false;
    flag_bomb = false;
    flag_door = false;

    // START THREADS
    thread_inputs = al_create_thread(input_monitor, NULL);
    al_start_thread(thread_inputs);

    thread_logics = al_create_thread(update_logics, NULL);
    al_start_thread(thread_logics);

    // FADE IN INTRO
    reset_gobman();
    reset_ghosts();
    reset_status();
    al_set_target_bitmap(map_bitmap);
    draw_tiles();
    draw_gobman();
    al_set_target_bitmap(buffer);
    draw_score();
    fade_in(DEFAULT_FADE_STEPS);

    LEVEL_START:

    reset_gobman();
    reset_ghosts();
    reset_status();

    // GET READY! : DO NOT DRAW GHOSTS
    al_set_target_bitmap(map_bitmap);
    al_clear_to_color(black);
    draw_tiles();
    draw_gobman();
    al_set_target_bitmap(buffer);
    draw_score();
    al_draw_textf(font_small, white,
        VIRTUAL_SCREEN_WIDTH/2, VIRTUAL_SCREEN_HEIGHT/2-12,
        ALLEGRO_ALIGN_CENTER, "GET");
    al_draw_textf(font_small, white,
        VIRTUAL_SCREEN_WIDTH/2+1, VIRTUAL_SCREEN_HEIGHT/2-6,
        ALLEGRO_ALIGN_CENTER, "READY!");
    flush_buffer();
    al_rest(TIME_GET_READY);

    is_running = true;
    flag_stop_logics = false;
    while (is_running) {
        if (flag_game_over) {
            al_set_target_bitmap(map_bitmap);
            animation_death();
            al_set_target_bitmap(buffer);
            draw_score();
            goto LEVEL_END;
        }
        else if (flag_lost_life) {
            flag_lost_life = false;
            al_set_target_bitmap(map_bitmap);
            animation_death();
            al_set_target_bitmap(buffer);
            goto LEVEL_START;
        }
        else if (flag_abort) {
            flag_stop_logics = true;
            if (warn_abort()) {
                is_running = false;
                flag_game_over = true;
                break;
            }
            else {
                flag_stop_logics = false;
                flag_abort = false;
            }
        }
        else if (flag_redpill) {
            animation_redpill();
            flag_redpill = false;
            al_set_target_bitmap(buffer);
            flag_stop_logics = false;
        }
        else if (flag_bomb && !(flag_bomb = false) && num_ghosts_alive > 0 && bombs > 0) {
            flag_stop_logics = true;
            animation_bomb();
            reset_ghosts();
            bombs--;
            flag_update_score = true;
            ticks_power = 0;
            flag_stop_logics = false;
        }
        else if (flag_stop_logics) {
            al_set_target_bitmap(map_bitmap);
            animation_eat();
            al_set_target_bitmap(buffer);
            flag_stop_logics = false;
        }

        al_set_target_bitmap(map_bitmap);
        al_clear_to_color(black);
        draw_tiles();
        draw_objects();
        draw_gobman();
        draw_ghosts();
        al_set_target_bitmap(buffer);

        if (flag_update_score) {
            flag_update_score = false;
            draw_score();
        }
        if (ticks_freeze > 0) {
            al_draw_textf(font_large, white,
            VIRTUAL_SCREEN_WIDTH/2+10,
            VIRTUAL_SCREEN_HEIGHT/2-10,
            ALLEGRO_ALIGN_RIGHT,
            "%d",
            (int) BPS_TO_SECS(ticks_freeze+SYSTEM_BPS-1));
        }
        #ifdef DEBUG
        al_draw_textf(font_large, white,
            VIRTUAL_SCREEN_WIDTH-10,
            VIRTUAL_SCREEN_HEIGHT-10,
            ALLEGRO_ALIGN_RIGHT,
            "F:%4.1f / G:%4.1f / P:%4.1f",
            BPS_TO_SECS(ticks_freeze),
            BPS_TO_SECS(ticks_next_ghost),
            BPS_TO_SECS(ticks_power));
        #endif
        flush_buffer();
    }

    LEVEL_END:

    if (!flag_game_over) {
        al_set_target_bitmap(map_bitmap);
        al_clear_to_color(black);
        draw_tiles();
        draw_objects();
        draw_gobman();
        draw_ghosts();
        al_set_target_bitmap(buffer);
        draw_score();
        if (ticks_freeze > 0) {
            al_draw_textf(font_large, white,
            VIRTUAL_SCREEN_WIDTH/2+10,
            VIRTUAL_SCREEN_HEIGHT/2-10,
            ALLEGRO_ALIGN_RIGHT,
            "%d",
            (int) BPS_TO_SECS(ticks_freeze+SYSTEM_BPS-1));
        }
    }

    if (flag_door) {
        animation_door();
    }

    al_destroy_bitmap(candy);

    fade_out(DEFAULT_FADE_STEPS);

    al_set_thread_should_stop(thread_inputs);
    al_set_thread_should_stop(thread_logics);

    al_join_thread(thread_inputs, NULL);
    al_join_thread(thread_logics, NULL);

    al_destroy_thread(thread_inputs);
    al_destroy_thread(thread_logics);

    return !flag_game_over;
}

void play() {
    score = 0;
    lives = START_LIVES;
    bombs = 0;
    level = 1;
    time_power = TIME_POWER;
    time_next_ghost = TIME_NEXT_GHOST;
    load();
    while (run_level()) {

        level = level % NUM_LEVELS +1;

        time_power -= 0.5;
        if (time_power < TIME_POWER_MIN)
            time_power = TIME_POWER_MIN;

        time_next_ghost -= 0.5;
        if (time_next_ghost < TIME_NEXT_GHOST_MIN)
            time_next_ghost = TIME_NEXT_GHOST_MIN;
    }
    unload();
}
