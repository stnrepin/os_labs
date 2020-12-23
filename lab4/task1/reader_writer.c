#include <reader_writer.h>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <error.h>

int resources_init(Resources *res) {
    int i, err;
    SYSTEM_INFO si;

    memset(res, 0, sizeof(Resources));

    for (i = 0; i < BUF_COUNT; i++) {
        res->buf_mut_names[i] = malloc(MAX_STR);
        snprintf(res->buf_mut_names[i], MAX_STR, "buffer_muter_%d", i);
    }

    GetSystemInfo(&si);
    res->page_sz = si.dwPageSize;

    res->map_file = CreateFile(MAPPING_FILE, GENERIC_ALL, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (res->map_file == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }
    res->mapping = CreateFileMapping(res->map_file, NULL, PAGE_READWRITE, 0, res->page_sz*BUF_COUNT, MAPPING_NAME);

    res->reader_log = fopen(LOG_READER_FILENAME, "w");
    if (res->reader_log == NULL) {
        goto out_errno;
    }
    res->writer_log = fopen(LOG_WRITER_FILENAME, "w");
    if (res->writer_log == NULL) {
        goto out_errno;
    }
    setvbuf(res->reader_log, NULL, _IONBF, 0);
    setvbuf(res->writer_log, NULL, _IONBF, 0);

    res->reader_log_mut = CreateMutex(NULL, 0, LOG_READER_MUTEX_NAME);
    if (res->reader_log_mut == NULL) {
        goto out_win_err;
    }
    res->writer_log_mut = CreateMutex(NULL, 0, LOG_WRITER_MUTEX_NAME);
    if (res->writer_log_mut == NULL) {
        goto out_win_err;
    }

    res->reader_sem = CreateSemaphore(NULL, 0, BUF_COUNT, SEMAPHORE_READER_NAME);
    if (res->reader_sem == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }
    res->writer_sem = CreateSemaphore(NULL, BUF_COUNT, BUF_COUNT, SEMAPHORE_WRITER_NAME);
    if (res->writer_sem == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }

    for (i = 0; i < BUF_COUNT; i++) {
        res->buf_muts[i] = CreateMutex(NULL, 0, res->buf_mut_names[i]);
        if (res->buf_muts[i] == INVALID_HANDLE_VALUE) {
            goto out_win_err;
        }
    }

    return 0;
out_errno:
    resources_free(res);
    return E_ERRNO_ERROR;
out_win_err:
    resources_free(res);
    return E_WINDOWS_ERROR;
}

int resources_get(Resources* res, int read) {
    int i;
    SYSTEM_INFO si;

    memset(res, 0, sizeof(Resources));

    for (i = 0; i < BUF_COUNT; i++) {
        res->buf_mut_names[i] = malloc(MAX_STR);
        snprintf(res->buf_mut_names[i], MAX_STR, "buffer_muter_%d", i);
    }

    GetSystemInfo(&si);
    res->page_sz = si.dwPageSize;

    res->mapping = OpenFileMapping(read ? GENERIC_READ : GENERIC_WRITE, 0, MAPPING_NAME);

    res->reader_log = fopen(LOG_READER_FILENAME, "a");
    if (res->reader_log == NULL) {
        goto out_errno;
    }
    res->writer_log = fopen(LOG_WRITER_FILENAME, "a");
    if (res->writer_log == NULL) {
        goto out_errno;
    }

    res->reader_log_mut = OpenMutex(SYNCHRONIZE, 0, LOG_READER_MUTEX_NAME);
    if (res->reader_log_mut == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }
    res->writer_log_mut = OpenMutex(SYNCHRONIZE, 0, LOG_WRITER_MUTEX_NAME);
    if (res->writer_log_mut == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }

    res->reader_sem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, 0, SEMAPHORE_READER_NAME);
    if (res->reader_sem == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }
    res->writer_sem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, 0, SEMAPHORE_WRITER_NAME);
    if (res->writer_sem == INVALID_HANDLE_VALUE) {
        goto out_win_err;
    }

    for (i = 0; i < BUF_COUNT; i++) {
        res->buf_muts[i] = OpenMutex(SYNCHRONIZE, 0, res->buf_mut_names[i]);
        if (res->buf_muts[i] == INVALID_HANDLE_VALUE) {
            goto out_win_err;
        }
    }

    return 0;
out_errno:
    resources_free(res);
    return E_ERRNO_ERROR;
out_win_err:
    resources_free(res);
    return E_WINDOWS_ERROR;
}

void resources_free(Resources *res) {
    int i;
    fclose(res->reader_log);
    fclose(res->writer_log);
    CloseHandle(res->reader_log_mut);
    CloseHandle(res->writer_log_mut);
    CloseHandle(res->mapping);
    CloseHandle(res->map_file);
    CloseHandle(res->reader_sem);
    CloseHandle(res->writer_sem);
    for (i = 0; i < BUF_COUNT; i++) {
        CloseHandle(res->buf_muts[i]);
        free(res->buf_mut_names[i]);
    }
}

int reader_run() {
    int err, i, cur_buf_i;
    Resources res;
    int map_size;
    char *data, *map_data;

    err = resources_get(&res, /*read*/ 1);
    if (err != 0) {
        return err;
    }

    data = malloc(sizeof(char)*res.page_sz);
    if (data == NULL) {
        err = E_ALLOC;
        goto out;
    }

    map_size = res.page_sz*BUF_COUNT;
    map_data = MapViewOfFile(res.mapping, FILE_MAP_READ, 0, 0, map_size);
    VirtualLock(map_data, map_size);

    rw_log(res.reader_log, res.reader_log_mut, "Initialized", -1);

    while (1) {
        rw_log(res.reader_log, res.reader_log_mut, "Waiting", -1);

        WaitForSingleObject(res.reader_sem, INFINITE);
        cur_buf_i = WaitForMultipleObjects(BUF_COUNT, res.buf_muts, 0, INFINITE) - WAIT_OBJECT_0;

        rw_log(res.reader_log, res.reader_log_mut, "Reading", cur_buf_i);

        memcpy(data, map_data + cur_buf_i*res.page_sz, res.page_sz);
        Sleep(rand() % 1001 + 500);

        rw_log(res.reader_log, res.reader_log_mut, "Releasing", -1);

        ReleaseMutex(res.buf_muts[cur_buf_i]);
        ReleaseSemaphore(res.writer_sem, 1, NULL);
        Sleep(10);
    }

out:
    if (err != 0) {
        print_error_file(res.reader_log, err);
    }
    resources_free(&res);
    return err;
}

int writer_run() {
    int err, i, cur_buf_i;
    Resources res;
    int map_size;
    char *data, *map_data;
    const char data_tile[] = { 0xbe, 0xee, 0xb0, 0x00 };

    err = resources_get(&res, /*read*/ 0);
    if (err != 0) {
        return err;
    }

    data = malloc(res.page_sz);
    if (data == NULL) {
        err = E_ALLOC;
        goto out;
    }

    map_size = res.page_sz*BUF_COUNT;
    map_data = MapViewOfFile(res.mapping, FILE_MAP_WRITE, 0, 0, map_size);
    VirtualLock(map_data, map_size);

    rw_log(res.writer_log, res.writer_log_mut, "Initialized", -1);

    while (1) {
        for (i = 0; i < res.page_sz; i += sizeof(data_tile)) {
            memcpy(&data[i], data_tile, sizeof(data_tile));
        }

        rw_log(res.writer_log, res.writer_log_mut, "Waiting", -1);

        WaitForSingleObject(res.writer_sem, INFINITE);
        cur_buf_i = WaitForMultipleObjects(BUF_COUNT, res.buf_muts, 0, INFINITE) - WAIT_OBJECT_0;

        rw_log(res.writer_log, res.writer_log_mut, "Writing", cur_buf_i);

        memcpy(map_data + cur_buf_i*res.page_sz, data, res.page_sz);
        Sleep(rand() % 1001 + 500);

        rw_log(res.writer_log, res.writer_log_mut, "Releasing", -1);

        ReleaseMutex(res.buf_muts[cur_buf_i]);
        ReleaseSemaphore(res.reader_sem, 1, NULL);
        Sleep(10);
    }

out:
    if (err != 0) {
        print_error_file(res.writer_log, err);
    }
    resources_free(&res);
    return err;
}

void rw_log(FILE *log, HANDLE log_mut, const char *mes, int page) {
    char buf[MAX_STR];
    DWORD pid, time;

    WaitForSingleObject(log_mut, INFINITE);

    pid = GetCurrentProcessId();
    time = timeGetTime();
    // Да, fputs не вставляет \n, а puts вставляет.
    snprintf(buf, MAX_STR, "[%d] Process #%d: %s\n", time, pid, mes);
    if (page != -1) {
        sprintf(buf + strlen(buf)-1, " on page %d\n", page);
    }
    fputs(buf, log);
    fflush(log);

    ReleaseMutex(log_mut);
}
