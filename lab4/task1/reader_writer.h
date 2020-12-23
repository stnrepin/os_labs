#ifndef LAB4_READER_H
#define LAB4_READER_H

#include <stdio.h>
#include <Windows.h>
#include <config.h>

typedef struct {
    int page_sz;
    FILE *writer_log;
    FILE *reader_log;
    HANDLE reader_log_mut;
    HANDLE writer_log_mut;
    HANDLE map_file;
    HANDLE mapping;
    HANDLE reader_sem;
    HANDLE writer_sem;
    HANDLE buf_muts[BUF_COUNT];
    char *buf_mut_names[BUF_COUNT];
} Resources;

int resources_init(Resources *res);
int resources_get(Resources *res, int read);
void resources_free(Resources *res);

int reader_run();
int writer_run();

void rw_log(FILE *log, HANDLE log_mut, const char *mes, int page);

#endif // !LAB4_READER_H
