#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

/* TTK4147 Exercise 1 - Group 4 */

/* Timespec helper functions */
struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}

/* Busy wait using clock_gettime() */
void busy_wait(struct timespec t){
    t.tv_sec = 1;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);

    while(timespec_cmp(now, then) < 0){
        //for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

/* Busy wait using times() */
int busy_wait_times(){
  clock_t start_time;
  long int ticks_per_sec = sysconf(_SC_CLK_TCK);

  struct tms time_passed;
  times(&time_passed);

  start_time = time_passed.tms_utime + time_passed.tms_stime;

  while ((time_passed.tms_utime+time_passed.tms_stime) - start_time < ticks_per_sec) {
    times(&time_passed);
  }
  return 0;
}

/* Busy wait using sleep() */
int wait_sleep () {
  sleep(1);
  return 0;
}



void main (){
  int mode;

  struct timespec t;

  printf("Enter mode (1 = sleep, 2 = busy_wait with clock_gettime, 3 = busy_wait with times()): \n");
  scanf("%i", &mode);

  switch (mode) {
    case 1:
      wait_sleep();

    case 2:
      busy_wait(t);

    case 3:
      busy_wait_times();

  }
}
