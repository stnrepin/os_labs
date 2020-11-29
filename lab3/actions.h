#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <wchar.h>
#include <Windows.h>

static_assert(sizeof(DWORD) == sizeof(long), "Platform should define DWORD as long");

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAX_STR 256

#define ITERATION_COUNT 100'000'000
#define BLOCK_SIZE (10*830716)
#define ACCURANCY_DIG 15

typedef struct {
    double res;
    int block_beg;
    int block_end;
    HANDLE *event;
    BOOL exit;
} ThreadArg;

inline double calc_pi_iteration(int it, int it_count) {
    double x;
    x = (it + 0.5) / it_count;
    return 4.0 / (1.0 + x*x);
}

int calc_pi_with_winapi(void *arg);
DWORD WINAPI thread_work(void *arg);

int calc_pi_with_omp(void *arg);

typedef struct {
    LONGLONG start;
    LONGLONG freq;
} Timer;

inline void timer_start(Timer* tmr) {
    LARGE_INTEGER freq, start;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    tmr->freq = freq.QuadPart;
    tmr->start = start.QuadPart;
}

inline long long timer_stop_us(Timer* tmr) {
    const int kMicrosecondsPerSecond = 1'000'000;
    LARGE_INTEGER finish;

    QueryPerformanceCounter(&finish);

    return (finish.QuadPart - tmr->start) *
           kMicrosecondsPerSecond / tmr->freq;
}

#endif // !ACTIONS_H_
