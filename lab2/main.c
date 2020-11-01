#include <main.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

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

int quit_menu_handler(void *arg) {
    return E_EXIT_REQUESTED;
}

int nope_menu_handler(void *arg) {
    return 0;
}

int task1_menu_handler(void *arg) {
    int res;
    do {
        res = launch_menu(arg, build_task1_menu);
        if (res != 0) {
            break;
        }
        puts("");
    }
    while (1);
    if (res == E_EXIT_REQUESTED) {
        res = 0;
    }
    return res;
}

int info_menu_handler(void *arg) {
    return launch_menu(arg, build_info_menu);
}

int reserve_alloc_menu_handler(void *arg) {
    int p = 0;
    return virtual_alloc(&p);
}

int commit_alloc_menu_handler(void *arg) {
    int p = 1;
    return virtual_alloc(&p);
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
        MENU_ITEM("System and Memory Info", info_menu_handler),
        MENU_ITEM("Get Memory Status", print_mem_status),
        MENU_ITEM("Reserve Memory", reserve_alloc_menu_handler),
        MENU_ITEM("Commit Memory", commit_alloc_menu_handler),
        MENU_ITEM("Write Memory", write_to_addr),
        MENU_ITEM("Read Memory", read_from_addr),
        MENU_ITEM("Set Memory Protection", virtual_protect),
        MENU_ITEM("Free", virtual_free),
        MENU_ITEM("Quit", quit_menu_handler),
    );
    return menu;
}

Menu *build_info_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("System info", print_system_info),
        MENU_ITEM("Global memory status", print_global_mem_status),
        MENU_ITEM("Back", nope_menu_handler),
    );
    return menu;
}

Menu *build_task2_menu() {
    Menu *menu;
    BUILD_MENU(
         menu,
         MENU_ITEM("Run as Writer", run_as_writer),
         MENU_ITEM("Run as Reader", run_as_reader),
    );
    return menu;
}
