#include <actions.h>

#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#include <Windows.h>
#include <error.h>

int list_drivers(void *arg) {
    DWORD drive_flags, size, i;
    char flags_buf[MAX_STR];
    wchar_t names_buf[MAX_STR];

    drive_flags = GetLogicalDrives();
    if (drive_flags == 0) {
        return E_WINDOWS_ERROR;
    }

    ltoa(drive_flags, flags_buf, 2);
    // DWORD is long.
    printf("Drivers (flags): %s\n", flags_buf);

    // Substruct 1 because nBufferLength should not include the
    // terminating null char.
    size = GetLogicalDriveStrings(MAX_STR-1, names_buf);
    puts("Drives:");
    i = 0;
    while (i < size) {
        i += printf("\t%S\n", &names_buf[i]) - 1; // 2 extra symbols in format string.
    }
    return 0;
}

int drive_info(void* arg) {
    DWORD drive_flags, type,
          volume_serial, max_comp_len, fs_flags,
          sec_per_clu, bytes_per_sec, free_clu, cluster_count;
    char drive_letter, drive_index;
    const char* type_str;
    wchar_t root_buf[MAX_STR], volume_buf[MAX_STR], fs_name_buf[MAX_STR];
    BOOL res;

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
    printf("Volume serial number: %08lx\n", volume_serial);
    printf("Max component length: %ld\n", max_comp_len);
    printf("File system flags: %08lx\n", fs_flags);
    printf("File system name: %S\n", fs_name_buf);

    res = GetDiskFreeSpace(root_buf, &sec_per_clu, &bytes_per_sec,
                           &free_clu, &cluster_count);
    if (!res) {
        return E_WINDOWS_ERROR;
    }
    printf("Sectors per cluster: %ld\n", sec_per_clu);
    printf("Bytes per sector: %ld\n", bytes_per_sec);
    printf("Number of free clusters: %ld\n", free_clu);
    printf("Total number of clusters: %ld\n", cluster_count);

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
    puts("Done");

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
    puts("Done");

    return 0;
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

int create_file(void* arg) {
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
    puts("Done");

    return 0;
}

int copy_file(void* arg) {
    return 0;
}

int move_file(void* arg) {
    return 0;
}

int move_file_ex(void* arg) {
    return 0;
}
