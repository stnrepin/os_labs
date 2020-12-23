#include <error.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <Windows.h>

void panic(int err) {
    printf("Panic: %s (%d)\n", get_error_message(err), err);
    exit(err);
}

void print_error(int err) {
    print_error_file(stdout, err);
}

void print_error_file(FILE *f, int err) {
    fprintf(f, "Error: %s (%d)\n", get_error_message(err), err);
}

const char *get_error_message(int err) {
    switch (err) {
        case E_ALLOC:
            return "alloc failed";
        case E_ERROR_HANDLING:
            return "failed error handling";
        case E_ERRNO_ERROR:
            return strerror(errno);
        case E_WINDOWS_ERROR:
            return get_windows_error_message();
        case E_INVALID_ARGS:
            return "invalid args";
    }
    return strerror(err);
}

const char *get_windows_error_message() {
    size_t size;
    char *buf;
    DWORD mes_id;

    mes_id = GetLastError();
    if (mes_id == 0) {
        return "<no-error>";
    }
    buf = NULL;
    size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL, mes_id, LANG_NEUTRAL, (LPSTR)&buf, 0, NULL);
    if (size < 2) {
        panic(E_ERROR_HANDLING);
    }
    buf[size-2] = '\0'; // Windows uses \n\r
    return buf; // XXX: Memory leak but since error occurred we don't mind.
}
