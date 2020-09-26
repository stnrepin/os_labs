#include "menu.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <error.h>

MenuItem *MenuItem_new() {
    MenuItem *item;
    item = malloc(sizeof(MenuItem));
    if (item == NULL) {
        panic(E_ALLOC);
    }
    return item;
}

void MenuItem_free(MenuItem *menu_item) {
    free(menu_item);
}

MenuItem *MenuItem_copy(MenuItem *src) {
    MenuItem *dest;

    dest = MenuItem_new();
    memcpy(dest, src, sizeof(MenuItem));

    return dest;
}

Menu *Menu_new() {
    Menu *menu;

    menu = malloc(sizeof(Menu));
    if (menu == NULL) {
        panic(E_ALLOC);
    }
    menu->count = 0;

    return menu;
}

void Menu_free(Menu *menu) {
    if (menu != NULL) {
        Menu_free_items(menu);
        menu->count = 0;
        free(menu);
    }
}

void Menu_free_items(Menu *menu) {
    int i;
    if (menu->items != NULL) {
        for (i = 0; i < menu->count; i++) {
            MenuItem_free(menu->items[i]);
        }
        free(menu->items);
    }
}

Menu *Menu_build(char *caption, MenuItem *items, int count) {
    int i;
    Menu *m;

    m = Menu_new();

    m->items = malloc(count * sizeof(MenuItem));
    if (m->items == NULL) {
        PANIC(E_ALLOC);
    }
    for (i = 0; i < count; i++) {
        m->items[i] = MenuItem_copy(&items[i]);
    }
    strncpy(m->caption, caption, MAX_CAPTION_LEN);
    m->count = i;

    return m;
}

void Menu_draw(Menu *menu) {
    int i;
    printf("%s\n", menu->caption);
    for (i = 0; i < menu->count; i++) {
        printf("%d - %s\n", i+1, menu->items[i]->caption);
    }
}

int Menu_run(Menu *menu, void* arg) {
    int err;
    int right_input, item, c;
    MenuHandlerFunc func;

    do {
        printf("[%d-%d]> ", 1, menu->count);
        scanf("%d", &item);
        /* Очищаем буфер ввода, оставшийся после выполнения scanf. */
        while ((c = getchar()) != '\n' && c != EOF) { }

        right_input = (item >= 1 && item <= menu->count);

    } while (!right_input);

    item--;
    func = menu->items[item]->handler;
    if (func != NULL) {
        err = func(arg);
    }
    else {
        err = E_MENU_NULL_HANDLER;
    }

    return err;
}
