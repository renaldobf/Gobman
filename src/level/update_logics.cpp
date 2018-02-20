#include "level.h"
#include "update_logics.h"

void* update_logics(ALLEGRO_THREAD *self, void *args) {
    ALLEGRO_EVENT_QUEUE *event_update;
    int loop_count = 0;
    int ticks, old_ticks, diff_ticks;
    int dit_count = 0;
    signed char *tile;
    bool is_moving = false;

    for (int i=0; i<MAP_WIDTH*MAP_HEIGHT; i++)
        if (map[i] == 1) dit_count++;

    event_update = al_create_event_queue();
    al_register_event_source(event_update, al_get_timer_event_source(timer));

    old_ticks=TICKS;
    while (!al_get_thread_should_stop(self)) {

        al_wait_for_event(event_update, NULL);
        al_flush_event_queue(event_update);

        if (!is_running || flag_stop_logics) {
            old_ticks=TICKS;
            continue;
        }

        ticks = TICKS;
        diff_ticks = ticks - old_ticks;
        old_ticks = ticks;

        ticks_power -= diff_ticks;
        if (ticks_power <= 0)
            ticks_power = 0;

        ticks_redpill -= diff_ticks;
        if (ticks_redpill <= 0)
            ticks_redpill = 0;

        ticks_freeze -= diff_ticks;
        if (ticks_freeze <= 0)
            ticks_freeze = 0;

        if (ticks_freeze == 0 && ticks_power == 0) {
            ticks_next_ghost -= diff_ticks;
        }

        if (ticks_next_ghost <= 0)
            ticks_next_ghost = 0;

        ticks_candy ++;
        if (ticks_candy == 0) {
            al_stop_samples();
            al_play_sample(sfx_spawn, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
        else if (ticks_candy > TICKS_OBJECT_TIMEOUT) {
            ticks_candy = - RAND_TIME_2(TIME_CANDY);
            candy_pos = vacant_random();
        }

        for (int i=0; i<OBJECT_COUNT; i++) {
            ticks_object[i]++; //= diff_ticks;
            if (ticks_object[i] == 0) {
                al_stop_samples();
                al_play_sample(sfx_spawn, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
            else if (ticks_object[i] > TICKS_OBJECT_TIMEOUT) {
                ticks_object[i] = - RAND_TIME_2(TIME_OBJECT[i]);
                object_pos[i] = vacant_random();
            }
        }

        if (num_ghosts_alive >= GHOST_COUNT)
            ticks_next_ghost = SECS_TO_BPS(time_next_ghost);
        else if (ticks_next_ghost == 0) {
            // RELEASES GHOST
            ticks_next_ghost = SECS_TO_BPS(time_next_ghost);
            while (!is_ghost_dead[next_ghost_out])
                next_ghost_out = (next_ghost_out+1)%GHOST_COUNT;
            ghosts_coord[next_ghost_out].x = ghosts_pos[next_ghost_out].x * TILE_WIDTH;
            ghosts_coord[next_ghost_out].y = ghosts_pos[next_ghost_out].y * TILE_HEIGHT;
            ghosts_dir[next_ghost_out] = rand()&4 ? LEFT : RIGHT;
            is_ghost_dead[next_ghost_out] = false;
            num_ghosts_alive++;
            next_ghost_out = (next_ghost_out+1)%GHOST_COUNT;
        }

        // GOBMAN MOVEMENT
        if (((gobman_dir == LEFT || gobman_dir == RIGHT) && (gobman_coord.x)%TILE_WIDTH  == 0)
        ||  ((gobman_dir == UP   || gobman_dir == DOWN ) && (gobman_coord.y)%TILE_HEIGHT == 0)
        ) {
            is_moving = false;
            if (is_free_gobman(gobman_pos, next_dir) && gobman_pos.x >= 0 && gobman_pos.x < MAP_WIDTH )
                gobman_dir = next_dir;
        }

        if ((gobman_dir == LEFT && next_dir == RIGHT)
        ||  (gobman_dir == RIGHT && next_dir == LEFT)
        ||  (gobman_dir == UP && next_dir == DOWN)
        ||  (gobman_dir == DOWN && next_dir == UP))
            gobman_dir = next_dir;

        if (is_free_gobman(gobman_pos, gobman_dir))
            is_moving = true;

        if (is_moving) {
            switch (gobman_dir) {
                case    UP: gobman_coord.y--; break;
                case  DOWN: gobman_coord.y++; break;
                case  LEFT: gobman_coord.x--; break;
                case RIGHT: gobman_coord.x++; break;
            }
            gobman_pos.x = (gobman_coord.x + TILE_WIDTH/2) / TILE_WIDTH;
            // Gobman exits screen to the left
            if (gobman_pos.x < 0) {
                int y, signal = gobman_pos.y < MAP_HEIGHT/2 ? 1 : -1;
                for (
                    y=gobman_pos.y+signal;
                    y!=gobman_pos.y;
                    y=(MAP_HEIGHT+y+signal)%MAP_HEIGHT
                )
                    if (map[y * MAP_WIDTH + (MAP_WIDTH-1)] == -1)
                        break;
                gobman_pos.y = y;
                gobman_pos.x = MAP_WIDTH-1;
                gobman_coord.x = gobman_pos.x * TILE_WIDTH + TILE_WIDTH/2;
                gobman_coord.y = gobman_pos.y * TILE_HEIGHT;
            }
            // Gobman exits screen to the right
            else if (gobman_pos.x > MAP_WIDTH) {
                int y, signal = gobman_pos.y < MAP_HEIGHT/2 ? 1 : -1;
                for (
                    y=gobman_pos.y+signal;
                    y!=gobman_pos.y;
                    y=(MAP_HEIGHT+y+signal)%MAP_HEIGHT
                )
                    if (map[y * MAP_WIDTH + (0)] == -1)
                        break;
                gobman_pos.y = y;
                gobman_pos.x = 0;
                gobman_coord.x = gobman_pos.x * TILE_WIDTH - TILE_WIDTH/2;
                gobman_coord.y = gobman_pos.y * TILE_HEIGHT;
            }
            gobman_pos.y = (gobman_coord.y + TILE_HEIGHT/2) / TILE_HEIGHT;

            // SCORE UPDATES
            tile = map+(gobman_pos.y*MAP_WIDTH + gobman_pos.x);
            switch (*tile) {
                case 1:
                    draw_score(POINTS_DOT);
                    *tile = 0;
                    al_play_sample(sfx_dot, 1.0, 0.0, 1.0,
                        ALLEGRO_PLAYMODE_ONCE, NULL);
                    dit_count--;
                    if (dit_count == 0) is_running = false;
                    break;
                case 2:
                    draw_score(POINTS_POWER);
                    *tile = 0;
                    al_play_sample(sfx_power, 1.0, 0.0, 1.0,
                        ALLEGRO_PLAYMODE_ONCE, NULL);
                    if (num_ghosts_alive > 0)
                        ticks_power = SECS_TO_BPS(time_power);
                    ghost_multiplier = 1;
                    break;
            }
            if (ticks_candy > 0 && gobman_pos == candy_pos) {
                flag_stop_logics = true;
                al_stop_samples();
                al_play_sample(sfx_candy, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                draw_score(POINTS_CANDY);
                animation_eat(
                    candy_pos.x * TILE_WIDTH,
                    candy_pos.y * TILE_HEIGHT,
                    POINTS_CANDY);
                ticks_candy = -RAND_TIME_2(TIME_CANDY);
                candy_pos = vacant_random();
            }

            for (int i=0; i<OBJECT_COUNT; i++) {
                if (ticks_object[i] > 0 && gobman_pos == object_pos[i]) {
                    ticks_object[i] = -RAND_TIME_2(TIME_OBJECT[i]);
                    al_stop_samples();
                    switch (i) {
                        case REDPILL:
                            al_play_sample(sfx_redpill, 1.0, 0.0, 1.0,
                                ALLEGRO_PLAYMODE_ONCE, NULL);
                            break;
                        case ONEUP:
                            al_play_sample(sfx_oneup, 1.0, 0.0, 1.0,
                                ALLEGRO_PLAYMODE_ONCE, NULL);
                            break;
                        default:
                            al_play_sample(sfx_object, 1.0, 0.0, 1.0,
                                ALLEGRO_PLAYMODE_ONCE, NULL);
                            break;
                    }
                    switch (i) {
                        case REDPILL:
                            flag_stop_logics = true;
                            flag_redpill = true;
                            ticks_redpill = SECS_TO_BPS(TIME_REDPILL);
                            ticks_object[i] -= ticks_redpill;
                            break;
                        case  BOMB: bombs++;  break;
                        case   BOX: bombs+=5; break;
                        case ONEUP: lives++;  break;
                        case HOURGLASS:
                            ticks_freeze = SECS_TO_BPS(TIME_FREEZE);
                            ticks_object[i] -= ticks_freeze;
                            break;
                        case DOOR:
                            is_running = false;
                            flag_door = true;
                            break;
                    }
                    flag_update_score = true;
                    object_pos[i] = vacant_random();
                }
            }

            gobman_frame = (gobman_frame+1)%(GOBMAN_FRAME_STEP*(GOBMAN_FRAMES*2-1));
        }
        else {
            gobman_frame = GOBMAN_FRAME_STEP*(GOBMAN_FRAMES*2-2)/2;
        }

        // GHOSTS MOVEMENT
        for (int i=0; i<GHOST_COUNT; i++) {
            if (is_ghost_dead[i]) {
                if (loop_count % 3 == 0) {
                    ghosts_coord[i].y += ghosts_dir[i]==UP? -1 : 1;
                    if (ghosts_coord[i].y < GHOSTS_RESTING_COORD_Y - 2)
                        ghosts_dir[i] = DOWN;
                    else if (ghosts_coord[i].y > GHOSTS_RESTING_COORD_Y + 2)
                        ghosts_dir[i] = UP;
                }
                ghosts_coord[i].x = GHOSTS_RESTING_COORD_X - TILE_WIDTH +  TILE_WIDTH*2*i/(GHOST_COUNT-1);
                continue;
            }
            if (ticks_freeze == 0) {
                if (((ghosts_dir[i] == LEFT || ghosts_dir[i] == RIGHT) && (ghosts_coord[i].x)%TILE_WIDTH==0)
                ||  ((ghosts_dir[i] == UP   || ghosts_dir[i] == DOWN)  && (ghosts_coord[i].y)%TILE_HEIGHT==0)
                ) {
                    if (rand()%1000 < 200 && is_free_ghosts(ghosts_pos[i], ghosts_dir[i])) {
                             if (is_free_ghosts(ghosts_pos[i], (ghosts_dir[i]+1)%4)) ghosts_dir[i] = (ghosts_dir[i]+1)%4;
                        else if (is_free_ghosts(ghosts_pos[i], (ghosts_dir[i]+3)%4)) ghosts_dir[i] = (ghosts_dir[i]+3)%4;
                    }
                    else if (ghosts_dir[i] == UP || ghosts_dir[i] == DOWN) {
                        if (ghosts_pos[i].x < gobman_pos.x && is_free_ghosts(ghosts_pos[i], RIGHT))
                            ghosts_dir[i] = RIGHT;
                        else if (ghosts_pos[i].x > gobman_pos.x && is_free_ghosts(ghosts_pos[i], LEFT))
                            ghosts_dir[i] = LEFT;
                        else if (!is_free_ghosts(ghosts_pos[i], ghosts_dir[i])) {
                                 if (is_free_ghosts(ghosts_pos[i], RIGHT)) ghosts_dir[i] = RIGHT;
                            else if (is_free_ghosts(ghosts_pos[i], LEFT))  ghosts_dir[i] = LEFT;
                            else ghosts_dir[i] = (ghosts_dir[i]+2)%4;
                        }
                    }
                    else// if (ghosts_dir[i] == LEFT || ghosts_dir[i] == RIGHT)
                    {
                        if (ghosts_pos[i].y < gobman_pos.y && is_free_ghosts(ghosts_pos[i], DOWN))
                            ghosts_dir[i] = DOWN;
                        else if (ghosts_pos[i].y > gobman_pos.y && is_free_ghosts(ghosts_pos[i], UP))
                            ghosts_dir[i] = UP;
                        else if (!is_free_ghosts(ghosts_pos[i], ghosts_dir[i])) {
                                 if (is_free_ghosts(ghosts_pos[i], UP))   ghosts_dir[i] = UP;
                            else if (is_free_ghosts(ghosts_pos[i], DOWN)) ghosts_dir[i] = DOWN;
                            else ghosts_dir[i] = (ghosts_dir[i]+2)%4;
                        }
                    }

                }
                if (ticks_power == 0 || loop_count & 0b1)
                switch (ghosts_dir[i]) {
                    case    UP: ghosts_coord[i].y--; break;
                    case  DOWN: ghosts_coord[i].y++; break;
                    case  LEFT: ghosts_coord[i].x--; break;
                    case RIGHT: ghosts_coord[i].x++; break;
                }
                ghosts_pos[i].x = (ghosts_coord[i].x + TILE_WIDTH/2) / TILE_WIDTH;
                ghosts_pos[i].y = (ghosts_coord[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;
            }
            
            // TEST FOR GHOST COLLISIONS
            if (abs(gobman_coord.x - ghosts_coord[i].x) < TILE_WIDTH &&
                abs(gobman_coord.y - ghosts_coord[i].y) < TILE_HEIGHT) {

                // TOUCHES A LIVE GHOST
                if (ticks_power == 0) {
                    lives--;
                    if (lives == 0) {
                        flag_stop_logics = true;
                        flag_game_over = true;
                    }
                    else {
                        flag_stop_logics = true;
                        flag_lost_life = true;
                    }
                }

                // TOUCHES A WEAK GHOST
                else {
                    flag_stop_logics = true;
                    al_stop_samples();
                    al_play_sample(sfx_ghost, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    animation_eat(ghosts_coord[i], POINTS_GHOST * ghost_multiplier);
                    draw_score(POINTS_GHOST * ghost_multiplier);
                    ghost_multiplier *= 2;
                    is_ghost_dead[i] = true;
                    ghosts_pos[i].x = GHOSTS_START_POS_X;
                    ghosts_pos[i].y = GHOSTS_START_POS_Y;
                    ghosts_coord[i].x = GHOSTS_RESTING_COORD_X;
                    ghosts_coord[i].y = GHOSTS_RESTING_COORD_Y;
                    num_ghosts_alive--;
                    if (num_ghosts_alive <= 0) ticks_power = 0;
                }
            }
        }

        loop_count++;
    }
    al_destroy_event_queue(event_update);
    return NULL;
}
