#ifndef ERROR_H_
#define ERROR_H_

enum Errors {
    E_ALLOC = 100,
    E_MENU_NULL_HANDLER,
    E_ERROR_HANDLING,
    E_DRIVE_INVALID_LETTER,
    E_FORMAT,
    E_SAME_FILES,
    E_WINDOWS_ERROR,
};

void panic(int err);
void print_error(int err);
const char *get_error_message(int err);
const char *get_windows_error_message();

#endif // !ERROR_H_
