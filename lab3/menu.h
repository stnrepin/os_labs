#ifndef MENU_H_
#define MENU_H_

#define MAX_CAPTION_LEN 64

#define MENU_ITEM(title, func) { title, func }
#define BUILD_MENU(menu, ...)                                        \
    do {                                                             \
        int item_count;                                              \
        MenuItem menu_items[] = { __VA_ARGS__ };                     \
        item_count = sizeof(menu_items)/sizeof(menu_items[0]);       \
        menu = Menu_build("Choose action:", menu_items, item_count); \
    } while(0)                                                       \

typedef int (*MenuHandlerFunc)(void*);

typedef struct {
    char caption[MAX_CAPTION_LEN];
    MenuHandlerFunc handler;
} MenuItem;

typedef struct {
    char caption[MAX_CAPTION_LEN];
    MenuItem **items;
    int count;
} Menu;

MenuItem *MenuItem_new();
void MenuItem_free(MenuItem *menu_item);
MenuItem *MenuItem_copy(MenuItem *src);

Menu *Menu_new();
void Menu_free(Menu *menu);
void Menu_free_items(Menu *menu);
Menu *Menu_build(char *caption, MenuItem *items, int count);
void Menu_draw(Menu *menu);
int Menu_run(Menu *menu, void *arg);

#endif // !MENU_H_
