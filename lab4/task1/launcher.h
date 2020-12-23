
#ifndef LAB4_LAUNCHER_H
#define LAB4_LAUNCHER_H

#include <Windows.h>

int init_and_run_procs();
int launch_process_default(const char *exe, char *cmd, PROCESS_INFORMATION *pi);

#endif // !LAB4_LAUNCHER_H
