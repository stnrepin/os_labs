#include <actions.h>

#include <stdio.h>
#include <Windows.h>
#include <error.h>

int list_drives(void *arg) {
    int res;
    wchar_t buf[MAX_STR]
}

int create_dir(void *arg) {
    BOOL res;
    wchar_t *name;

    name = (wchar_t*)arg;
    puts("Creating...");
    res = CreateDirectory(name, NULL);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    puts("Done");

    return 0;
}

int remove_dir(void *arg) {
    BOOL res;
    wchar_t *name;

    name = (wchar_t*)arg;
    puts("Removing...");
    res = RemoveDirectory(name);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    puts("Done");

    return 0;
}
