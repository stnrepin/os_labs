#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <launcher.h>
#include <reader_writer.h>
#include <error.h>

#pragma comment(lib, "winmm.lib")

int main(int argc, char **argv) {
    int err;
    err = 0;

    srand(time(NULL));

    if (argc > 3) {
        err = E_INVALID_ARGS;
        goto out;
    }

    if (argc == 1) {
        printf("Launch processes\n");
        err = init_and_run_procs();
        goto out;
    }

    if (argv[1][0] == 'r' && argv[1][1] == '\0') {
        printf("Launch Reader\n");
        err = reader_run();
    }
    else if (argv[1][0] == 'w' && argv[1][1] == '\0') {
        printf("Launch Writer\n");
        err = writer_run();
    }
    else {
        err = E_INVALID_ARGS;
    }

out:
    if (err != 0) {
        print_error(err);
    }
    return err;
}
