#ifndef MAIN_H_
#define MAIN_H_

#ifndef UNICODE
    #error "the program only works with Unicode. Define UNICODE macro."
#endif // !UNICODE

#include <wchar.h>
#include <menu.h>

int wmain(int argc, wchar_t **argv);

int launch_menu(void *arg, Menu *(menu_builder)());
int quit_menu_handler(void *arg);
int nope_menu_handler(void *arg);

int task1_menu_handler(void *arg);
int info_menu_handler(void *arg);
int task2_menu_handler(void *arg);

int task2_menu_handler(void *arg);

Menu *build_main_menu();

Menu *build_task1_menu();
Menu *build_info_menu();
Menu *build_alloc_menu();

Menu *build_task2_menu();

#endif // !MAIN_H_
