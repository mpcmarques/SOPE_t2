#include "constants.h"

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result){
        if ((stop->tv_nsec - start->tv_nsec) < 0) {
                result->tv_sec = stop->tv_sec - start->tv_sec - 1;
                result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
        } else {
                result->tv_sec = stop->tv_sec - start->tv_sec;
                result->tv_nsec = stop->tv_nsec - start->tv_nsec;
        }

        return;
}

void msleep(int *milliseconds)
{
  int ms_remaining = (*milliseconds) % 1000;
  long usec = ms_remaining * 1000;
  struct timespec ts_sleep;

  ts_sleep.tv_sec = (*milliseconds) / 1000;
  ts_sleep.tv_nsec = 1000*usec;
  nanosleep(&ts_sleep, NULL);
}
