#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <sched.h>

#include "io.h"


/// Assigning CPU core ///

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

/// 'struct timespec' functions ///

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

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}


pthread_mutex_t taskA, taskB, taskC; //Identify mutex 


void* task_A_test(void *args) {

    set_cpu(1);

    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec = 0, .tv_nsec = 1};

    int *which_task = (int*) args; // 
    while (1){
        if (io_read(1) == 0) {
            io_write(1,0);
            usleep(5);
            io_write(1,1);
        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
    //printf("Thread id: %s \n", (char*)which_task);

    return NULL;
}

void* task_B_test(void *args) {

    set_cpu(1);

    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec = 0, .tv_nsec = 1};

    int *which_task = (int*) args; // 
    while (1){
        if (io_read(2) == 0) {
            io_write(2,0);
            usleep(5);
            io_write(2,1);
        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
    //printf("Thread id: %s \n", (char*)which_task);
    
    return NULL;
}

void* task_C_test(void *args) {

    set_cpu(1);

    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec = 0, .tv_nsec = 1};

    int *which_task = (int*) args; // 
    while (1){
        if (io_read(3) == 0) {
            io_write(3,0);
            usleep(5);
            io_write(3,1);
        }

        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
    //printf("Thread id: %s \n", (char*)which_task);

    return NULL;
}

void* disturbance(void *args) {

    set_cpu(1);

    int *which_task = (int*) args;
    while (1){
        asm volatile("" ::: "memory");

    }
    return NULL;
}


int main(){
    // Initialize input/ouput on lab-computer
    io_init();

    
    for (int i=0; i<10; i++){
        pthread_t i;
        pthread_create(&i, NULL, disturbance, "Task "+i);
    }
    
    

    //Create threads (philosophers)
    pthread_t taskA, taskB, taskC;
    
    pthread_create(&taskA, NULL, task_A_test, "Task A");  //Returns the thread id and creates with startroutine
    pthread_create(&taskB, NULL, task_B_test, "Task B");   
    pthread_create(&taskC, NULL, task_C_test, "Task C");  
    
    pthread_join(taskA, NULL); //Wait for threads to end before completing
    pthread_join(taskB, NULL); 
    pthread_join(taskC, NULL);

    
    for (int i=0; i<10; i++){
        pthread_join(i, NULL);
    }

    
}


