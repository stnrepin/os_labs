#ifndef ERROR_H_
#define ERROR_H_

enum Errors {
    E_ALLOC = 100,
    E_MENU_NULL_HANDLER,
    E_ERROR_HANDLING,
    E_WINDOWS_ERROR,
    E_EXIT_REQUESTED,
};

void panic(int err);
void print_error(int err);
const char *get_error_message(int err);
const char *get_windows_error_message();

#endif // !ERROR_H_
