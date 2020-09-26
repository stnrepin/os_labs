#ifndef MAIN_H_
#define MAIN_H_

#include <string.h>
#include <Windows.h>
#include <menu.h>

#define MAX_STR 256

int create_directory();

int wmain(int argc, wchar_t **argv);

wchar_t* readlinew(wchar_t* buf, size_t size);
int launch_menu(Menu* (menu_builder)());
Menu *build_main_menu();
Menu *build_task1_menu();
Menu *build_task2_menu();
int task1_menu_handler();
int task2_menu_handler();

#endif // !MAIN_H_