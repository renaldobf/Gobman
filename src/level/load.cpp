#include "load.h"
#include "level.h"

#include "../base.h"
#include "../resources.h"

void load() {
    // Create map sub-bitmap
    map_bitmap = al_create_sub_bitmap(
        al_get_target_bitmap(),
        MAP_OFFSET_X * screen_width / VIRTUAL_SCREEN_WIDTH,
        MAP_OFFSET_Y * screen_height / VIRTUAL_SCREEN_HEIGHT,
        TILE_WIDTH*MAP_WIDTH * screen_width / VIRTUAL_SCREEN_WIDTH,
        TILE_HEIGHT*MAP_HEIGHT * screen_height / VIRTUAL_SCREEN_HEIGHT
        );
    al_set_target_bitmap(map_bitmap);
    al_use_transform(&transform);
    al_set_target_backbuffer(display);
    // Load GobMan sprites
    gobman_sprite = al_load_bitmap(RES_BMP_GOBMAN);
    for (int i=0; i<GOBMAN_FRAMES; i++) {
        gobman[i]  = GET_SUB_BITMAP(gobman_sprite, GOBMAN_FRAMES, 2, i, 0);
        gobmanv[i] = GET_SUB_BITMAP(gobman_sprite, GOBMAN_FRAMES, 2, i, 1);
    }

    // Load ghosts sprites
    ghosts_sprite = al_load_bitmap(RES_BMP_GHOSTS);
    for (int i=0; i<=GHOST_COUNT; i++)
        ghosts[i]  = GET_SUB_BITMAP(ghosts_sprite, GHOST_COUNT+1, 1, i, 0);

    // Load walls, duplicate sprite and subdivide them
    walls = al_load_bitmap(RES_BMP_WALLS);
    tinted_walls = al_clone_bitmap(walls);
    {
        int x, y, i = 3;
        for (y=0; y<3; y++)
        for (x=0; x<5; x++)
            tileset[i++] = GET_SUB_BITMAP(tinted_walls,5,3,x,y);
    }

    explosion = al_load_bitmap(RES_BMP_EXPLOSION);

    // Load object sprites
    objects = al_load_bitmap(RES_BMP_OBJECTS);
    tileset[0] = NULL;
    tileset[1] = GET_SUB_BITMAP(objects, 8, 1, 0, 0);
    tileset[2] = GET_SUB_BITMAP(objects, 8, 1, 1, 0);
    for (int i=0; i<OBJECT_COUNT; i++)
        object[i] = GET_SUB_BITMAP(objects, OBJECT_COUNT+2, 1, i+2, 0);

    // Load candies sprites
    candies = al_load_bitmap(RES_BMP_CANDIES);

    // Load score labels
    label_score = al_load_bitmap("res/score.png"); // to text
    label_level = al_load_bitmap("res/level.png"); // to text

    // Load sound effects
    sfx_dot     = al_load_sample(RES_SFX_DOT);
    sfx_power   = al_load_sample(RES_SFX_POWER);
    sfx_death   = al_load_sample(RES_SFX_DEATH);
    sfx_ghost   = al_load_sample(RES_SFX_GHOST);
    sfx_candy   = al_load_sample(RES_SFX_CANDY);
    sfx_spawn   = al_load_sample(RES_SFX_SPAWN);
    sfx_object  = al_load_sample(RES_SFX_OBJECT);
    sfx_oneup   = al_load_sample(RES_SFX_ONEUP);
    sfx_redpill = al_load_sample(RES_SFX_REDPILL);
    sfx_bomb    = al_load_sample(RES_SFX_BOMB);

    // Load bitmap fonts
    {
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_SCORE);
        int ranges[] = {'0', '9'}, num_ranges=1;
        font_score = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }

    {
        ALLEGRO_BITMAP *bmp_font = al_load_bitmap(RES_FONT_POINTS);
        int ranges[] = {'0', '9'}, num_ranges=1;
        font_points = al_grab_font_from_bitmap(bmp_font, num_ranges, ranges);
        al_destroy_bitmap(bmp_font);
    }

    black = HEX_TO_COLOR(0x000000);
    white = HEX_TO_COLOR(0xffffff);
}

// Unload previously loaded graphics and sounds
void unload() {
    for (int i=0; i<GOBMAN_FRAMES; i++) {
        al_destroy_bitmap(gobman[i]);
        al_destroy_bitmap(gobmanv[i]);
    }
    al_destroy_bitmap(gobman_sprite);

    for (int i=0; i<GHOST_COUNT+1; i++)
        al_destroy_bitmap(ghosts[i]);
    al_destroy_bitmap(ghosts_sprite);

    for (int i=1; i<(1+2+15); i++)
        al_destroy_bitmap(tileset[i]);

    for (int i=0; i<OBJECT_COUNT; i++)
        al_destroy_bitmap(object[i]);

    al_destroy_bitmap(objects);
    al_destroy_bitmap(candies);
    al_destroy_bitmap(walls);
    al_destroy_bitmap(tinted_walls);
    al_destroy_bitmap(explosion);

    al_destroy_bitmap(label_score);
    al_destroy_bitmap(label_level);

    al_destroy_bitmap(map_bitmap);

    al_destroy_sample(sfx_dot);
    al_destroy_sample(sfx_power);
    al_destroy_sample(sfx_death);
    al_destroy_sample(sfx_ghost);
    al_destroy_sample(sfx_candy);
    al_destroy_sample(sfx_spawn);
    al_destroy_sample(sfx_object);
    al_destroy_sample(sfx_oneup);
    al_destroy_sample(sfx_redpill);
    al_destroy_sample(sfx_bomb);

    al_destroy_font(font_score);
    al_destroy_font(font_points);
}
