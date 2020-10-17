#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <stdint.h>
#include <wchar.h>
#include <Windows.h>

static_assert(sizeof(DWORD) == sizeof(long), "Platform should define DWORD as long");

#define MAX_STR 256

#define ARRAY_SIZE(arr, el_t) (sizeof(arr)/sizeof(el_t))

typedef struct {
    wchar_t first[MAX_STR];
    wchar_t second[MAX_STR];
} WStrPair;

typedef struct {
    const char name[MAX_STR];
    DWORD flag;
} FileAttr;

static const FileAttr kFileAttrStringMap[] = {
    { "Archive", 0x20 },
    { "Compressed", 0x800},
    { "Device", 0x40 },
    { "Directory", 0x10 },
    { "Encrypted", 0x4000 },
    { "Hidden", 0x2},
    { "Integrity stream", 0x8000 },
    { "Normal", 0x80 },
    { "Not content indexed", 0x2000 },
    { "No scrub data", 0x20000 },
    { "Offline", 0x1000 },
    { "Readonly", 0x1 },
    { "Recall on data access", 0x400000 },
    { "Recall on open", 0x40000 },
    { "Reparse point", 0x400 },
    { "Sparse file", 0x200 },
    { "System", 0x4 },
    { "Temporary", 0x100 },
    { "Virtual", 0x10000 },
};

static const FileAttr kFileAttrStringMapAllowedToBeSet[] = {
    { "Archive", 0x20 },
    { "Hidden", 0x2},
    { "Normal", 0x80 },
    { "Not content indexed", 0x2000 },
    { "Offline", 0x1000 },
    { "Readonly", 0x1 },
    { "System", 0x4 },
    { "Temporary", 0x100 },
};

int list_drivers(void *arg);
int drive_info(void *arg);
int create_dir(void *arg);
int remove_dir(void *arg);
int create_file(void *arg);
int copy_file(void *arg);
int move_file(void *arg);
int move_file_ex(void *arg);
int get_file_atts(void *arg);
int set_file_atts(void *arg);
int get_file_info(void *arg);
int get_file_time(void *arg);
int set_file_time(void *arg);

typedef struct {
    HANDLE src;
    HANDLE dst;
} HandlePair;

typedef struct {
    OVERLAPPED ol;
    int op_cnt;
    volatile int *op_finished;
    volatile DWORD *last_err;
    int buf_sz;
    char *buf;
} BufferedOverlapped;

typedef struct {
    DWORD start;
} Timer;
typedef DWORD TimerDiff;

int run_copy_overlapped(void *arg);
int copy_file_overlapped(HandlePair *hp, int blk_sz, int op_cnt,
                         /*out*/ TimerDiff *tmrd);
void CALLBACK read_src_async(DWORD err, DWORD byte_read, LPOVERLAPPED ol);
void CALLBACK write_dst_async(DWORD err, DWORD byte_read, LPOVERLAPPED ol);
Timer timer_start();
TimerDiff timer_finish(Timer *t);

wchar_t *readlinew(wchar_t *buf, size_t size);

#define HUMAN_READABLE_UNITS_COUNT 5
static char kHumanReadableUnits[HUMAN_READABLE_UNITS_COUNT][4] = {
    "B", "KiB", "MiB", "GiB", "TiB",
};
const char* bytes_to_human_round(uint64_t bytes, /*out*/ double *size);

void println_filetime(FILETIME ft);

#endif // !ACTIONS_H_
