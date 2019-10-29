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

typedef struct {
	int priority;
	RT_MUTEX mutex;
} resource;

typedef struct {
	int base_priority;
	int current_priority;
} task_prio;

resource A;
resource B; 



void icpp_lock(RT_TASK* task, resource r, task_prio p) {

	// Set the tasks current priority to the resource's priority
	p.current_priority = r.priority;
	rt_task_set_priority(task, p.current_priority);

	// Lock resource mutex
	rt_mutex_acquire(&r.mutex, TM_INFINITE);
}

void icpp_unlock(RT_TASK* task, resource r, task_prio p) {

	rt_mutex_release(&r.mutex);

	p.current_priority = p.base_priority;
	rt_task_set_priority(task, p.current_priority);
}


// Busy wait function
void busy_wait_us(unsigned long delay) {
	for(; delay > 0 ; delay--) {
		rt_timer_spin(1000);
	}
}


void low(void *args){
	struct rt_task_info temp;

	task_prio low_prio;
	low_prio.base_priority = 1;
	low_prio.current_priority = low_prio.base_priority;

	RT_TASK* curtask;
	curtask = rt_task_self();

	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Lock resource A
	icpp_lock(curtask, A, low_prio);
	rt_printf("Low gets A.\n");

	rt_task_inquire(NULL, &temp);
	rt_printf("Current priority low (got A): %i\n", temp.cprio);


	// Busy wait 3 time units
	busy_wait_us(3*TIME_UNIT);

	// Lock resource B
	icpp_lock(curtask, B, low_prio);
	rt_printf("Low gets B.\n");
	rt_task_inquire(NULL, &temp);
	rt_printf("Current priority low (got B): %i\n", temp.cprio);

	// unlock resource B
	icpp_unlock(curtask, B, low_prio); // Increment
	rt_printf("Low releases B.\n");

	// unlock resource A
	icpp_unlock(curtask, A, low_prio);
	rt_printf("Low releases A.\n");

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	rt_printf("No deadlock low task.\n");
	// rt_task_inquire(NULL, &temp);
}


void high(void *args){
	struct rt_task_info temp;

	task_prio high_prio;
	high_prio.base_priority = 2;
	high_prio.current_priority = high_prio.base_priority;

	RT_TASK* curtask;
	curtask = rt_task_self();
	
	// Wait for semaphore broadcast
	rt_sem_p(&semaphoreBarrier, TM_INFINITE); // Decrement 

	// Sleep for 1 time unit
	rt_task_sleep(1*TIME_UNIT*1000);

	// Lock resource B
	icpp_lock(curtask, B, high_prio);
	rt_printf("High gets B.\n");

	rt_task_inquire(NULL, &temp);
	rt_printf("Current priority high (got B): %i\n", temp.cprio);

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	// Lock resource A
	icpp_lock(curtask, A, high_prio);
	rt_printf("High gets A.\n");

	rt_task_inquire(NULL, &temp);
	rt_printf("Current priority high (got A): %i\n", temp.cprio);

	// Busy wait 2 time units
	busy_wait_us(2*TIME_UNIT);	

	// unlock resource A
	icpp_unlock(curtask, A, high_prio); // Increment
	rt_printf("High releases A.\n");

	// unlock resource B
	icpp_unlock(curtask, B, high_prio); // Increment
	rt_printf("High releases B.\n");

	// Busy wait 1 time unit
	busy_wait_us(1*TIME_UNIT);

	rt_printf("No deadlock high task.\n");

}

void barrier(void *args){
	// Create semaphore
	rt_sem_create(&semaphoreBarrier, "Semaphore Barrier", 0, S_PRIO);
	rt_mutex_create(&mutexA, "Mutex A");
	rt_mutex_create(&mutexB, "Mutex B");

	A.mutex = mutexA;
	A.priority = 5;

	B.mutex = mutexB;
	B.priority = 10;

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
