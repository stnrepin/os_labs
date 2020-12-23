#ifndef MAIN_H_
#define MAIN_H_

#include <menu.h>

int main(int argc, char **argv);

int launch_menu(void *arg, Menu *(menu_builder)());
int quit_menu_handler(void *arg);

int launch_menu_loop(void *arg, Menu *(*menu_builder)());
int server_menu_hander(void *arg);
int client_menu_handler(void *arg);

Menu *build_main_menu();

Menu *build_server_menu();
Menu *build_client_menu();

#endif // !MAIN_H_
