#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <Windows.h>

static_assert(sizeof(DWORD) == sizeof(long), "Platform should define DWORD as long");

#define MAX_STR 256

int list_drivers(void *arg);
int drive_info(void* arg);
int create_dir(void *arg);
int remove_dir(void *arg);
int create_file(void *arg);

wchar_t *readlinew(wchar_t *buf, size_t size);

#endif // !ACTIONS_H_
