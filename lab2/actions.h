#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <stdint.h>
#include <wchar.h>
#include <Windows.h>

static_assert(sizeof(DWORD) == sizeof(long), "Platform should define DWORD as long");

#define MAX_STR 256

typedef struct {
    void* addr;
    size_t sz;
    int commit;
} VirtualAllocParam;

int print_system_info(void *arg);
int print_global_mem_status(void *arg);
int print_mem_status(void *arg);
int virtual_alloc(void *arg);
int write_to_addr(void *arg);
int read_from_addr(void *arg);
int virtual_protect(void *arg);
int virtual_free(void *arg);

static const wchar_t kMappingFilePath[MAX_STR] = L"$mapping.bin";

int run_as_writer(void *arg);
int run_as_reader(void *arg);

int page_fault_exception_filter(DWORD ex_code);
void read_addr(void **addr, const char *suffix_mes);
int read_region(void **addr, const char *suffix_mes, int print_info,
                MEMORY_BASIC_INFORMATION *mbi_o);

wchar_t *readlinew(wchar_t *buf, size_t size);

#define HUMAN_READABLE_UNITS_COUNT 5
static char kHumanReadableUnits[HUMAN_READABLE_UNITS_COUNT][4] = {
    "B", "KiB", "MiB", "GiB", "TiB",
};
const char* bytes_to_human_round(uint64_t bytes, /*out*/ double *size);

void mem_protection_constant_to_str(DWORD val, char* buf, size_t sz);

#endif // !ACTIONS_H_
