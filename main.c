#include <main.h>

#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include <actions.h>
#include <error.h>
#include <menu.h>

int wmain(int argc, wchar_t **argv) {
    int res;

    if (argc == 2) {
        SetCurrentDirectory(argv[1]);
    }

    res = launch_menu(NULL, build_main_menu);
    if (res == 0) {
        puts("Done");
    }
    else {
        print_error(res);
    }
    return res;
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

int task1_menu_handler(void *arg) {
    return launch_menu(arg, build_task1_menu);
}

int create_remove_dir(void *arg) {
    wchar_t name[MAX_STR];

    printf("Enter directory name: ");
    readlinew(name, MAX_STR);

    return launch_menu(name, build_create_remove_dir_menu);
}

int copy_move_file(void *arg) {
    WStrPair pair;

    printf("Enter source file/directory path: ");
    readlinew(pair.first, MAX_STR);
    printf("Enter destination file/directory path: ");
    readlinew(pair.second, MAX_STR);

    return launch_menu(&pair, build_copy_move_file_menu);
}

int file_attrs(void *arg) {
    int res;
    HANDLE h;
    OFSTRUCT of;
    wchar_t name[MAX_STR];

    printf("Enter file name: ");
    readlinew(name, MAX_STR);
    h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return E_WINDOWS_ERROR;
    }

    res = launch_menu(h, build_file_attrs_menu);
    CloseHandle(h);
    return res;
}

int task2_menu_handler(void *arg) {
    return launch_menu(arg, build_task2_menu);
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
        MENU_ITEM("List Drivers", list_drivers),
        MENU_ITEM("Driver info", drive_info),
        MENU_ITEM("Create/Remove Directory", create_remove_dir),
        MENU_ITEM("Create File", create_file),
        MENU_ITEM("Copy/Move File", copy_move_file),
        MENU_ITEM("File Attributes", file_attrs),
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

Menu *build_copy_move_file_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Copy File or Directory", copy_file),
        MENU_ITEM("Move File or Directory", move_file),
        MENU_ITEM("Move File or Directory (Replace Existing)", move_file_ex),
    );
    return menu;
}

Menu *build_file_attrs_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Get Fil Attributes", get_file_atts),
        MENU_ITEM("Set File Attributes", set_file_atts),
        MENU_ITEM("Get File Information", get_file_info),
        MENU_ITEM("Get File Time", get_file_time),
        MENU_ITEM("Set File Time", set_file_time),
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
