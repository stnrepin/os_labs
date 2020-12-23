#include <main.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <Windows.h>

#include <actions.h>
#include <error.h>
#include <menu.h>

int main(int argc, char **argv) {
    int res;

    res = launch_menu(NULL, build_main_menu);
    if (res == 0) {
        puts("Done");
    }
    else {
        print_error(res);
    }
    return res;
}

int launch_menu(void *arg, Menu *(menu_builder)()) {
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

int launch_menu_loop(void *arg, Menu *(*menu_builder)()) {
    int res;
    do {
        res = launch_menu(arg, menu_builder);
        if (res != 0) {
            break;
        }
        puts("");
    } while (1);
    if (res == E_EXIT_REQUESTED) {
        res = 0;
    }
    return res;
}

int server_menu_hander(void *arg) {
    ServerCtx ctx;
    return launch_menu_loop(&ctx, build_server_menu);
}

int client_menu_handler(void *arg) {
    ClientCtx ctx;
    return launch_menu_loop(&ctx, build_client_menu);
}

Menu *build_main_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Server", server_menu_hander),
        MENU_ITEM("Client", client_menu_handler),
    );
    return menu;
}

Menu *build_server_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Create Pipe", server_create),
        MENU_ITEM("Connect Pipe", server_connect),
        MENU_ITEM("Begin Write", server_begin_write),
        MENU_ITEM("Wait Write", server_wait_write),
        MENU_ITEM("Disconnect Pipe", server_disconnect),
        MENU_ITEM("Force Quite", quit_menu_handler),
    );
    return menu;
}

Menu *build_client_menu() {
    Menu *menu;
    BUILD_MENU(
        menu,
        MENU_ITEM("Connect Pipe", client_connect),
        MENU_ITEM("Read Pipe", client_read),
        MENU_ITEM("Disconnect Pipe", client_disconnect),
        MENU_ITEM("Force Quite", quit_menu_handler),
    );
    return menu;
}
