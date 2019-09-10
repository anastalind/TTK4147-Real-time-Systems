#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

#define N 10000000


void measure_access_latency() {

  // Dummy variable to run timers
  uint64_t timer_value;

  uint64_t access_latency;

  // Start and end time of measurement
  clock_t start_time;
  clock_t end_time;

  struct tms time_passed;

  struct timespec now;

  clock_gettime(CLOCK_MONOTONIC, &now);
  start_time = now.tv_sec + now.tv_nsec;

  for (int i = 0; i < N; i++) {
    timer_value = times(&time_passed);
  }

  clock_gettime(CLOCK_MONOTONIC, &now);
  end_time = now.tv_sec + now.tv_nsec;

  access_latency = (end_time-start_time)/N;

  printf("Start time: %lu \n", start_time);
  printf("End time: %lu \n", end_time);
  printf("Avg timer value: %lu nsec\n", access_latency);
}


void plot_resolution() {
  int ns_max = 50;
  int histogram[ns_max];

  long int ticks_per_nsec = 3.3;

  struct tms time_passed;

  struct timespec now;

  clock_t t1, t2;

  memset(histogram, 0, sizeof(int)*ns_max);

  for(int i = 0; i < 10*1000*1000; i++){

    times(&time_passed);
    t1 = time_passed.tms_stime + time_passed.tms_utime;
    times(&time_passed);
    t2 = time_passed.tms_stime + time_passed.tms_utime;

    int ns = (t2 - t1)/ticks_per_nsec;

      if(ns >= 0 && ns < ns_max){
          histogram[ns]++;
      }
  }

  for(int i = 0; i < ns_max; i++){
      printf("%d\n", histogram[i]);
  }
}

void main() {
  plot_resolution();
}
