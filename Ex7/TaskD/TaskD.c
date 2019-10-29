#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
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
RT_TASK highTask;
RT_TASK barrierTask;

// Create semaphore
RT_SEM semaphoreBarrier;
RT_MUTEX mutexA;
RT_MUTEX mutexB;

// Busy wait function
void busy_wait_us(unsigned long delay) {
	for(; delay > 0 ; delay--) {
		rt_timer_spin(1000);
	}
}


void low(void *args){
	struct rt_task_info temp;

	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Lock resource A
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_printf("Low gets A.\n");

	// Busy wait 3 time units
	busy_wait_us(3*TIME_UNIT);

	// Lock resource B
	rt_mutex_acquire(&mutexB, TM_INFINITE);

	// unlock resource B
	rt_mutex_release(&mutexB); // Increment

	// unlock resource A
	rt_mutex_release(&mutexA); // Increment

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	rt_printf("No deadlock low task.\n");
	// rt_task_inquire(NULL, &temp);
}


void high(void *args){
	struct rt_task_info temp;
	
	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Sleep for 1 time unit
	rt_task_sleep(1*TIME_UNIT*1000);

	// Lock resource B
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_printf("High gets B.\n");

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	// Lock resource A
	rt_mutex_acquire(&mutexA, TM_INFINITE);

	// Busy wait 2 time units
	busy_wait_us(2*TIME_UNIT);	

	// unlock resource A
	rt_mutex_release(&mutexA); // Increment

	// unlock resource B
	rt_mutex_release(&mutexB); // Increment

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	rt_printf("No deadlock high task.\n");

}

void barrier(void *args){
	// Create semaphore
	rt_sem_create(&semaphoreBarrier, "Semaphore Barrier", 0, S_PRIO);
	rt_mutex_create(&mutexA, "Mutex A");
	rt_mutex_create(&mutexB, "Mutex B");

	// Creates task
	rt_task_create(&lowTask, "Test1", 0, 1, T_CPU(cpuid) | T_JOINABLE);
	rt_task_create(&highTask, "Test3", 0, 2, T_CPU(cpuid) | T_JOINABLE);

	// Start 2 tasks
	rt_task_start(&lowTask, &low, NULL);
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
	rt_task_join(&highTask);

	while(1){}

	// Delete semaphore
	rt_sem_delete(&semaphoreBarrier); // Call at end of lifetime of semaphore, deallocating memory
	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);

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
