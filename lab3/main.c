#include <main.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <Windows.h>

#include <actions.h>
#include <error.h>
#include <menu.h>

int wmain(int argc, wchar_t **argv) {
    int res, thread_cnt;

    if (argc == 2) {
        SetCurrentDirectory(argv[1]);
    }

    printf("Enter the number of threads: ");
    scanf("%d", &thread_cnt);

    res = launch_menu(&thread_cnt, build_main_menu);
    if (res == 0) {
        puts("Done");
    }
    else {
        print_error(res);
    }
    return res;
}

int task1_menu_handler(void *arg) {
    return call_n_times(20, arg, calc_pi_with_winapi);
}

int task2_menu_handler(void *arg) {
    return call_n_times(20, arg, calc_pi_with_omp);
}

int call_n_times(int n, void *arg, int (*f)(void*)) {
    int i, res;

    for (i = 0; i < n; i++) {
        res = f(arg);
        if (res != 0) {
            return res;
        }
    }
    return 0;
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
        MENU_ITEM("Calculate PI with WinAPI", task1_menu_handler),
        MENU_ITEM("Calculate PI with OpenMP", task2_menu_handler),
    );
    return menu;
}
