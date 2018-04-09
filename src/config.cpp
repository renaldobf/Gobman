#include "config.h"
#include "base.h"
#include "resources.h"

#include <cstdio>
#include <ctype.h>
#include <cstring>

#define PRINT_VERSION() printf(\
    APPLICATION_NAME " - v" APPLICATION_VERSION "\n"\
    "Built: %s\n"\
    COPYRIGHT_STRING "\n"\
    "\n" , __DATE__)

#define PRINT_HELP(a) printf(\
    APPLICATION_NAME " - v" APPLICATION_VERSION "\n"\
    "Usage: %s [OPTION]...\n"\
    "\n"\
    " Startup:\n"\
    "   -v,  --version       Shows the version and exits.\n"\
    "   -h,  --help          Shows this help text.\n"\
    "\n"\
    " Screen options:\n"\
    "        --fullscreen    Runs the game in fullscreen.\n"\
    "        --windowed      Runs the game in window mode.\n"\
    "\n"\
    " Sound options:\n"\
    "        --nosound       Mute all sound effects.\n"\
    /*"        --volume        Set sound volume to \n"*/\
    "\n", a)

bool has_config_changed;

void parse_args(int argc, char *argv[]) {
    for (int i=1; i<argc; i++) {

        if (strcmp(argv[i], "--version") == 0
        ||  strcmp(argv[i], "-v") == 0) {
            PRINT_VERSION();
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "--help") == 0
             ||  strcmp(argv[i], "-h") == 0) {
            PRINT_HELP(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "--fullscreen") == 0) {
            fullscreen_override = true;
        }
        else if (strcmp(argv[i], "--windowed") == 0) {
            windowed_override = true;
        }
        else if (strcmp(argv[i], "--nosound") == 0) {
            mute_override = true;
        }
        else {
            fprintf(stderr,
                "%s: Option is not valid: \'%s\'\n"
                "Try \'%s --help\' for more information\n",
                argv[0], argv[i], argv[0]);
            exit(EXIT_FAILURE);
        }

    }
}

void load_default_conf() {
    is_fullscreen = DEFAULT_FULLSCREEN_MODE;

    key_up    = ALLEGRO_KEY_W;
    key_down  = ALLEGRO_KEY_S;
    key_left  = ALLEGRO_KEY_A;
    key_right = ALLEGRO_KEY_D;
    key_fire  = ALLEGRO_KEY_ENTER;

    joystick_is_installed = false;
    joystick_id = NULL;

    #ifdef ANDROID
    controller_type = TOUCH_SWIPE;
    #else
    controller_type = JOYSTICK;
    #endif

    has_config_changed = true;
}

#define CHAR_BUFFER_SIZE 255

void set_var(const char *scope, const char *variable, const char *value) {
    if (strcmp(scope, "") == 0) {
/* ---------------------------- FULLSCREEN ---------------------------------- */
        if (strcmp(variable, "fullscreen") == 0) {
            if (strcmp(value, "true") == 0) {
                is_fullscreen = true;
            }
            else if (strcmp(value, "false") == 0) {
                is_fullscreen = false;
            }
        }
/* ----------------------------- NO SOUND ----------------------------------- */
        else if (strcmp(variable, "nosound") == 0) {
            if (strcmp(value, "true") == 0) {
                mute = true;
            }
            else if (strcmp(value, "false") == 0) {
                mute = false;
            }
        }
    }
/* ----------------------------- JOYSTICK ----------------------------------- */
    else if (strcmp(scope, "Joystick") == 0) {
        if (strcmp(variable, "is_installed") == 0) {
            if (strcmp(value, "true") == 0) {
                joystick_is_installed = true;
            }
            else if (strcmp(value, "false") == 0) {
                joystick_is_installed = false;
            }
        }
        else if (strcmp(variable, "joystick_name") == 0) {
            strcpy(joystick_name, value);
            remove_space(joystick_name);
        }
        else if (strcmp(variable, "vertical_stick") == 0) {
            joystick_vertical_stick = atoi(value);
        }
        else if (strcmp(variable, "vertical_axis") == 0) {
            joystick_vertical_axis = atoi(value);
        }
        else if (strcmp(variable, "vertical_positive") == 0) {
            joystick_vertical_positive = atoi(value);
        }
        else if (strcmp(variable, "horizontal_stick") == 0) {
            joystick_horizontal_stick = atoi(value);
        }
        else if (strcmp(variable, "horizontal_axis") == 0) {
            joystick_horizontal_axis = atoi(value);
        }
        else if (strcmp(variable, "horizontal_positive") == 0) {
            joystick_horizontal_positive = atoi(value);
        }
    }
/* ------------------------------- KEYS ------------------------------------- */
    else if (strcmp(scope, "Keys") == 0) {
        if (strcmp(variable, "key_up") == 0) {
            key_up = atoi(value);
        }
        else if (strcmp(variable, "key_down") == 0) {
            key_down = atoi(value);
        }
        else if (strcmp(variable, "key_left") == 0) {
            key_left = atoi(value);
        }
        else if (strcmp(variable, "key_right") == 0) {
            key_right = atoi(value);
        }
        else if (strcmp(variable, "key_fire") == 0) {
            key_fire = atoi(value);
        }
    }
/* ------------------------------- END!!! ----------------------------------- */
}

void load_conf() {
    load_default_conf();

    FILE *file = fopen(CONFIG_FILE, "rb");
    if (file == NULL) return;

    char buffer[CHAR_BUFFER_SIZE];
    char category[CHAR_BUFFER_SIZE] = "";
    char name[CHAR_BUFFER_SIZE] = "";
    char value[CHAR_BUFFER_SIZE] = "";

    int buffer_read = 0;

    int i = CHAR_BUFFER_SIZE;
    int i_category = 0;
    int i_name = 0;
    int i_value = 0;

    bool comment = false;
    bool has_category = false;
    bool has_name = false;
    bool has_begun = false;

    while (true) {
        i++;
        if (i >= buffer_read || i >= CHAR_BUFFER_SIZE) {
            buffer_read = fread(buffer, 1, CHAR_BUFFER_SIZE, file);
            if (buffer_read <= 0)
                break;
            i=0;
        }
        if (comment) {
            if (buffer[i] == '\n') comment = false;
            continue;
        }
        if (buffer[i] == '#') {
            comment = true;
            continue;
        }
        if (!has_begun) {
            if (isspace(buffer[i])) continue;
            if (buffer[i] == '[') {
                has_category = true;
                i_category = 0;
                continue;
            }
            if (buffer[i] == ']') {
                category[i_category] = '\0';
                has_category = false;
                continue;
            }
            if (has_category) {
                category[i_category] = buffer[i];
                i_category++;
                continue;
            }
            else {
                has_begun = true;
            }
        }
        if (!has_name) {
            if (buffer[i] == '=') {
                name[i_name] = '\0';
                has_name = true;
                i_value = 0;
                continue;
            }
            name[i_name] = buffer[i];
            i_name++;
        }
        else {
            if (buffer[i] == '\n') {
                value[i_value] = '\0';
                set_var(category, name, value);
                has_name = false;
                has_begun = false;
                i_name = 0;
                continue;
            }
            value[i_value] = buffer[i];
            i_value++;
        }
    } // while
    if (has_name) {
        value[i_value] = '\0';
        set_var(category, name, value);
    }
    fclose(file);
}

void save_conf() {
    if (!has_config_changed) return;

    FILE *file = fopen(CONFIG_FILE, "wb");
    if (file == NULL) return;

    /* ------------------- INFO AND TIMESTAMP HEADER ------------------------ */
    {
        time_t timer;
        char buffer[25];
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
        strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(file, "# Gobman configuration file\n# Last modified: %s\n\n", buffer);
    }
    /* ----------------------- GENERAL CONFIG ------------------------------- */
    fprintf(file, "fullscreen=%s\n", is_fullscreen ? "true" : "false");
    fprintf(file, "nosound=%s\n", mute ? "true" : "false");

    fprintf(file, "\n");

    /* ---------------------- JOYSTICK CONFIG ------------------------------- */
    fprintf(file, "[Joystick]\n");

    if (!joystick_is_installed)
        fprintf(file, "is_installed=false\n");
    else
        fprintf(file, "is_installed=true\n"
            "joystick_name=%s\n"
            "vertical_stick=%d\n"
            "vertical_axis=%d\n"
            "vertical_positive=%d\n"
            "horizontal_stick=%d\n"
            "horizontal_axis=%d\n"
            "horizontal_positive=%d\n",
            joystick_name,
            joystick_vertical_stick,
            joystick_vertical_axis,
            joystick_vertical_positive,
            joystick_horizontal_stick,
            joystick_horizontal_axis,
            joystick_horizontal_positive);

    fprintf(file, "\n");
    /* ------------------------ KEYS CONFIG --------------------------------- */
    fprintf(file, "[Keys]\n"
        "key_up=%d\n"
        "key_down=%d\n"
        "key_left=%d\n"
        "key_right=%d\n"
        "key_fire=%d\n",
        key_up, key_down, key_left, key_right, key_fire);
    /* ---------------------------------------------------------------------- */
    fprintf(file, "\n");
    fflush(file);
    fclose(file);
}
