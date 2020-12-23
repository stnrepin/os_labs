#ifndef LAB4_ERROR_H
#define LAB4_ERROR_H

#include <stdio.h>

enum Errors {
    E_ALLOC = 100,
    E_ERROR_HANDLING,
    E_ERRNO_ERROR,
    E_WINDOWS_ERROR,
    E_INVALID_ARGS,
};

void panic(int err);
void print_error(int err);
void print_error_file(FILE *f, int err);
const char *get_error_message(int err);
const char *get_windows_error_message();

#endif // !LAB4_ERROR_H
