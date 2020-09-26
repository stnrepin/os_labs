#ifndef MAIN_H_
#define MAIN_H_

#include <string.h>
#include <Windows.h>
#include <menu.h>

int create_directory();

int wmain(int argc, wchar_t **argv);

wchar_t *readlinew(wchar_t *buf, size_t size);
int launch_menu(void *arg, Menu *(menu_builder)());

Menu *build_main_menu();

Menu *build_task1_menu();
int task1_menu_handler(void* arg);
Menu *build_create_remove_dir_menu();

Menu *build_task2_menu();

int task2_menu_handler(void* arg);

#endif // !MAIN_H_