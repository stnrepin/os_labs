#ifndef MAIN_H_
#define MAIN_H_

#ifndef UNICODE
    #error "the program only works with Unicode. Define UNICODE macro."
#endif // !UNICODE

#include <wchar.h>
#include <menu.h>

int wmain(int argc, wchar_t **argv);

int launch_menu(void *arg, Menu *(menu_builder)());

int task1_menu_handler(void *arg);
int create_remove_dir(void*);
int copy_move_file(void*);
int file_attrs(void*);

int task2_menu_handler(void *arg);

Menu *build_main_menu();

Menu *build_task1_menu();
Menu *build_create_remove_dir_menu();
Menu *build_copy_move_file_menu();
Menu *build_file_attrs_menu();

Menu *build_task2_menu();

#endif // !MAIN_H_
