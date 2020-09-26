#ifndef MAIN_H_
#define MAIN_H_

#ifndef UNICODE
    #error "the program only works with Unicode. Define UNICODE macro."
#endif // !UNICODE

#include <wchar.h>
#include <menu.h>

int create_directory();

int wmain(int argc, wchar_t **argv);

int launch_menu(void *arg, Menu *(menu_builder)());

Menu *build_main_menu();

Menu *build_task1_menu();
int task1_menu_handler(void* arg);
Menu *build_create_remove_dir_menu();

Menu *build_task2_menu();

int task2_menu_handler(void* arg);

#endif // !MAIN_H_