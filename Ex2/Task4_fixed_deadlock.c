//EXERCISE 2, TASK 4: THE DINING PHILOSOPHERS

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t fork1,fork2,fork3,fork4,fork5,waiter; //Identify mutex 


void* dining1 (void *args) {
    int *which_philosopher = (int*) args; //
    while(1){
    pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&fork1);
            pthread_mutex_lock(&fork2);    
                usleep(1000);
                printf("Thread id: %s \n", (char*)which_philosopher);
            pthread_mutex_unlock(&fork2);
        pthread_mutex_unlock(&fork1);
    pthread_mutex_unlock(&waiter);
    }
    printf("Thread id: %s \n", (char*)which_philosopher);
    return NULL;
}


void* dining2 (void *args) {
    int *which_philosopher = (int*) args; //
    while(1){
    pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&fork2);
            pthread_mutex_lock(&fork3);    
                usleep(1000);
                printf("Thread id: %s \n", (char*)which_philosopher);
            pthread_mutex_unlock(&fork3);
        pthread_mutex_unlock(&fork2);
    pthread_mutex_unlock(&waiter);
    }
    printf("Thread id: %s \n", (char*)which_philosopher);
    return NULL;
}


void* dining3 (void *args) {
    int *which_philosopher = (int*) args; //
    while(1){
    pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&fork3);
            pthread_mutex_lock(&fork4);    
                usleep(1000);
                printf("Thread id: %s \n", (char*)which_philosopher);
            pthread_mutex_unlock(&fork4);
        pthread_mutex_unlock(&fork3);
    pthread_mutex_unlock(&waiter);
    } 
    printf("Thread id: %s \n", (char*)which_philosopher);
    return NULL;
}


void* dining4 (void *args) {
    int *which_philosopher = (int*) args; //
    while(1){
    pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&fork4);
            pthread_mutex_lock(&fork5);    
                usleep(1000);
                printf("Thread id: %s \n", (char*)which_philosopher);
            pthread_mutex_unlock(&fork5);
        pthread_mutex_unlock(&fork4);
    pthread_mutex_unlock(&waiter);
    }
    printf("Thread id: %s \n", (char*)which_philosopher);
    return NULL;
}


void* dining5 (void *args) {
    int *which_philosopher = (int*) args; //
    while(1){
    pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&fork5);
            pthread_mutex_lock(&fork1);    
                usleep(1000);
                printf("Thread id: %s \n", (char*)which_philosopher);
            pthread_mutex_unlock(&fork1);
        pthread_mutex_unlock(&fork5);
    pthread_mutex_unlock(&waiter);
    }
    printf("Thread id: %s \n", (char*)which_philosopher);
    return NULL;
}


int main(){
    //Create mutexes (forks and waiter)
    pthread_mutex_init(&waiter, NULL);
    // 2nd arg is a pthread_mutexattr_t
    pthread_mutex_init(&fork1, NULL);
    pthread_mutex_init(&fork2, NULL);
    pthread_mutex_init(&fork3, NULL);
    pthread_mutex_init(&fork4, NULL);
    pthread_mutex_init(&fork5, NULL);
    
    //Create threads (philosophers)
    pthread_t phil1, phil2, phil3, phil4, phil5;
    
    pthread_create(&phil1, NULL, dining1, "Philosopher 1");  //Returns the thread id and creates with startroutine
    pthread_create(&phil2, NULL, dining2, "Philosopher 2");   
    pthread_create(&phil3, NULL, dining3, "Philosopher 3");  
    pthread_create(&phil4, NULL, dining4, "Philosopher 4");  
    pthread_create(&phil5, NULL, dining5, "Philosopher 5");  
    
    pthread_join(phil1, NULL); //Wait for threads to end before completing
    pthread_join(phil2, NULL); 
    pthread_join(phil3, NULL);
    pthread_join(phil4, NULL);
    pthread_join(phil5, NULL);

    pthread_mutex_destroy(&fork1);
    pthread_mutex_destroy(&fork2);
    pthread_mutex_destroy(&fork3);
    pthread_mutex_destroy(&fork4);
    pthread_mutex_destroy(&fork5);
    pthread_mutex_destroy(&waiter);
}
