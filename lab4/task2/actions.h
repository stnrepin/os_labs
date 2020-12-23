#ifndef ACTIONS_H
#define ACTIONS_H

#include <Windows.h>

#define MAX_STR 64
#define PIPE_NAME "\\\\.\\pipe\\Lab4Pipe"

typedef struct {
    HANDLE pipe;
    HANDLE event;
    OVERLAPPED ol;
} ServerCtx, ClientCtx;

int server_create(void *arg);
int server_connect(void *arg);
int server_begin_write(void *arg);
int server_wait_write(void *arg);
int server_disconnect(void *arg);

int client_connect(void *arg);
int client_read(void *arg);
int client_disconnect(void *arg);

#endif // !ACTIONS_H
