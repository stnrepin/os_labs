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
int task2_menu_handler(void *arg);
int call_n_times(int n, void *arg, int (*f)(void*));

Menu *build_main_menu();

#endif // !MAIN_H_
