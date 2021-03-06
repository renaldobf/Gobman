#ifndef LEVEL_H
#define LEVEL_H

#include "../base.h"

#define MAP_WIDTH  23
#define MAP_HEIGHT 17

#define TILE_WIDTH  13
#define TILE_HEIGHT 11
#define TILE_COUNT (1+1+1+15) // FIRST iS NULL, DOT, POWER AND 15 WALLS

#define MAP_OFFSET_X 10
#define MAP_OFFSET_Y 11

#define GOBMAN_FRAMES 3
#define GOBMAN_FRAME_STEP 4

#define GOBMAN_START_POS_X 11
#define GOBMAN_START_POS_Y 9

#define GHOSTS_START_POS_X 11
#define GHOSTS_START_POS_Y (7-2)

#define GHOSTS_RESTING_COORD_X (11*TILE_WIDTH)
#define GHOSTS_RESTING_COORD_Y (7*TILE_HEIGHT)

#define GHOST_COUNT 4

#define TIME_POWER           15.0
#define TIME_POWER_MIN        5.0
#define TIME_NEXT_GHOST      10.0
#define TIME_NEXT_GHOST_MIN   4.0
#define TIME_BLINK_GHOST      2.0
#define TIME_BLINK_INTERVAL   0.1
#define TIME_GET_READY        2.0
#define TIME_REDPILL         10.0
#define TIME_FREEZE          20.0

#define START_LIVES 3

#define POINTS_DOT      5
#define POINTS_POWER   20
#define POINTS_CANDY  200
#define POINTS_GHOST  100
#define EXTRA_LIFE_POINTS 10000

#define REDPILL   0
#define BOMB      1
#define BOX       2
#define HOURGLASS 3
#define DOOR      4
#define ONEUP     5
#define OBJECT_COUNT 6

#define TIME_CANDY 120
#define TIME_OBJECT ((int[]){90,120,210,300,520,720})
#define OBJECT_TIMEOUT 15

#define TICKS_OBJECT_TIMEOUT SECS_TO_BPS(OBJECT_TIMEOUT)

#define RAND_TIME(x)   ( rand()%(SECS_TO_BPS(2 + x)) ) // 2 seconds after the level begins
#define RAND_TIME_2(x) ( SECS_TO_BPS( (x) + rand()%(x) ) / 2 )

//#define ASSERT_POS(a) {MAP_OFFSET_X + a.x * TILE_WIDTH, MAP_OFFSET_Y + a.y * TILE_HEIGHT}
//#define GET_DIST(a,b) sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) )

struct pair { int x; int y; };

#define direction char
#define    UP 0
#define RIGHT 1
#define  DOWN 2
#define  LEFT 3

// GOBMAN VARS
extern ALLEGRO_BITMAP *gobman_sprite;
extern ALLEGRO_BITMAP *gobman[GOBMAN_FRAMES];
extern ALLEGRO_BITMAP *gobmanv[GOBMAN_FRAMES];
extern struct pair gobman_pos;
extern struct pair gobman_coord;
extern direction   gobman_dir;
extern direction   next_dir;
extern int         gobman_frame;

// GHOSTS VARS
extern ALLEGRO_BITMAP *ghosts_sprite;
extern ALLEGRO_BITMAP *ghosts[GHOST_COUNT+1];
extern struct pair ghosts_pos[GHOST_COUNT];
extern struct pair ghosts_coord[GHOST_COUNT];
extern direction   ghosts_dir[GHOST_COUNT];
extern bool        is_ghost_dead[GHOST_COUNT];
extern int         num_ghosts_alive;
extern int         next_ghost_out;

// GRAPHICS AND SFX
extern ALLEGRO_BITMAP *label_score;
extern ALLEGRO_BITMAP *label_level;

extern ALLEGRO_SAMPLE *sfx_dot,    *sfx_power, *sfx_death, *sfx_ghost,   *sfx_candy,
                      *sfx_object, *sfx_spawn, *sfx_oneup, *sfx_redpill, *sfx_bomb;
extern ALLEGRO_FONT *font_score, *font_points;
extern ALLEGRO_COLOR black, white;

extern ALLEGRO_BITMAP *objects, *object[OBJECT_COUNT], *candies, *candy;
extern ALLEGRO_BITMAP *walls, *tinted_walls, *explosion;
extern ALLEGRO_BITMAP *tileset[TILE_COUNT];
extern ALLEGRO_BITMAP *map_bitmap;
extern signed char *map;

// TIMING VARS
extern float time_power;
extern float time_next_ghost;

// TIMING VARS
extern int ticks_power;
extern int ticks_freeze;
extern int ticks_next_ghost;
extern int ticks_object[OBJECT_COUNT];
extern int ticks_candy;
extern int ticks_redpill;

extern struct pair candy_pos, object_pos[OBJECT_COUNT];

// SCORE VARS
extern int level;
extern int lives;
extern int bombs;
extern int score;
extern int ghost_multiplier;

// LOGIC FLAGS
extern bool is_running;
extern bool flag_game_over;
extern bool flag_update_score;
extern bool flag_lost_life;
extern bool flag_stop_logics;
extern bool flag_abort;
extern bool flag_redpill;
extern bool flag_door;

//bool is_free(int x, int y, direction dir);
//bool is_free(struct pair pos, direction dir);

bool is_free_gobman(struct pair pos, direction dir);
bool is_free_ghosts(struct pair pos, direction dir);

void animation_eat(int set_x, int set_y, int set_points);
void animation_eat(struct pair pos, int points);
void animation_eat();

void draw_score(int points);

void play();

bool operator==(const struct pair& a, const struct pair& b);
struct pair vacant_random();

#define NUM_LEVELS 10

#define LEVEL_COLOR ((int[]){0xa000a0,0xdddd00,0x0000ff,0xdd0000,0x00dd00,\
0x888888,0xa000a0,0xdddd00,0x0000ff,0xdd0000})

#define LOAD_MAP(x) for(int i=0; i<MAP_WIDTH*MAP_HEIGHT; i++) level_map[i] = (x)[i];

#define LEVEL1_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 7, 1, 5, 6, 1, 5, 3, 3, 3, 3,14, 1,16, 3, 3, 3, 3, 6, 1, 5, 6, 1, 8,\
-1, 1, 4, 4, 1,15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 4, 4, 1,-1,\
 6, 1, 4, 4, 1, 1, 1, 5, 3, 3, 3, 3, 3, 3, 3, 6, 1, 1, 1, 4, 4, 1, 5,\
 4, 1, 4, 4, 1,13, 1, 4, 1, 1, 1, 1, 1, 1, 1, 4, 1,13, 1, 4, 4, 1, 4,\
 4, 1, 8, 7, 1, 4, 1,15, 1, 5,14,18,16, 6, 1,15, 1, 4, 1, 8, 7, 1, 4,\
 4, 1, 1, 1, 2, 4, 1, 1, 1, 4,18,18,18, 4, 1, 1, 1, 4, 2, 1, 1, 1, 4,\
 4, 1, 5, 6, 1, 4, 1,13, 1, 8, 3, 3, 3, 7, 1,13, 1, 4, 1, 5, 6, 1, 4,\
 4, 1, 4, 4, 1,15, 1, 4, 1, 1, 1, 0, 1, 1, 1, 4, 1,15, 1, 4, 4, 1, 4,\
 4, 1, 4, 4, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 3, 7, 1, 1, 1, 4, 4, 1, 4,\
 4, 1, 4, 4, 1,13, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 1, 4, 4, 1, 4,\
 7, 1, 4, 4, 1, 8, 3, 3, 3, 3,14, 1,16, 3, 3, 3, 3, 7, 1, 4, 4, 1, 8,\
-1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 1,-1,\
 6, 1, 8,11, 3, 3,14, 1,16, 3, 3, 3, 3, 3,14, 1,16, 3, 3,11, 7, 1, 5,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL2_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 4, 1, 5, 3, 3, 3, 3, 3, 3,14, 1, 1, 1,16, 3, 3, 3, 3, 3, 3, 6, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1,13, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4,\
 4, 1, 4, 1,13, 1, 1, 1,16, 3, 3,11, 3, 3,14, 1, 1, 1,13, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1,13, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 1, 4, 1, 4, 1, 4,\
 4, 1,15, 1, 4, 1, 4, 1, 1, 5,14,18,16, 6, 1, 1, 4, 1, 4, 1,15, 1, 4,\
 4, 1, 1, 1, 4, 1, 4, 1, 1, 4,18,18,18, 4, 1, 1, 4, 1, 4, 1, 1, 1, 4,\
 4, 1,13, 1, 4, 1, 4, 1, 1, 8, 3, 3, 3, 7, 1, 1, 4, 1, 4, 1,13, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 0, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1,15, 1,16, 3, 3, 9, 3, 3,14, 1,15, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1,15, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1,15, 1, 4, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4,\
 4, 1, 8, 3, 3, 3, 3, 3, 3,14, 1, 1, 1,16, 3, 3, 3, 3, 3, 3, 7, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 2, 1, 1,13, 1, 1,13, 1, 1, 1, 4, 1, 1, 1,13, 1, 1,13, 1, 1, 2, 4,\
 8, 3, 3, 3,11, 3, 3,11, 3, 3, 3,11, 3, 3, 3,11, 3, 3,11, 3, 3, 3, 7}

#define LEVEL3_MAP (int[]){\
 5, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 6,\
 4, 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 2, 4,\
 4, 1,15, 1, 5, 3, 3, 3, 3,14, 1, 1,16, 3, 3, 3, 3, 3, 6, 1,15, 1, 4,\
 4, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 4,\
 4, 1, 1, 1, 4, 1, 5,14, 1, 1,16, 3, 3, 3, 3, 3, 6, 1, 4, 1, 1, 1, 4,\
 4, 1,13, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 4, 1,13, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 5,14,18,16, 6, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 4,18,18,18, 4, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 8, 3, 3, 3, 7, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 2, 1, 1, 1, 0, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 8, 3, 3, 3, 3, 3,14, 1, 1,16, 7, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 8, 3, 3, 3, 3,14, 1, 1,16, 3, 3, 3, 3, 3, 7, 1, 4, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4,\
 4, 1, 8,14, 1, 1,16, 3, 3, 3, 3, 3, 3, 3, 3, 3,14, 1, 1,16, 7, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL4_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 4, 1,16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,14, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 1, 1,13, 1,13, 1, 1,16, 3, 3, 3,14, 1, 1,13, 1,13, 1, 1, 1, 4,\
 4, 1,13, 1, 4, 1,15, 1, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 4, 1,13, 1, 4,\
 7, 1, 4, 1, 4, 1, 1, 1, 1, 5,14,18,16, 6, 1, 1, 1, 1, 4, 1, 4, 1, 8,\
-1, 1, 4, 1,12, 3, 3,14, 1, 4,18,18,18, 4, 1,16, 3, 3,10, 1, 4, 1,-1,\
 6, 1, 4, 1, 4, 1, 1, 1, 1, 8, 3, 3, 3, 7, 1, 1, 1, 1, 4, 1, 4, 1, 5,\
 4, 1,15, 1,15, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,15, 1,15, 1, 4,\
 4, 1, 1, 1, 1, 1, 1,16, 3, 3, 6, 1, 5, 3, 3,14, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 5, 3, 6, 1, 1, 1, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 5, 3, 6, 1, 4,\
 4, 1, 4,18, 4, 1,16, 3,14, 1, 4, 1, 4, 1,16, 3,14, 1, 4,18, 4, 1, 4,\
 4, 1, 4,18, 4, 1, 1, 1, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 4,18, 4, 1, 4,\
 4, 1, 8, 3, 7, 1, 1,16, 3, 3, 7, 1, 8, 3, 3,14, 1, 1, 8, 3, 7, 1, 4,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL5_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 4, 1, 5, 3,14, 1,16, 3, 3,14, 1, 4, 1,16, 3, 3, 3, 6, 1, 5, 6, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1,15, 1, 8, 7, 1, 4,\
 4, 1, 4, 1, 5, 6, 1, 5,14, 1,16,11,14, 1,16, 6, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 1, 4, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 4, 1,16, 9, 3,14, 1, 4,\
 7, 1,15, 1, 8, 7, 1, 4, 1, 5,14,18,16, 6, 1, 4, 1, 1, 4, 1, 1, 1, 4,\
-1, 1, 1, 1, 1, 1, 2, 4, 1, 4,18,18,18, 4, 1,12,14, 1, 4, 1,16, 3,10,\
 9, 3,14, 1,16, 3, 3,10, 1, 8, 3, 3, 3, 7, 1, 4, 1, 1, 4, 1, 1, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 0, 1, 1, 1, 4, 1,16, 7, 1,13, 1, 8,\
 4, 1,13, 1, 5, 6, 1, 8, 3, 3,14, 1,16, 3, 3, 7, 1, 1, 1, 1, 4, 1,-1,\
 4, 1, 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 1, 4, 1, 5,\
 4, 1, 4, 1, 8, 7, 1, 5, 3, 6, 1, 5,14, 1,16,14, 1, 8, 7, 1,15, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 4,18, 4, 1,15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 8, 3, 3,14, 1, 8, 3, 7, 1, 1, 1,16, 3, 3, 3,14, 1,16,14, 1, 4,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,13, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL6_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 7, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
-1, 1,16, 3,14, 1,13, 1,13, 1, 5, 6, 1,13, 1,16, 3,14, 1, 5, 6, 1, 4,\
 6, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4, 1, 4, 1, 1, 1, 1, 1, 4, 4, 1, 4,\
 4, 1, 5, 3,14, 1, 4, 1,15, 1, 8, 7, 1, 8, 3, 6, 1,13, 1, 4, 4, 1, 4,\
 4, 1, 4, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4, 1, 4,\
 4, 1, 4, 1,16, 3,17,14, 1, 5,14,18,16, 6, 1, 4, 1, 4, 1, 4, 4, 1, 4,\
 4, 1, 4, 1, 1, 1, 4, 1, 1, 4,18,18,18, 4, 1,15, 1, 4, 1, 8, 7, 1, 4,\
 4, 1, 8, 3,14, 1, 4, 1,16,11, 3, 3, 3, 7, 1, 1, 1, 4, 1, 1, 1, 1, 4,\
 4, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 0, 1, 1, 1,13, 1, 8, 3, 3, 6, 1, 4,\
 4, 1,16, 3, 3, 3, 7, 1, 5, 3, 3, 3, 3, 3, 3, 7, 1, 1, 1, 1, 4, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1,13, 1, 1, 4, 1, 4,\
 4, 1,13, 1,16, 3, 3, 3, 7, 1,16, 3,14, 1,16,14, 1, 8,14, 1,15, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8,\
 4, 1, 8, 3, 3, 3, 3, 3, 3, 3, 3, 3,14, 1,16, 3, 3,14, 1,16,14, 1,-1,\
 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 5,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL7_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 5, 6, 1,13, 1, 5,14, 1,16, 3,14, 1,16, 3, 3, 3, 3, 3, 6, 1, 4,\
 4, 1, 8, 7, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 4,\
 4, 2, 1, 1, 1, 4, 1, 4, 1,16,14, 1,16,14, 1, 5, 6, 1,13, 1, 4, 1, 4,\
 4, 1, 5, 6, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 4, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 8, 7, 1,15, 1, 4, 1, 5,14,18,16, 6, 1, 4, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 1, 1, 1, 1, 1, 4, 1, 4,18,18,18, 4, 1, 4, 4, 1, 4, 1, 4, 1, 4,\
 7, 1,16, 3, 3, 3, 3, 7, 1, 8, 3, 3, 3, 7, 1, 8, 7, 1,15, 1, 4, 1, 8,\
-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1,-1,\
 9, 3, 3,14, 1,16, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 1, 4, 1, 5,\
 4, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 4, 1, 4,\
 4, 1, 5, 3, 6, 1,13, 1,15, 1, 5, 3, 3, 6, 1,13, 1, 1, 1, 1, 4, 1, 4,\
 4, 1, 4,18, 4, 1, 4, 1, 1, 1, 4,18,18, 4, 1,12, 3, 3,14, 1, 4, 1, 4,\
 4, 1, 8, 3, 7, 1,15, 1,13, 1, 8, 3, 3, 7, 1,15, 1, 1, 1, 1,15, 1, 4,\
 4, 2, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4,\
 8, 3, 3, 3, 3, 3, 3, 3,11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}

#define LEVEL8_MAP (int[]){\
16, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
-1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
13, 1,13, 1, 5, 6, 1, 1, 4, 1,13, 1, 5, 9, 3,14, 1, 5, 3, 3, 6, 1, 4,\
 4, 1, 4, 1, 4, 4, 1, 1, 4, 2, 4, 1, 4, 4, 1, 1, 1, 4, 1, 2, 4, 1, 4,\
 4, 1,15, 1, 4, 4, 1, 5,11, 3, 7, 1, 8, 7, 1, 5, 3, 7, 1,16,10, 1, 4,\
 4, 1, 1, 1, 4, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 4, 1, 4,\
 4, 1,13, 1, 4, 4, 1,15, 1, 5,14,18,16, 6, 1,15, 1, 5, 6, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 4, 1, 1, 1, 4,18,18,18, 4, 1, 1, 1, 4, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 8, 7, 1,13, 1, 8, 3, 3, 3, 7, 1,13, 1, 4, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 1, 1, 1, 4, 1, 1, 1, 0, 1, 1, 1, 4, 1, 4, 4, 1, 4, 1, 4,\
 4, 1,12,14, 1, 5, 3, 7, 1, 5, 6, 1, 5, 3, 3, 7, 1, 4, 4, 1,15, 1, 4,\
 4, 1, 4, 1, 1,15, 1, 1, 1, 4, 4, 1, 4, 1, 1, 1, 1, 4, 4, 1, 1, 1, 4,\
 4, 1,12,14, 1, 1, 1, 5, 3, 7, 4, 1,15, 1, 5, 3, 3, 7, 4, 1,13, 1, 4,\
 4, 1, 4, 2, 1,13, 1, 4,18,18, 4, 1, 1, 1, 4,18,18,18, 4, 1, 4, 1, 4,\
 4, 1, 8, 3, 3, 7, 1, 8, 3, 3, 7, 1,13, 1, 8, 3, 3, 3, 7, 1,15, 1,15,\
 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11, 3, 3, 3, 3, 3, 3, 3, 3, 3,14}

#define LEVEL9_MAP (int[]){\
 5, 3, 9, 3, 9, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 9, 3, 6,\
 4, 2, 4, 1,15, 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,15, 1, 4, 2, 4,\
 4, 1, 4, 1, 1, 1,12, 3,14, 1,16, 3, 3, 3, 3, 3, 6, 1, 1, 1, 4, 1, 4,\
 4, 1, 4, 1,13, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1,13, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1,12, 3, 3, 3, 3, 3,14, 1,16, 3,10, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1,15, 1, 4, 1,15, 1, 1, 5,14,18,16, 6, 1, 1,15, 1, 4, 1,15, 1, 4,\
 4, 1, 1, 1, 4, 1, 1, 1, 1, 4,18,18,18, 4, 1, 1, 1, 1, 4, 1, 1, 1, 4,\
 4, 1,13, 1, 4, 1,13, 1, 1, 8, 3, 3, 3, 7, 1, 1,13, 1, 4, 1,13, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 0, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1,12,14, 1,16, 3, 3, 3, 3, 3, 3,10, 1, 4, 1, 4, 1, 4,\
 4, 1, 4, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 1, 4,\
 4, 1,15, 1, 4, 1,12, 3, 3, 3, 3, 3,14, 1,16, 3,10, 1, 4, 1,15, 1, 4,\
 4, 1, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 1, 1, 4,\
 4, 1,13, 1, 4, 1, 8, 3,14, 1,16, 3, 3, 3, 3, 3,10, 1, 4, 1,13, 1, 4,\
 4, 2, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 2, 4, 1, 4, 2, 4,\
 8, 3,11, 3,11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11, 3,11, 3,11, 3, 7}

#define LEVEL10_MAP (int[]){\
 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6,\
 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 4,\
 4, 1, 5, 3, 3, 3, 3, 6, 1, 5, 3, 6, 1,13, 1, 5, 6, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 5, 3, 3, 3, 7, 1,15, 1, 8, 3, 7, 1, 4, 4, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 8, 3, 3, 6, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 1, 1, 1, 5,14,18,16, 6, 1, 4, 5, 3, 6, 4, 1, 1, 4,\
 4, 1, 4, 4, 1, 5, 3, 6, 1, 4,18,18,18, 4, 1, 4, 8, 3, 7, 4, 1, 1, 4,\
 4, 1, 4, 4, 1, 8, 6, 4, 1, 8, 3, 3, 3, 7, 1, 8, 3, 3, 3, 7, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 4, 4, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,\
 4, 1, 4, 4, 1, 1, 4, 4, 5, 3, 3, 3, 3, 6, 5, 3, 3, 6, 5, 6, 5, 6, 4,\
 4, 1, 4, 4, 1, 1, 4, 4, 4, 5, 6, 5, 6, 4, 4,16,14, 4, 4,12, 4, 4, 4,\
 4, 1, 4, 8, 3, 3, 7, 4, 4, 4, 8, 7, 4, 4, 4, 5, 6, 4, 4, 4,10, 4, 4,\
 4, 1, 8, 3, 3, 3, 3, 7, 8, 7, 1, 1, 8, 7, 8, 7, 8, 7, 8, 7, 8, 7, 4,\
 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 4,\
 8, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7}


#endif // LEVEL_H
