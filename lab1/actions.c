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

int list_drivers(void *arg) {
    DWORD drive_flags, size, i;
    char flags_buf[MAX_STR];
    wchar_t names_buf[MAX_STR];

    drive_flags = GetLogicalDrives();
    if (drive_flags == 0) {
        return E_WINDOWS_ERROR;
    }

    ltoa(drive_flags, flags_buf, 2);
    printf("Drivers (flags): %s\n", flags_buf);

    // Substruct 1 because nBufferLength should not include the
    // terminating null char.
    size = GetLogicalDriveStrings(MAX_STR-1, names_buf);
    puts("Drives:");
    i = 0;
    while (i < size) {
        i += printf("\t%S\n", &names_buf[i]) - 1; // 2 extra symbols
                                                  // in format string.
    }
    return 0;
}

int drive_info(void *arg) {
    DWORD drive_flags, type,
          volume_serial, max_comp_len, fs_flags,
          sec_per_clu, bytes_per_sec, free_clu, cluster_count;
    uint64_t free_space;
    char drive_letter, drive_index, mes[MAX_STR*10];
    const char *type_str, *free_space_unit;
    wchar_t root_buf[MAX_STR], volume_buf[MAX_STR], fs_name_buf[MAX_STR];
    int i, mes_end;
    const FlagInfo *fs_flag;
    BOOL res;
    double free_space_h;

    drive_flags = GetLogicalDrives();

    printf("Enter drive root (e.g. 'C:/'): ");
    readlinew(root_buf, MAX_STR);

    type = GetDriveType(root_buf);
    if (type == DRIVE_UNKNOWN) {
        puts("Drive is unknown");
        return 0;
    }
    if (type == DRIVE_NO_ROOT_DIR) {
        puts("Drive root path is invalid");
        return 0;
    }

    type_str = "<unknown>";
    switch (type) {
        case DRIVE_UNKNOWN:
            __fallthrough;
        case DRIVE_NO_ROOT_DIR:
            type_str = "<error>";
            break;
        case DRIVE_REMOVABLE:
            type_str = "removable media";
            break;
        case DRIVE_FIXED:
            type_str = "fixed media";
            break;
        case DRIVE_REMOTE:
            type_str = "remote (network)";
            break;
        case DRIVE_CDROM:
            type_str = "CD-ROM";
            break;
        case DRIVE_RAMDISK:
            type_str = "RAM";
            break;
    }
    printf("Drive type: %s\n", type_str);

    res = GetVolumeInformation(root_buf,
                               volume_buf, MAX_STR, 
                               &volume_serial,
                               &max_comp_len,
                               &fs_flags,
                               fs_name_buf, MAX_STR);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    printf("Volume name: %S\n", volume_buf);
    printf("Volume serial number: 0x%08lx\n", volume_serial);
    printf("Max component length: %ld\n", max_comp_len);
    printf("File system flags: 0x%08lx\n", fs_flags);
    mes_end = 0;
    mes[mes_end] = '\0';
    for (i = 0; i < ARRAY_SIZE(kVolumeInformationFsFlags, FlagInfo); i++) {
        fs_flag = &kVolumeInformationFsFlags[i];
        if (fs_flags & fs_flag->flag) {
            mes[mes_end++] = '\t';
            memcpy(&mes[mes_end], fs_flag->name, strlen(fs_flag->name));
            mes_end += strlen(fs_flag->name);
            mes[mes_end++] = '\n';
            mes[mes_end] = '\0';
        }
    }
    printf("%s", mes);
    printf("File system name: %S\n", fs_name_buf);

    res = GetDiskFreeSpace(root_buf, &sec_per_clu, &bytes_per_sec,
                           &free_clu, &cluster_count);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    free_space = ((uint64_t)bytes_per_sec)*sec_per_clu*free_clu;
    free_space_unit = bytes_to_human_round(free_space, &free_space_h);
    printf("Sectors per cluster: %ld\n", sec_per_clu);
    printf("Bytes per sector: %ld\n", bytes_per_sec);
    printf("Number of free clusters: %ld\n", free_clu);
    printf("Total number of clusters: %ld\n", cluster_count);
    printf("Free space: %g%s\n", free_space_h, free_space_unit);

    return 0;
}

int create_dir(void *arg) {
    BOOL res;
    wchar_t *name;

    name = (wchar_t*)arg;
    puts("Creating...");
    res = CreateDirectory(name, NULL);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    return 0;
}

int remove_dir(void *arg) {
    BOOL res;
    wchar_t *name;

    name = (wchar_t*)arg;
    puts("Removing...");
    res = RemoveDirectory(name);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    return 0;
}

int create_file(void *arg) {
    HANDLE h;
    wchar_t name[MAX_STR];

    printf("Enter file path: ");
    readlinew(name, MAX_STR);
    puts("Creating...");
    h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                   CREATE_NEW, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return E_WINDOWS_ERROR;
    }
    CloseHandle(h);

    return 0;
}

int copy_file(void *arg) {
    BOOL res;
    WStrPair *pair;
    pair = (WStrPair*)arg;
    puts("Copying...");
    res = CopyFile(pair->first, pair->second, TRUE);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int move_file(void *arg) {
    BOOL res;
    WStrPair *pair;
    pair = (WStrPair*)arg;
    puts("Moving...");
    res = MoveFile(pair->first, pair->second);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int move_file_ex(void *arg) {
    BOOL res;
    WStrPair *pair;
    pair = (WStrPair*)arg;
    puts("Moving...");
    res = MoveFileEx(pair->first, pair->second, MOVEFILE_REPLACE_EXISTING);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int get_file_atts(void *arg) {
    HANDLE h;
    DWORD res, attrs;
    size_t i, mes_end;
    wchar_t path[MAX_STR];
    char mes[MAX_STR];
    const FlagInfo *attr;

    h = (HANDLE)arg;
    res = GetFinalPathNameByHandle(h, path, MAX_STR, 0);
    if (res == 0) {
        return E_WINDOWS_ERROR;
    }
    attrs = GetFileAttributes(path);
    if (res == INVALID_FILE_ATTRIBUTES) {
        return E_WINDOWS_ERROR;
    }

    mes_end = 0;
    for (i = 0; i < ARRAY_SIZE(kFileAttrs, FlagInfo); i++) {
        attr = &kFileAttrs[i];
        if (attrs & attr->flag) {
            memcpy(&mes[mes_end], attr->name, strlen(attr->name));
            mes_end += strlen(attr->name);
            mes[mes_end++] = ' ';
        }
    }
    mes[mes_end] = '\0';
    printf("File attributes: %s\n", mes);
    return 0;
}

int set_file_atts(void *arg) {
    HANDLE h;
    DWORD path_size, attrs, attr_val;
    int i, j, attr_count, len;
    char line[30];
    const FlagInfo *file_attr;
    BOOL res;
    wchar_t path[MAX_STR];

    h = (HANDLE)arg;
    path_size = GetFinalPathNameByHandle(h, path, MAX_STR, 0);
    if (path_size == 0) {
        return E_WINDOWS_ERROR;
    }

    attr_count = ARRAY_SIZE(kFileAttrsAllowedToBeSet, FlagInfo);
    len = 0;
    for (i = 0; i < attr_count; i += 1) {
        file_attr = &kFileAttrsAllowedToBeSet[i];
        len = sprintf(line, "%d - %s", i + 1, file_attr->name);
        for (j = len; j < sizeof(line); j++) {
            line[j] = ' ';
        }
        line[j-1] = '\0';
        if (i % 2 != 0 || i == attr_count -  1) {
            line[j-2] = '\n';
        }
        printf("%s", line);
    }

    attrs = 0;
    do {
        printf("Enter file attribute index (0 to stop): ");
        scanf("%d", &i);
        if (i > 0 && i <= attr_count) {
            file_attr = &kFileAttrsAllowedToBeSet[i-1];
            attrs |= file_attr->flag;
            printf("Set %s attribute\n", file_attr->name);
        }
    } while(i != 0);

    res = SetFileAttributes(path, attrs);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    return 0;
}

int get_file_info(void *arg) {
    BOOL res;
    HANDLE h;
    BY_HANDLE_FILE_INFORMATION info;
    uint64_t index, size;
    int i;
    double size_h;
    const char* unit;

    h = (HANDLE)arg;
    res = GetFileInformationByHandle(h, &info);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    index = ((uint64_t)info.nFileIndexHigh << 32) + info.nFileIndexLow;
    size = ((uint64_t)info.nFileSizeHigh << 32) + info.nFileSizeLow;
    unit = bytes_to_human_round(size, &size_h);

    printf("File index: %" PRIu64 "\n", index);
    printf("File size: %g%s\n", size_h, unit);
    printf("Number of links: %lu\n", info.nNumberOfLinks);
    return 0;
}

int get_file_time(void *arg) {
    BOOL res;
    HANDLE h;
    FILETIME ctime, atime, wtime;
    h = (HANDLE)arg;
    res = GetFileTime(h, &ctime, &atime, &wtime);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    printf("Creation time (UTC): ");
    println_filetime(ctime);
    printf("Last access time (UTC): ");
    println_filetime(atime);
    printf("Last write time (UTC): ");
    println_filetime(wtime);

    return 0;
}

int set_file_time(void *arg) {
    HANDLE h;
    BOOL res;
    int c;
    SYSTEMTIME st;
    FILETIME ft;

    h = (HANDLE)arg;

    printf("Enter new date in format YYYY MM DD HH MM SS: ");
    c = scanf("%hd %hd %hd %hd %hd %hd", &st.wYear, &st.wMonth, &st.wDay,
                                         &st.wHour, &st.wMinute, &st.wSecond);
    st.wMilliseconds = 0;
    if (c != 6) {
        return E_FORMAT;
    }

    res = SystemTimeToFileTime(&st, &ft);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    res = SetFileTime(h, &ft, &ft, &ft);
    if (!res) {
        return E_WINDOWS_ERROR;
    }

    return 0;
}

//
// Task 2
//

int run_copy_overlapped(void *arg) {
    int cluster_size, cluster_count, op_cnt;
    HANDLE src_f, dst_f;
    DWORD res, sectors_per_cluster, bytes_per_sector;
    HandlePair hp;
    wchar_t src_name[MAX_STR], dst_name[MAX_STR];
    TimerDiff tmrd;

    res = GetDiskFreeSpace(NULL, &sectors_per_cluster, &bytes_per_sector,
                           NULL, NULL);
    if (res == 0) {
        return E_WINDOWS_ERROR;
    }
    cluster_size = sectors_per_cluster*bytes_per_sector;
    printf("Use cluster size %ld\n", cluster_size);

    printf("Enter the number of clusters per block: ");
    scanf("%d", &cluster_count);
    printf("Enter operation count: ");
    scanf("%d%*c", &op_cnt);

    printf("Enter source file name: ");
    readlinew(src_name, MAX_STR);
    src_f = CreateFile(src_name, GENERIC_READ, 0, NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
    if (src_f == INVALID_HANDLE_VALUE) {
        return E_WINDOWS_ERROR;
    }

    printf("Enter destination file name: ");
    readlinew(dst_name, MAX_STR);
    dst_f = CreateFile(dst_name, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS,
                       FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
    if (dst_f == INVALID_HANDLE_VALUE) {
        return E_WINDOWS_ERROR;
    }

    hp.src = src_f;
    hp.dst = dst_f;
    res = copy_file_overlapped(&hp, cluster_size*cluster_count, op_cnt, &tmrd);
    if (res != 0) {
        return res;
    }

    CloseHandle(src_f);
    CloseHandle(dst_f);

    printf("File copied\n");
    printf("Timer elapsed: %ld\n", tmrd);

    return 0;
}

int copy_file_overlapped(HandlePair* hp,
                         int buf_sz, int op_cnt,
                         /*out*/ TimerDiff* tmrd)
{
    int i, res, offset;
    volatile int ol_op_finished;
    volatile DWORD ol_op_err; 
    BufferedOverlapped *bol;
    Timer tmr;

    res = 0;
    ol_op_finished = 0;
    ol_op_err = ERROR_SUCCESS;

    bol = malloc(op_cnt*sizeof(BufferedOverlapped));
    if (bol == NULL) {
        return E_ALLOC;
    }
    for (i = 0; i < op_cnt; i++) {
        bol[i].ol.hEvent = hp;
        bol[i].op_cnt = op_cnt;
        bol[i].op_finished = &ol_op_finished;
        bol[i].last_err = &ol_op_err;
        bol[i].buf_sz = buf_sz;
        bol[i].buf = malloc(bol[i].buf_sz);
        bol[i].ol.OffsetHigh = 0;
        if (bol[i].buf == NULL) {
            res = E_ALLOC;
            op_cnt = i;
            goto out;
        }
    }

    offset = 0;
    tmr = timer_start();
    for (i = 0; i < op_cnt; i++) {
        bol[i].ol.Offset = offset;
        offset += buf_sz;
        read_src_async(ERROR_SUCCESS, 0, (LPOVERLAPPED)&bol[i]);
    }
    while (ol_op_finished < op_cnt &&
                (ol_op_err == ERROR_SUCCESS || ol_op_err == ERROR_HANDLE_EOF))
    {
        SleepEx(INFINITE, TRUE);
    }
    *tmrd = timer_finish(&tmr);

    if (GetLastError() != ERROR_SUCCESS) {
        res = E_WINDOWS_ERROR;
    }
    if (ol_op_err != ERROR_SUCCESS && ol_op_err != ERROR_HANDLE_EOF) {
        SetLastError(ol_op_err);
        res = E_WINDOWS_ERROR;
    }

out:
    for(i = 0; i < op_cnt; i++) {
        free(bol[i].buf);
    }
    free(bol);
    return res;
}

void CALLBACK read_src_async(DWORD err, DWORD byte_read, LPOVERLAPPED ol) {
    BOOL res;
    BufferedOverlapped *bol;
    HANDLE src;
    DWORD unused;

    bol = (BufferedOverlapped*)ol;
    if (err != ERROR_SUCCESS) {
        *bol->last_err = err;
        return;
    }
    src = ((HandlePair*)bol->ol.hEvent)->src;
    bol->ol.Offset += (bol->op_cnt)*byte_read;
    res = ReadFileEx(src, bol->buf, bol->buf_sz, ol, write_dst_async);
    if (!res) {
        GetOverlappedResult(src, ol, &unused, TRUE);
        *bol->last_err = GetLastError();
    }
}

void CALLBACK write_dst_async(DWORD err, DWORD byte_read, LPOVERLAPPED ol) {
    BOOL res;
    BufferedOverlapped *bol;
    HANDLE dst;
    DWORD unused;

    bol = (BufferedOverlapped*)ol;
    if (err != ERROR_SUCCESS && err != ERROR_HANDLE_EOF) {
        *bol->last_err = err;
        return;
    }
    if (byte_read == 0) {
        *bol->op_finished += 1;
        return;
    }
    dst = ((HandlePair*)bol->ol.hEvent)->dst;
    res = WriteFileEx(dst, bol->buf, byte_read, ol, read_src_async);
    if (!res) {
        *bol->op_finished += 1;
        GetOverlappedResult(dst, ol, &unused, TRUE);
        *bol->last_err = GetLastError();
    }
}

Timer timer_start() {
    Timer t = { .start = GetTickCount() };
    return t;
}

TimerDiff timer_finish(Timer *t) {
    return GetTickCount() - t->start;
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

void println_filetime(FILETIME ft) {
    BOOL res;
    SYSTEMTIME st;

    res = FileTimeToSystemTime(&ft, &st);
    if (!res) {
        puts("<error>");
        return;
    }
    printf("%04d-%02d-%02d %02d:%02d:%02d:%03d\n",
           st.wYear, st.wMonth, st.wDay,
           st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return;
}
