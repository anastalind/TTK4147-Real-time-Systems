#include <rtdk.h>
#include <native/task.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <native/timer.h>

#include "io.h"

#define TASK_PERIOD 1000*1000
#define cpuid 1
#define TASK_DELAY 5

RT_TASK task1;
RT_TASK task2;
RT_TASK task3;

// Set single CPU for pthread threads (NOT Xenomai threads!)
int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}


void thread1(void *args){
	// Busy-polling that won't crash your computer (hopefully)
	unsigned long duration = 40000000000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;	

	RT_TASK* curtask;
	curtask = rt_task_self();

	// Make task periodic
	rt_task_set_periodic(curtask, TM_NOW, TASK_PERIOD);

    while (1){
        if (io_read(1) == 0) {
            io_write(1,0);
            //rt_timer_spin(TASK_DELAY);
            io_write(1,1);
        }
            // do work
    
	    if(rt_timer_read() > endTime){
	        rt_printf("Time expired\n");
	        rt_task_delete(NULL);
	    }
	    if(rt_task_yield()){
	        rt_printf("Task failed to yield\n");
	        rt_task_delete(NULL);
	    }
	    rt_task_wait_period(NULL);
    }
}

void thread2(void *args){
	// Busy-polling that won't crash your computer (hopefully)
	unsigned long duration = 40000000000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;	

	RT_TASK* curtask;
	curtask = rt_task_self();

	// Make task periodic
	rt_task_set_periodic(curtask, TM_NOW, TASK_PERIOD);

    while (1){
        if (io_read(2) == 0) {
            io_write(2,0);
            //rt_timer_spin(TASK_DELAY);
            io_write(2,1);
        }
            // do work
    
	    if(rt_timer_read() > endTime){
	        rt_printf("Time expired\n");
	        rt_task_delete(NULL);
	    }
	    if(rt_task_yield()){
	        rt_printf("Task failed to yield\n");
	        rt_task_delete(NULL);
	    }
	    rt_task_wait_period(NULL);
    }
}

void thread3(void *args){
	// Busy-polling that won't crash your computer (hopefully)
	unsigned long duration = 40000000000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;	

	RT_TASK* curtask;
	curtask = rt_task_self();

	// Make task periodic
	rt_task_set_periodic(curtask, TM_NOW, TASK_PERIOD);

    while (1){
        if (io_read(3) == 0) {
            io_write(3,0);
            //Busy wait
            //rt_timer_spin(TASK_DELAY);
            io_write(3,1);
        }
            // do work
    
	    if(rt_timer_read() > endTime){
	        rt_printf("Time expired\n");
	        rt_task_delete(NULL);
	    }
	    if(rt_task_yield()){
	        rt_printf("Task failed to yield\n");
	        rt_task_delete(NULL);
	    }
	    rt_task_wait_period(NULL);
    }
}

void* disturbance(void *args) {
    set_cpu(1);

    while (1){
       asm volatile("" ::: "memory");
        }
    return NULL;
}

int main() {
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

    // Initialize input/ouput on lab-computer
    io_init();

    // Disturbance pthreads 
    for (int i=0; i<10; i++){
        pthread_t i;
        pthread_create(i, NULL, disturbance, "Task "+i);
    }


	// Creates task
	rt_task_create(&task1, "Test1", 0, 99, T_CPU(cpuid));
	rt_task_create(&task2, "Test2", 0, 99, T_CPU(cpuid));
	rt_task_create(&task3, "Test3", 0, 99, T_CPU(cpuid));
	// Start task
	rt_task_start(&task1, &thread1, NULL);
	rt_task_start(&task2, &thread2, NULL);
	rt_task_start(&task3, &thread3, NULL);

	pause();

	// Join tasks
	rt_task_join(&task1);
	rt_task_join(&task2);
	rt_task_join(&task3);



    for (int i=0; i<10; i++){
        pthread_join(i,NULL);
    }


	return 0;


}