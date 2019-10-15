#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "io.h"

pthread_mutex_t taskA, taskB, taskC; //Identify mutex 


void* task_A_test(void *args) {
    int *which_task = (int*) args; // 
    while (1){
        if (io_read(1) == 0) {
            io_write(1,0);
            usleep(5);
            io_write(1,1);
        }
    }
    //printf("Thread id: %s \n", (char*)which_task);

    return NULL;
}

void* task_B_test(void *args) {
    int *which_task = (int*) args; // 
    while (1){
        if (io_read(2) == 0) {
            io_write(2,0);
            usleep(5);
            io_write(2,1);
        }
    }
    printf("Thread id: %s \n", (char*)which_task);
    
    return NULL;
}

void* task_C_test(void *args) {
    int *which_task = (int*) args; // 
    while (1){
        if (io_read(3) == 0) {
            io_write(3,0);
            usleep(5);
            io_write(3,1);
        }
    }
    printf("Thread id: %s \n", (char*)which_task);

    return NULL;
}


int main(){
    // Initialize input/ouput on lab-computer
    io_init();

    while(1)
    {
        io_write(1,0);
        sleep(1);
        io_write(1,1);
        sleep(1);
    }

    //Create threads (philosophers)
    pthread_t taskA, taskB, taskC;
    
    pthread_create(&taskA, NULL, task_A_test, "Task A");  //Returns the thread id and creates with startroutine
    pthread_create(&taskB, NULL, task_B_test, "Task B");   
    pthread_create(&taskC, NULL, task_C_test, "Task C");  
    
    pthread_join(taskA, NULL); //Wait for threads to end before completing
    pthread_join(taskB, NULL); 
    pthread_join(taskC, NULL);

}