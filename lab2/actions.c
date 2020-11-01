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
    printf("Allocation granularity: %ldB\n", si.dwAllocationGranularity);
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

    printf("Physical memory in use: %d%%\n", ms.dwMemoryLoad);

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

    read_addr(&addr, NULL);

    sz = VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
    if (sz == 0) {
        return E_WINDOWS_ERROR;
    }

    reg_sz_u = bytes_to_human_round(mbi.RegionSize, &reg_sz);

    printf("Base address: 0x%p\n", mbi.BaseAddress);
    printf("Base address of a range of pages allocated: 0x%p\n", mbi.AllocationBase);
    mem_protection_constant_to_str(mbi.AllocationProtect, buf, MAX_STR);
    printf("Initial protection option: %s\n", buf);
    printf("Region size: %g%s\n", reg_sz, reg_sz_u);
    printf("State of the pages: %s\n", mbi.State == MEM_COMMIT
                                        ? "commited"
                                        : mbi.State == MEM_FREE
                                            ? "free"
                                            : "reserved");
    mem_protection_constant_to_str(mbi.Protect, buf, MAX_STR);
    printf("Current protection option: %s\n", buf);
    printf("Type of pages: %s\n", mbi.Type == MEM_IMAGE
                                    ? "image"
                                    : mbi.State == MEM_MAPPED
                                        ? "mapped"
                                        : "private");
    return 0;
}

int virtual_alloc(void *arg) {
    BOOL res;
    void *addr, *ptr;
    size_t sz;

    res = read_region(&addr, "(0 to auto choose)", 0, NULL);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    printf("Enter the size of the region to allocate (in bytes): ");
    scanf("%zu", &sz);

    ptr = VirtualAlloc(addr, sz,
                       MEM_RESERVE | (*(int*)arg ? MEM_COMMIT : 0),
                       PAGE_READWRITE);
    printf("Alloced memory pointer: 0x%p\n", ptr);
    if (ptr == NULL) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int write_to_addr(void *arg) {
    int res;
    void *addr;
    unsigned char val;

    res = 0;
    do {
        read_addr(&addr, "(0 to stop)");
        if (addr == NULL) {
            break;
        }

        printf("Enter the value to set (hex): 0x");
        scanf("%hhx", &val);

        __try {
            memset(addr, val, 1);
        }
        __except (page_fault_exception_filter(GetExceptionCode())) {
            res = E_PAGE_ACCESS_VIOLATION;
            break;
        }
    } while (addr != NULL && res == 0);
    return res;
}

int read_from_addr(void *arg) {
    int res;
    void *addr;
    unsigned char val;

    res = 0;
    do {
        read_addr(&addr, "(0 to stop)");
        if (addr == NULL) {
            break;
        }

        __try {
            memcpy(&val, addr, 1);
            printf("Pointee value: 0x%hhx\n", val);
        }
        __except (page_fault_exception_filter(GetExceptionCode())) {
            res = E_PAGE_ACCESS_VIOLATION;
        }
    } while (addr != NULL && res == 0);
    return res;
}

int virtual_protect(void *arg) {
    BOOL res;
    void *addr;
    int i;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD dummy;
    char buf[MAX_STR];

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
        { PAGE_TARGETS_NO_UPDATE, "PAGE_TARGETS_NO_UPDATE" }, //
    };

    res = read_region(&addr, NULL, 1, &mbi);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    printf("Available protect options:\n");
    for (i = 0; i < ARRAYSIZE(consts); i++) {
        printf("%d - %s\n", i+1, consts[i].desc);
    }
    do {
        printf("Enter the index of the required options: ");
        scanf("%d", &i);
        i--;
    } while (i < 0 || i >= ARRAYSIZE(consts));

    res = VirtualProtect(addr, mbi.RegionSize, consts[i].konst, &dummy);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    res = VirtualQuery(addr, &mbi, sizeof(mbi));
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    mem_protection_constant_to_str(mbi.Protect, buf, MAX_STR);
    printf("New page memory-protection option: %s", buf);
    return 0;
}

int virtual_free(void *arg) {
    BOOL res;
    void* addr;
    MEMORY_BASIC_INFORMATION mbi;

    res = read_region(&addr, NULL, 1, &mbi);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    res = VirtualFree(mbi.BaseAddress, 0, MEM_RELEASE);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    printf("Memory freed\n");
    return 0;
}


//
// Task 2
//

int run_as_writer(void *arg) {
    int res;
    BOOL wres;
    unsigned char buf[MAX_STR];
    int i;
    HANDLE fh, mh;
    void *map_addr;

    res = 0;

    printf("Create mapping...\n");

    fh = CreateFile(kMappingFilePath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fh == INVALID_HANDLE_VALUE) {
        res = E_WINDOWS_ERROR;
        goto ret;
    }

    mh = CreateFileMapping(fh, NULL, PAGE_READWRITE, 0, MAX_STR, L"MappingFile");
    if (mh == INVALID_HANDLE_VALUE) {
        res = E_WINDOWS_ERROR;
        goto close_fh;
    }

    map_addr = MapViewOfFile(mh, FILE_MAP_WRITE, 0, 0, 0);
    if (map_addr == NULL) {
        res = E_WINDOWS_ERROR;
        goto close_mh;
    }

    printf("Enter bytes to send (use 0xff as EOF):\n");
    i = 0;
    do {
        printf("Byte [%d/%d]: 0x", i, MAX_STR-1);
        scanf("%hhx%*c", &buf[i]);
        i++;
    } while(buf[i-1] != 0xff && i < MAX_STR);

    printf("Sending bytes...\n");

    memcpy(map_addr, buf, MAX_STR);

    printf("Press any key to exit\n");
    getch();

    wres = UnmapViewOfFile(map_addr);
    if (!wres) {
        res = E_WINDOWS_ERROR;
    }

close_mh:
    CloseHandle(mh);
close_fh:
    CloseHandle(fh);
ret:
    return res;
}

int run_as_reader(void *arg) {
    int res;
    BOOL wres;
    unsigned char buf[MAX_STR];
    int i, read_bytes_next;
    HANDLE fh;
    void *map_addr;

    res = 0;

    printf("Open mapping...\n");

    fh = OpenFileMapping(FILE_MAP_READ, FALSE, L"MappingFile");
    if (fh == INVALID_HANDLE_VALUE) {
        res = E_WINDOWS_ERROR;
        goto ret;
    }

    map_addr = MapViewOfFile(fh, FILE_MAP_READ, 0, 0, 0);
    if (map_addr == NULL) {
        res = E_WINDOWS_ERROR;
        goto close_fh;
    }

    printf("Press any key to start reading\n");
    getch();

    printf("Reading bytes...\n");

    i = 0;
    memcpy(buf, map_addr, MAX_STR);
    do {
        printf("Byte [%d/%d]: 0x%hhx\n", i, MAX_STR, buf[i]);
    } while (buf[i++] != 0xff);

    wres = UnmapViewOfFile(map_addr);
    if (!wres) {
        res = E_WINDOWS_ERROR;
    }

close_fh:
    CloseHandle(fh);
ret:
    return res;
}

//
// Utils
//

int page_fault_exception_filter(DWORD ex_code) {
    if (ex_code == EXCEPTION_ACCESS_VIOLATION) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void read_addr(void **addr, const char *suffix_mes) {
    printf("Enter the address %s: 0x", suffix_mes == NULL ? "" : suffix_mes);
    scanf("%p", addr);
}

int read_region(void **addr, const char *suffix_mes, int print_info, MEMORY_BASIC_INFORMATION *mbi_o) {
    BOOL res;
    MEMORY_BASIC_INFORMATION mbi;
    double reg_sz;
    const char *reg_sz_u;

    if (mbi_o == NULL) {
        mbi_o = &mbi;
    }

    printf("Enter the base address of the region %s: 0x", suffix_mes == NULL ? "" : suffix_mes);
    scanf("%p", addr);

    res = VirtualQuery(*addr, mbi_o, sizeof(MEMORY_BASIC_INFORMATION));
    if (!res) {
        return 0;
    }

    if (print_info) {
        printf("The region base address: 0x%p\n", mbi_o->BaseAddress);
        reg_sz_u = bytes_to_human_round(mbi_o->RegionSize, &reg_sz);
        printf("The region size: %g%s\n", reg_sz, reg_sz_u);
    }

    return 1;
}

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