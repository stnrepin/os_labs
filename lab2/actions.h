#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <stdint.h>
#include <wchar.h>
#include <Windows.h>

static_assert(sizeof(DWORD) == sizeof(long), "Platform should define DWORD as long");

#define MAX_STR 256

int print_system_info(void *arg);
int print_global_mem_status(void *arg);
int print_mem_status(void *arg);
int virtual_alloc_auto(void *arg);
int virtual_alloc_manual(void *arg);
int virtual_alloc_phy_auto(void *arg);
int virtual_alloc_phy_manual(void *arg);
int write_to_addr(void *arg);
int virtual_protect(void *arg);
int virtual_protect_check(void *arg);
int virtual_free_addr(void *arg);

int write_mapping_file(void *arg);
int read_mapping_file(void *arg);

wchar_t *readlinew(wchar_t *buf, size_t size);

#define HUMAN_READABLE_UNITS_COUNT 5
static char kHumanReadableUnits[HUMAN_READABLE_UNITS_COUNT][4] = {
    "B", "KiB", "MiB", "GiB", "TiB",
};
const char* bytes_to_human_round(uint64_t bytes, /*out*/ double *size);

void mem_protection_constant_to_str(DWORD val, char* buf, size_t sz);

#endif // !ACTIONS_H_
