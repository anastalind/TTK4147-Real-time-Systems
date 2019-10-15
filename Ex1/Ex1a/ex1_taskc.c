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



void plot_resolution() {
  int ns_max = 300;
  int histogram[ns_max];

  long int ticks_per_nsec = 3.3;

  struct tms time_passed;

  struct timespec now;

  clock_t t1, t2;

  memset(histogram, 0, sizeof(int)*ns_max);

  for(int i = 0; i < 10*1000*1000; i++){

      clock_gettime(CLOCK_MONOTONIC, &now);
      t1 = now.tv_sec + now.tv_nsec;

      sched_yield();

      clock_gettime(CLOCK_MONOTONIC, &now);
      t2 = now.tv_sec + now.tv_nsec;

      int ns = (t2-t1);

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
