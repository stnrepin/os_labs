#include <error.h>

#include <errno.h>
#include <string.h>

void panic(int err) {
    printf("panic: %s (%d)\n", get_error_message(err), err);
    exit(err);
}

void print_error(int err) {
    printf("error: %s (%d)\n", get_error_message(err), err);
}

const char* get_error_message(int err) {
    switch (err) {
        case E_ALLOC:
            return "alloc failed";
        case E_MENU_NULL_HANDLER:
            return "menu item handler is not set";
        default:
            return strerror(err);
    }
}
