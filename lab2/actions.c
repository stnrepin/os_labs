#include <actions.h>

#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#include <stddef.h>
#include <inttypes.h>
#include <math.h>

#include <Windows.h>

#include <error.h>

//
// Task 1
//

int print_system_info(void *arg) {
    SYSTEM_INFO si;
    const char *cstr;
    char buf[MAX_STR];
    size_t buf_i;
    int i;
    DWORD proc_mask;

    GetSystemInfo(&si);

    cstr = "unknown";
    switch (si.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            cstr = "x64";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            cstr = "arm";
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            cstr = "arm64";
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            cstr = "ia64";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            cstr = "x86";
            break;
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
            break;
    }
    printf("Processor arch: %s\n", cstr);

    printf("Processor level: %ld\n", si.wProcessorLevel);
    printf("Processor revision: 0x%x\n", si.wProcessorRevision);
    printf("Number of processors: %ld\n", si.dwNumberOfProcessors);

    proc_mask = si.dwActiveProcessorMask;
    buf_i = 0;
    for (i = 0; i < sizeof(proc_mask)*CHAR_BIT; i++) {
        if (proc_mask & (1 << i)) {
            buf_i += sprintf(&buf[buf_i], "%d ", i);
        }
    }
    buf[buf_i] = '\0';
    printf("Active provessors: %s\n", buf);
 
    printf("Page size: %ldB\n", si.dwPageSize);
    printf("Allocation granularity: %ld\n", si.dwAllocationGranularity);
    printf("Minimum address: 0x%p\n", si.lpMinimumApplicationAddress);
    printf("Maximum address: 0x%p\n", si.lpMaximumApplicationAddress);

    return 0;
}

int print_global_mem_status(void *arg) {
    BOOL res;
    MEMORYSTATUSEX ms;
    size_t i, desc_len;
    char buf[MAX_STR];
    const char *field_val_u;
    double field_val;

    struct MemoryStatusFieldWithDesc {
        DWORDLONG *field;
        const char* desc;
    } fields[] = {
        { &ms.ullTotalPhys, "Total physical memory" },
        { &ms.ullAvailPhys, "Available physical memory" },
        { &ms.ullTotalPageFile, "Committed memory limit" },
        { &ms.ullAvailPageFile, "Available commitable memory" },
        { &ms.ullTotalVirtual, "Total virtual memory" },
        { &ms.ullAvailVirtual, "Available virtual memory" },
    };

    ms.dwLength = sizeof(MEMORYSTATUSEX);
    res = GlobalMemoryStatusEx(&ms);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    printf("Physical memory in use: %d\n", ms.dwMemoryLoad);

    for (i = 0; i < ARRAYSIZE( fields ); i++ ) {
        field_val_u = bytes_to_human_round(*fields[i].field, &field_val);

        desc_len = strlen(fields[i].desc);
        strncpy(buf, fields[i].desc, desc_len);
        strcpy(&buf[desc_len], ": %g%s\n");
        printf(buf, field_val, field_val_u);
    }

    return 0;
}

int print_mem_status(void *arg) {
    void *addr;
    MEMORY_BASIC_INFORMATION mbi;
    size_t sz;
    double reg_sz;
    char buf[MAX_STR];
    const char* reg_sz_u;
    
    printf("Current function address: 0x%p\n", &print_mem_status);

    printf("Enter address to the region of page to be queried (hex): ");
    scanf("%p", &addr);

    sz = VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
    if (sz == 0) {
        return E_WINDOWS_ERROR;
    }

    reg_sz_u = bytes_to_human_round(mbi.RegionSize, &reg_sz);

    printf("Base address: %p\n", mbi.BaseAddress);
    printf("Base address of a range of pages allocated: %p\n", mbi.AllocationBase);
    mem_protection_constant_to_str(mbi.AllocationProtect, buf, MAX_STR);
    printf("Memory protection option: %s\n", buf);
    printf("Region size: %g%s\n", reg_sz, reg_sz_u);
    printf("State of the pages: %s\n", mbi.State == MEM_COMMIT
                                        ? "commited"
                                        : mbi.State == MEM_FREE
                                            ? "free"
                                            : "reserved");
    mem_protection_constant_to_str(mbi.Protect, buf, MAX_STR);
    printf("Access protection: %s\n", buf);
    printf("Type of pages: %s\n", mbi.Type == MEM_IMAGE
                                    ? "image"
                                    : mbi.State == MEM_MAPPED
                                        ? "mapped"
                                        : "private");
    return 0;
}

int virtual_alloc_auto(void *arg) {
    return 0;
}

int virtual_alloc_manual(void *arg) {
    return 0;
}

int virtual_alloc_phy_auto(void *arg) {
    return 0;
}

int virtual_alloc_phy_manual(void *arg) {
    return 0;
}

int write_to_addr(void *arg) {
    return 0;
}

int virtual_protect(void *arg) {
    return 0;
}

int virtual_protect_check(void *arg) {
    return 0;
}

int virtual_free_addr(void *arg) {
    return 0;
}


//
// Task 2
//

int write_mapping_file(void *arg) {
    return 0;
}

int read_mapping_file(void *arg) {
    return 0;
}

//
// Utils
//

wchar_t *readlinew(wchar_t *buf, size_t size) {
    size_t len;
    buf = fgetws(buf, size, stdin);
    len = wcslen(buf);
    if (len > 0) {
        buf[len-1] = '\0';
    }
    return buf;
}

const char* bytes_to_human_round(uint64_t bytes, /*out*/ double *size) {
    int unit_index, i;

    i = 0;
    *size = bytes;
    while (*size >= 1024 && i < HUMAN_READABLE_UNITS_COUNT - 1) {
        *size /= 1024;
        i++;
    }
    *size = round((*size)*100)/100;
    return kHumanReadableUnits[i];
}

void mem_protection_constant_to_str(DWORD val, char* buf, size_t sz) {
    struct ConstDesc {
        int konst;
        const char* desc;
    } consts[] = {
        { PAGE_EXECUTE, "PAGE_EXECUTE" },
        { PAGE_EXECUTE_READ, "PAGE_EXECUTE_READ" },
        { PAGE_EXECUTE_READWRITE, "PAGE_EXECUTE_READWRITE" },
        { PAGE_EXECUTE_WRITECOPY, "PAGE_EXECUTE_WRITECOPY" },
        { PAGE_NOACCESS, "PAGE_NOACCESS" },
        { PAGE_READONLY, "PAGE_READONLY" },
        { PAGE_READWRITE, "PAGE_READWRITE" },
        { PAGE_WRITECOPY, "PAGE_WRITECOPY" },
        { PAGE_TARGETS_INVALID, "PAGE_TARGETS_INVALID" },   // Values equal
        { PAGE_TARGETS_NO_UPDATE, "PAGE_TARGETS_NO_UPDATE" }, //
        { PAGE_GUARD, "PAGE_GUARD" },
        { PAGE_NOCACHE, "PAGE_NOCACHE" },
        { PAGE_WRITECOMBINE, "PAGE_WRITECOMBINE" },
        { PAGE_ENCLAVE_THREAD_CONTROL, "PAGE_ENCLAVE_THREAD_CONTROL" },
        { PAGE_ENCLAVE_UNVALIDATED, "PAGE_ENCLAVE_UNVALIDATED" },
    };
    int i, needed, buf_i;

    buf_i = 0;
    for (i = 0; i < ARRAYSIZE(consts); i++) {
        if (val & consts[i].konst) {
            needed = strlen(consts[i].desc) + 1 + 1; // buf_i + space + nul
            if (buf_i + needed > sz) { 
                buf_i = sz-1;
                break;
            }
            strcpy(&buf[buf_i], consts[i].desc);
            buf_i += strlen(consts[i].desc);
            buf[buf_i++] = ' ';
        }
    }
    buf[buf_i] = '\0';
}