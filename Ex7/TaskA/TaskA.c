#include <native/task.h>
#include <native/sem.h>
#include <rtdk.h>
#include <sys/mman.h>

#define cpuid 1

#define TIME_OUT 100



RT_TASK task1;
RT_TASK task2;
RT_TASK task3;

// Create semaphore
RT_SEM semaphore1;


void thread1(void *args){
	// Wait for semaphore broadcast
	rt_sem_p(&semaphore1, TM_INFINITE); // Decrement 

	rt_printf("Thread 1 acquired semaphore.");

	// rt_sem_v(&semaphore1); // Increment

}


void thread2(void *args){

	// Wait for semaphore broadcast
	rt_sem_p(&semaphore1, TM_INFINITE); // Decrement 

	rt_printf("Thread 2 acquired semaphore.\n");

	// rt_sem_v(&semaphore1); // Increment


}

void thread3(void *args){

	// Create sempahore
	rt_sem_create(&semaphore1, "Semaphore 1", 0, S_PRIO);

	// Creates task
	rt_task_create(&task1, "Test1", 0, 1, T_CPU(cpuid));
	rt_task_create(&task2, "Test2", 0, 2, T_CPU(cpuid));

	// Start 2 tasks
	rt_task_start(&task1, &thread1, NULL);
	rt_task_start(&task2, &thread2, NULL);

	// Sleep
	rt_task_sleep(100);

	// Broadcast semaphore
	rt_sem_broadcast(&semaphore1); // Send signals to all tasks that are waiting for a semaphore - they will start at the same time (synchronized)
								   // Starting in order that they arrived. 
	//rt_sem_v(&semaphore1);
	// Sleep
	rt_task_sleep(100);

	// Join tasks
	rt_task_join(&task1);
	rt_task_join(&task2);

	// Delete semaphore
	rt_sem_delete(&semaphore1); // Call at end of lifetime of semaphore, deallocating memory

}


int main() {
	rt_print_auto_init(1);

	// Avoid real-time task to be swapped to disk
	mlockall(MCL_CURRENT | MCL_FUTURE);


	// Create task
	rt_task_create(&task3, "Test3", 0, 3, T_CPU(cpuid));
	
	// Start task
	rt_task_start(&task3, &thread3, NULL);
	

	// Wait for task to complete
	rt_task_join(&task3);

}
