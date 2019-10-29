#include <native/task.h>
#include <native/sem.h>
#include <native/timer.h>
#include <rtdk.h>
#include <sys/mman.h>

#include <time.h>
#include <sched.h>
#include <unistd.h>


#define cpuid 1

#define TIME_OUT 100
#define TIME_UNIT 100
#define SLEEP_PERIOD 100 * 1000 * 1000


RT_TASK lowTask;
RT_TASK medTask;
RT_TASK highTask;
RT_TASK barrierTask;

// Create semaphore
RT_SEM semaphoreBarrier;
RT_SEM semaphoreResource;

// Busy wait function
void busy_wait_us(unsigned long delay) {
	for(; delay > 0 ; delay--) {
		rt_timer_spin(1000);
	}
}


void low(void *args){
	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Lock resource
	rt_sem_p(&semaphoreResource, TM_INFINITE);
	rt_printf("low starts busy wait work...\n");

	// Busy wait 3 time units
	busy_wait_us(3*TIME_UNIT);

	rt_printf("low stops busy wait work.\n");
	// unlock resource
	rt_sem_v(&semaphoreResource); // Increment

}


void med(void *args){
	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Sleep for 1 time unit
	rt_task_sleep(1*TIME_UNIT*1000);
	
	rt_printf("med starts busy wait work...\n");
	// Busy wait 5 time units
	busy_wait_us(5*TIME_UNIT);
	rt_printf("med stops busy wait work.\n");
}

void high(void *args){
	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 
	
	// Sleep for 2 time units
	rt_task_sleep(2*TIME_UNIT*1000);

	// Lock
	rt_sem_p(&semaphoreResource, TM_INFINITE); // Decrement 
	rt_printf("high starts busy wait work...\n");

	// Busy wait 2 time units
	busy_wait_us(2*TIME_UNIT);

	rt_printf("high stops busy wait work.\n");
	// Unlock semaphore
	rt_sem_v(&semaphoreResource); // Increment

}

void barrier(void *args){
	// Create semaphore
	rt_sem_create(&semaphoreBarrier, "Semaphore Barrier", 0, S_PRIO);
	rt_sem_create(&semaphoreResource, "Semaphore Resource", 1, S_PRIO);

	// Creates task
	rt_task_create(&lowTask, "Test1", 0, 1, T_CPU(cpuid) | T_JOINABLE);
	rt_task_create(&medTask, "Test2", 0, 2, T_CPU(cpuid) | T_JOINABLE);
	rt_task_create(&highTask, "Test3", 0, 3, T_CPU(cpuid) | T_JOINABLE);

	// Start 2 tasks
	rt_task_start(&lowTask, &low, NULL);
	rt_task_start(&medTask, &med, NULL);
	rt_task_start(&highTask, &high, NULL);

	// Sleep
	rt_task_sleep(SLEEP_PERIOD);

	// Broadcast semaphore
	rt_sem_broadcast(&semaphoreBarrier); // Send signals to all tasks that are waiting for a semaphore - they will start at the same time (synchronized)
								   		 // Starting in order that they arrived. 

	// Sleep
	rt_task_sleep(SLEEP_PERIOD);

	// Join tasks
	rt_task_join(&lowTask);
	rt_task_join(&medTask);
	rt_task_join(&highTask);

	while(1){}

	// Delete semaphore
	rt_sem_delete(&semaphoreBarrier); // Call at end of lifetime of semaphore, deallocating memory
	rt_sem_delete(&semaphoreResource);

}


int main() {
	rt_print_auto_init(1);

	// Avoid real-time task to be swapped to disk
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// Create task
	rt_task_create(&barrierTask, "Barrier", 0, 0, T_CPU(cpuid));
	
	// Start task
	rt_task_start(&barrierTask, &barrier, NULL);
	
	// Wait for task to complete
	rt_task_join(&barrierTask);

}
