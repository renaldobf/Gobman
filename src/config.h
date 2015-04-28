#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_FULLSCREEN_MODE false //true

#define CONFIG_FILE "gobman.conf"

void parse_args(int argc, char *argv[]);
void load_conf();
void save_conf();

#endif // CONFIG_H
