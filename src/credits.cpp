#include "credits.h"
#include "base.h"

#include <cstdio>

void credits() {
    // Breaking effect on quit
    ALLEGRO_BITMAP *tmp = clone_backbuffer();
    al_draw_bitmap(tmp, 0, VIRTUAL_SCREEN_HEIGHT/2, 0);
    flush_buffer();
    al_draw_bitmap(tmp, VIRTUAL_SCREEN_WIDTH/2, VIRTUAL_SCREEN_HEIGHT/2, 0);
    flush_buffer();
    al_rest(0.05);

    printf(
"                             GobMan V1.1 July, 1994\n                "\
"   Copyright (C) 1992,1994 By Filipe Mateus\n\n      This game is dis"\
"tributed as shareware, you're encouraged to copy it.\n      If you en"\
"joy it, you should register your copy.\n      By registering, you'll "\
"receive a GobMan with more challanges\n      and more levels.\n\n    "\
"    GobMan registration: $10 + $3 shipping ($5 shipping overseas)\n\n"\
"               Canadian funds in Canada and US funds for others.\n   "\
"  Please send a check, with your name, address and disk size to:\n\n "\
"                          Filipe Mateus\n                           7"\
"128 Richard Hogen\n                           St-Hubert, Quebec, Cana"\
"da\n                           J3Y 8P6\n\n    Big thanks to: Simon Ca"\
"dieux, Dominique Bouvrette, Sébastien Houde,\n                   and "\
"Alain Cadieux\n"
    );
}
