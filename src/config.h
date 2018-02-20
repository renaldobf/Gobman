#ifndef CONFIG_H
#define CONFIG_H

#include "base.h"

#ifdef ANDROID
#define DEFAULT_FULLSCREEN_MODE true
#else
#define DEFAULT_FULLSCREEN_MODE false
#endif

#define CONFIG_FILE "gobman.conf"

void parse_args(int argc, char *argv[]);
void load_conf();
void save_conf();

#endif // CONFIG_H
