//EXERCISE 2, TASK 2: SEMAPHORES

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

//Task C
sem_t Semaphore; //Identify Semaphore
long int increment_var = 0;

//Incrementing local and global variables using two unsynchronized threads
void *threads(void *args){
    //Store id for thread using function
    int *threadId = (int*) args;
    //Local variable incrementing
    int local_increment_var = 0;
    
    //Incrementing variables in for-loop
    for (int i = 0;i<50000000;i++){
        
        sem_wait(&Semaphore); //Decrement semaphore, wait for resource
        //Increment global variable
        ++increment_var;
        sem_post(&Semaphore); //
        
        //Increment local variable
        ++local_increment_var;
    }
    printf("Thread id: %s, Local var: %d, Global var: %ld \n", (char*)threadId,++local_increment_var,++increment_var);
    return 0;
}

int main(){
    sem_init(&Semaphore,0,1); //Initialize Semaphore
    
    pthread_t threadId1;
    pthread_t threadId2; //Identify thread
    pthread_create(&threadId1, NULL, threads, "Thread 1");  //Returns the threadHandle id and creates with startroutine
    pthread_create(&threadId2, NULL, threads, "Thread 2");  //Returns the threadHandle id and creates with startroutine  
    pthread_join(threadId1, NULL);
    pthread_join(threadId2, NULL); //Wait for threads to end before completing
}
