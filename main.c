#include <main.h>

#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include <actions.h>
#include <error.h>
#include <menu.h>

int create_remove_dir(void *arg) {
    wchar_t name[MAX_STR];

    printf("Enter directory name: ");
    readlinew(name, MAX_STR);

    return launch_menu(name, build_create_remove_dir_menu);
}

int wmain(int argc, wchar_t **argv) {
    int res;

    if (argc == 2) {
        SetCurrentDirectory(argv[1]);
    }

    res = launch_menu(NULL, build_main_menu);
    if (res != 0) {
        print_error(res);
    }
    return res;
}

wchar_t *readlinew(wchar_t *buf, size_t size) {
    size_t len;
    buf = fgetws(buf, size, stdin);
    len = wcslen(buf);
    _putws(buf);
    buf[len-1] = '\0';
    return buf;
}

int launch_menu(void *arg, Menu *(*menu_builder)()) {
    int res;
    Menu *menu;

    menu = menu_builder();
    Menu_draw(menu);
    res = Menu_run(menu, arg);
    Menu_free(menu);
    return res;
}

Menu *build_main_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Task 1", task1_menu_handler),
        MENU_ITEM("Task 2", task2_menu_handler),
    );
    return menu;
}

Menu *build_task1_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Create/Remove Folder", create_remove_dir),
    );
    return menu;
}

Menu *build_create_remove_dir_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Create Directory", create_dir),
        MENU_ITEM("Remove Directory", remove_dir),
    );
    return menu;
}

Menu *build_task2_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("<TODO>", NULL),
    );
    return menu;
}

int task1_menu_handler(void *arg) {
    return launch_menu(arg, build_task1_menu);
}

int task2_menu_handler(void *arg) {
    return launch_menu(arg, build_task2_menu);
}
