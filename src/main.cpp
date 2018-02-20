#include "base.h"
#include "config.h"
#include "setup.h"
#include "main_menu.h"
#include "intro.h"
#include "credits.h"
#include "hiscore.h"
#include "help.h"
#include "level/level.h"

int main (int argc, char *argv[]) {
    parse_args(argc, argv);
    load_conf();
    setup();
    intro();
    bool is_running = true;
    while(is_running) {
    switch (main_menu()) {
            case 1:
                play();
                hiscore(score, level);
                break;
            case 2:
                hiscore();
                break;
            case 4:
                help();
                break;
            case EXIT_PROGRAM:
                is_running = false;
                break;
        }
    }
    //credits();
    save_conf();
    shutdown();
    return 0;
}
