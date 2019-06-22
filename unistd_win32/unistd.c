/*******************************************************************************
*  file    : unistd.c
*  created : 05.01.2019
*  author  : Slyshyk Oleksiy (alexslyshyk@gmail.com)
*******************************************************************************/

#include "unistd.h"

#ifdef __cplusplus
extern "C"
#endif
void usleep(unsigned long waitTime)
{
    if (waitTime >= 1000)
    {
        HANDLE timer;
        LARGE_INTEGER dueTime;
        dueTime.QuadPart = -10 * (LONGLONG)waitTime;

        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        SetWaitableTimer(timer, &dueTime, 0, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
        return;
    }
    LARGE_INTEGER perf_cnt, start, now;

    QueryPerformanceFrequency(&perf_cnt);
    QueryPerformanceCounter(&start);

    do {
        QueryPerformanceCounter((LARGE_INTEGER*) &now);
    } while ((now.QuadPart - start.QuadPart) / (float)perf_cnt.QuadPart * 1000 * 1000 < waitTime);
}
