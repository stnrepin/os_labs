#include <launcher.h>

#include <conio.h>

#include <config.h>
#include <error.h>
#include <reader_writer.h>

int launch_process_default(const char *exe, char *cmd, PROCESS_INFORMATION *pi) {
    STARTUPINFO si;
    memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    memset(pi, 0, sizeof(PROCESS_INFORMATION));
    CreateProcess(exe, cmd, NULL, NULL, 0, 0,
                  NULL, NULL, &si, pi);
    return 0;
}

int init_and_run_procs() {
    int rw_proc_count, i, err;
    Resources res;
    PROCESS_INFORMATION *readers, *writers;
    char *writer_cmd = "w",
         *reader_cmd = "r",
         *cmd;

    printf("Enter the count of readers/writers: ");
    scanf("%d", &rw_proc_count);

    readers = malloc(rw_proc_count * sizeof(PROCESS_INFORMATION));
    writers = malloc(rw_proc_count * sizeof(PROCESS_INFORMATION));
    if (readers == NULL || writers == NULL) {
        return E_ALLOC;
    }

    err = resources_init(&res);
    if (err != 0) {
        return err;
    }

    for (i = 0; i < rw_proc_count; i++) {
        cmd = strdup(EXE_FILENAME " w");
        err = launch_process_default(NULL, cmd, &writers[i]);
        if (err != 0) {
            goto out;
        }
        cmd = strdup(EXE_FILENAME " r");
        err = launch_process_default(NULL, cmd, &readers[i]);
        if (err != 0) {
            goto out;
        }
    }

    puts("Press any key to kill all processes");
    _getch();

    for (i = 0; i < rw_proc_count; i++) {
        TerminateProcess(writers[i].hProcess, 0);
        TerminateProcess(readers[i].hProcess, 0);
        CloseHandle(writers[i].hProcess);
        CloseHandle(writers[i].hThread);
        CloseHandle(readers[i].hProcess);
        CloseHandle(readers[i].hThread);
    }
out:
    resources_free(&res);
    return err;
}
