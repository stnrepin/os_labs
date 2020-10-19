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
} FlagInfo;

static const FlagInfo kVolumeInformationFsFlags[] = {
   { "FILE_CASE_PRESERVED_NAMES", 0x00000002 },
   { "FILE_CASE_SENSITIVE_SEARCH", 0x00000001 },
   { "FILE_DAX_VOLUME", 0x20000000 },
   { "FILE_FILE_COMPRESSION", 0x00000010 },
   { "FILE_NAMED_STREAMS", 0x00040000 },
   { "FILE_PERSISTENT_ACLS", 0x00000008 },
   { "FILE_READ_ONLY_VOLUME", 0x00080000 },
   { "FILE_SEQUENTIAL_WRITE_ONCE", 0x00100000 },
   { "FILE_SUPPORTS_ENCRYPTION", 0x00020000 },
   { "FILE_SUPPORTS_EXTENDED_ATTRIBUTES", 0x00800000 },
   { "FILE_SUPPORTS_HARD_LINKS", 0x00400000 },
   { "FILE_SUPPORTS_OBJECT_IDS", 0x00010000 },
   { "FILE_SUPPORTS_OPEN_BY_FILE_ID", 0x01000000 },
   { "FILE_SUPPORTS_REPARSE_POINTS", 0x00000080 },
   { "FILE_SUPPORTS_SPARSE_FILES", 0x00000040 },
   { "FILE_SUPPORTS_TRANSACTIONS", 0x00200000 },
   { "FILE_SUPPORTS_USN_JOURNAL", 0x02000000 },
   { "FILE_UNICODE_ON_DISK", 0x00000004 },
   { "FILE_VOLUME_IS_COMPRESSED", 0x00008000 },
   { "FILE_VOLUME_QUOTAS", 0x00000020 },
};

static const FlagInfo kFileAttrs[] = {
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

static const FlagInfo kFileAttrsAllowedToBeSet[] = {
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
