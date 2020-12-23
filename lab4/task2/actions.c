#include <actions.h>

#include <string.h>
#include <stdio.h>

#include <error.h>

int server_create(void *arg) {
    ServerCtx *ctx;
    ctx = (ServerCtx*)arg;

    puts("Creating pipe...");
    ctx->pipe = CreateNamedPipe(
                    PIPE_NAME,
                    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
                    PIPE_TYPE_MESSAGE, 1, MAX_STR, 0, 0, NULL);
    if (ctx->pipe == NULL) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int server_connect(void *arg) {
    BOOL res;
    ServerCtx *ctx;
    ctx = (ServerCtx*)arg;

    puts("Waiting connection...");
    res = ConnectNamedPipe(ctx->pipe, NULL);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    puts("Client connected");
    return 0;
}

int server_begin_write(void *arg) {
    ServerCtx *ctx;
    char data[MAX_STR];
    DWORD written;
    ctx = (ServerCtx*)arg;

    printf("Enter the message to send: ");
    fgets(data, MAX_STR, stdin);
    data[strlen(data)-1] = '\0';

    puts("Sending the message...");
    ctx->event = CreateEvent(NULL, 1, 0, NULL);
    ctx->ol.hEvent = ctx->event;
    ctx->ol.Offset = 0;
    ctx->ol.OffsetHigh = 0;

    WriteFile(ctx->pipe, data, strlen(data) + 1, &written, &ctx->ol);
    if (written == 0) {
        return E_WINDOWS_ERROR;
    }
    printf("Message (size %dB) sent\n", written);
    return 0;
}

int server_wait_write(void *arg) {
    ServerCtx *ctx;
    ctx = (ServerCtx*)arg;

    puts("Waiting for sending...");
    WaitForSingleObject(ctx->event,  INFINITE);
    return 0;
}

int server_disconnect(void *arg) {
    ServerCtx *ctx;
    ctx = (ServerCtx*)arg;

    puts("Disconnecting...");

    if (ctx->pipe != NULL) {
        DisconnectNamedPipe(ctx->pipe);
        CloseHandle(ctx->pipe);
    }
    if (ctx->event != NULL) {
        CloseHandle(ctx->event);
    }
    return 0;
}

int client_connect(void *arg) {
    ClientCtx *ctx;
    ctx = (ClientCtx*)arg;

    puts("Connecting to server...");
    ctx->pipe = CreateFile(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (ctx->pipe == INVALID_HANDLE_VALUE) {
        return E_WINDOWS_ERROR;
    }
    puts("Connected to server");
    return 0;
}

int client_read(void *arg) {
    char data[MAX_STR];
    ClientCtx *ctx;
    ctx = (ClientCtx*)arg;

    puts("Reading incoming message...");
    ctx->ol.Offset = 0;
    ctx->ol.OffsetHigh = 0;

    ReadFileEx(ctx->pipe, data, MAX_STR, &ctx->ol, NULL);
    SleepEx(INFINITE, 1);
    printf("Incoming message: %s\n", data);
    return 0;
}

int client_disconnect(void *arg) {
    ClientCtx *ctx;
    ctx = (ClientCtx*)arg;

    puts("Disconnecting from server...");

    if (ctx->pipe != NULL) {
        DisconnectNamedPipe(ctx->pipe);
        CloseHandle(ctx->pipe);
    }
    return 0;
}
