#include <actions.h>

#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <inttypes.h>

#include <Windows.h>
#include <omp.h>

#include <error.h>

//
// Task 1
//

int calc_pi_with_winapi(void *arg) {
    const int kMicrosecondsPerSecond = 1'000'000;
    int err, thread_cnt, i, block_beg;
    HANDLE *threads, *events;
    ThreadArg* thread_args;
    Timer tmr;
    long long calc_time;
    double pi;

    err = 0;
    thread_cnt = *(int*)arg;
    pi = 0;

    threads = (HANDLE*)malloc(sizeof(HANDLE) * thread_cnt);
    events = (HANDLE*)malloc(sizeof(HANDLE) * thread_cnt);
    thread_args = (ThreadArg*)malloc(sizeof(ThreadArg) * thread_cnt);
    if (threads == NULL || events == NULL || thread_args == NULL) {
        err = E_ALLOC;
        goto out_free;
    }

    for (i = 0; i < thread_cnt; i++) {
        events[i] = CreateEvent(
                        /*lrEventAttributes*/ NULL,
                        /*bManualReset*/ FALSE,
                        /*bInitialState*/ TRUE,
                        /*lpName*/ NULL
                    );
        if (events[i] == INVALID_HANDLE_VALUE) {
            err = E_WINDOWS_ERROR;
            thread_cnt = i;
            goto out_hs;
        }

        thread_args[i].res = 0;
        thread_args[i].event = events[i];
        thread_args[i].exit = FALSE;

        threads[i] = CreateThread(
                        /*lpThreadAttributes*/ NULL,
                        /*dwStackSize*/ 0,
                        thread_work,
                        (void*)&thread_args[i],
                        CREATE_SUSPENDED,
                        /*lpThreadId*/ NULL
                    );
        if (threads[i] == INVALID_HANDLE_VALUE) {
            err = E_WINDOWS_ERROR;
            thread_cnt = i;
            goto out_hs;
        }
        if (!SetThreadPriority(threads[i], THREAD_PRIORITY_TIME_CRITICAL)) {
            err = E_WINDOWS_ERROR;
            thread_cnt = i;
            goto out_hs;
        }
    }
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        err = E_WINDOWS_ERROR;
        goto out_hs;
    }

    timer_start(&tmr);

    // NOTE: Пропускаем проверки на ошибки внутри критического кода.
    //

    block_beg = 0;
    while (block_beg < ITERATION_COUNT) {
        i = WaitForMultipleObjects(thread_cnt, events,
                                   /*bWaitAll*/ FALSE,
                                   /*dwMilliseconds*/ INFINITE)
            - WAIT_OBJECT_0;
        thread_args[i].block_beg = block_beg;
        block_beg += BLOCK_SIZE;
        thread_args[i].block_end = MIN(block_beg, ITERATION_COUNT);

        // Иногда оказывается, что управление в главный поток
        // возвращается раньше, чем вызывается SuspendThread().
        //
        // То есть получается, что-то вроде:
        //
        // Time    Worker thread      Main thread
        // -----   ---------------    -----------
        // 1       SetEvent()         WaitForMultipleObjects()
        // 2                          ResumeThread() => returns 0
        // 3       SuspendThread()
        //
        // Таким образом, после ResumeThread() поток на самом деле
        // не просыпается. Решается ожиданием SuspendThread() перед
        // ResumeThread().
        //
        while (ResumeThread(threads[i]) == 0)
            ;
    }

    WaitForMultipleObjects(thread_cnt, events, /*bWaitAll*/ TRUE,
                           /*dwMilliseconds*/ INFINITE);

    for (i = 0; i < thread_cnt; i++) {
        pi += thread_args[i].res;
    }
    pi /= ITERATION_COUNT;

    calc_time = timer_stop_us(&tmr);

    printf("PI = %.*lf\n", ACCURANCY_DIG, pi);
    printf("Time = %"PRId64" us\n", calc_time);

    for (i = 0; i < thread_cnt; i++) {
        thread_args[i].exit = TRUE;
        ResumeThread(threads[i]);
    }
    WaitForMultipleObjects(thread_cnt, threads, /*bWaitAll*/ TRUE,
                           /*dwMilliseconds*/ INFINITE);

out_hs:
    for (i = 0; i < thread_cnt; i++) {
        CloseHandle(events[i]);
        CloseHandle(threads[i]);
    }
out_free:
    free(threads);
    free(events);
    free(thread_args);
    return err;
}

DWORD thread_work(void *arg) {
    int i, beg, end;
    double tmp_res;
    ThreadArg *ta;

    ta = (ThreadArg*)arg;
    while (!ta->exit) {
        // Если внести ta->block_{beg,end} в цикл, то
        // появляется заметное падение производительности
        // (конкретно, компилятор не может произвести loop
        // unrolling, т.к. ожидает изменения ta на каждой
        // итерации).
        //
        // Также, и tmp_res нужен для того, чтобы большее
        // количество переменных считалось компилятором
        // неизменными снаружи ("чистыми").
        //
        beg = ta->block_beg;
        end = ta->block_end;
        tmp_res = 0;
        for (i = beg; i < end; i++) {
            tmp_res += calc_pi_iteration(i, ITERATION_COUNT);
        }
        ta->res += tmp_res;

        SetEvent(ta->event);
        SuspendThread(GetCurrentThread());
    }
    return 0;
}

//
// Task 2
//

int calc_pi_with_omp(void *arg) {
    int i;
    double pi;
    Timer tmr;
    long long calc_time;

    omp_set_num_threads(*(int*)arg);

    timer_start(&tmr);

    pi = 0;
    #pragma omp parallel for \
        private(i) \
        schedule(dynamic, BLOCK_SIZE) \
        reduction(+:pi)
    for (i = 0; i < ITERATION_COUNT; i++) {
        pi += calc_pi_iteration(i, ITERATION_COUNT);
    }

    pi /= ITERATION_COUNT;

    calc_time = timer_stop_us(&tmr);

    printf("PI = %.*lf\n", ACCURANCY_DIG, pi);
    printf("Time = %"PRId64" us\n", calc_time);

    return 0;
}
