#ifndef ERROR_H_
#define ERROR_H_

#define PANIC(ec) (exit(ec));

enum Errors {
    E_ALLOC = 128,
    E_MENU_NULL_HANDLER,
    E_FOLDER_CREATION,
};

void panic(int err);
void print_error(int err);
const char* get_error_message(int err);

#endif // !ERROR_H_
