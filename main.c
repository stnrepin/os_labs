#include <main.h>

#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include <error.h>
#include <menu.h>

int create_directory() {
    BOOL res;
    wchar_t name[MAX_STR];
    
    printf("Enter folder name: ");
    readlinew(name, MAX_STR);

    puts("Creating...");
    res = CreateDirectoryW(name, NULL);
    if (!res) {
        return E_FOLDER_CREATION;
    }
    puts("Done");

    return 0;
}

int main(int argc, char** argv) {
    int err;

    if (argc == 2) {
        SetCurrentDirectory(argv[1]);
    }

    err = launch_menu(build_main_menu);
    if (err != 0) {
        print_error(err);
    }
    return err;
}

wchar_t* readlinew(wchar_t* buf, size_t size) {
    size_t len;
    buf = fgetws(buf, size, stdin);
    len = wcslen(buf);
    buf[len-1] = '\0';
    return buf;
}

int launch_menu(Menu* (*menu_builder)()) {
    int err;
    Menu* menu;

    menu = menu_builder();
    Menu_draw(menu);
    return Menu_run(menu);
}

Menu *build_main_menu() {
    Menu* menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Task 1", task1_menu_handler),
        MENU_ITEM("Task 2", task2_menu_handler),
    );
    return menu;
}

Menu *build_task1_menu() {
    Menu* menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Create Folder", create_directory)
    );
    return menu;
}

Menu *build_task2_menu() {
    Menu* menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("<TODO>", NULL),
    );
    return menu;
}

int task1_menu_handler() {
    return launch_menu(build_task1_menu);
}

int task2_menu_handler() {
    return launch_menu(build_task2_menu);
}
