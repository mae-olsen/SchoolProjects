#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdbool.h>

//Resources:
//Lecture 12 Slides, ChatGPT for CAS __atomic_compare_exchange_n() usage

volatile long long counter_mutex = 0; // Counter for Mutex Worker
volatile long long counter_semaphore = 0; // Counter for Binary Semaphore Worker
volatile long long counter_cas = 0; // Counter for Compare-and-Swap Worker
volatile long long counter_tas = 0; // Counter for Test-and-Set Worker
volatile long long counter_dum = 0; // Counter without sync

volatile int tas_lock = 0; // 0: unlocked, 1: locked

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t semaphore;

// 1. Mutex worker, finish the code to use pthread mutex to synchronize threads
void* mutexWorker(void* arg) {      
    long maxcount = *(long*)arg;
    for (long i = 0; i < maxcount; i++) {
        pthread_mutex_lock(&mutex);      //acquire lock
        counter_mutex++;    //critical section
        pthread_mutex_unlock(&mutex);    //release lock after incrementing
    }   
    return NULL;
}

// 2. Semaphore Worker, finish the code to use semaphore to synchronize threads
void* semaphoreWorker(void* arg) {
    long maxcount = *(long*)arg;
    for (long i = 0; i < maxcount; i++) {
        sem_wait(&semaphore);   //acquire lock
        counter_semaphore++;    //critical section
        sem_post(&semaphore);   //release lock after incrementing
    }
    return NULL;
}

// 3. CAS worker, finish the code to use CAS for atomic_increment
void atomic_increment(long long* value) {
    long long expected, desired;

    do{     //keep trying to increment if failed
        expected = *value;
        desired = expected + 1;

    } while(!__atomic_compare_exchange_n(value, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));    //try to update value
}

void* CASWorker(void* arg) {    
    long maxcount = *(long*)arg;
    for (long i = 0; i < maxcount; i++) {
        atomic_increment(&counter_cas); //increment counter_cas atomically without lock
    }
    return NULL;
}

void tas_lock_acquire(volatile int* lock) {    
    while(__sync_lock_test_and_set(lock, 1));   //spin-wait
}

void tas_lock_release(volatile int* lock) {
     __sync_lock_release(lock); //release lock
}
// 4. Test-and-Set Worker, finish the code with TAS lock 
void* testAndSetWorker(void* arg) {
    long maxcount = *(long*)arg;
    for (long i = 0; i < maxcount; i++) {
        tas_lock_acquire(&tas_lock);    //acquire lock
        counter_tas++;
        tas_lock_release(&tas_lock);    //release lock
    }
    return NULL;
}

// 5. Dum worker
void *DumWorker(void *arg) {
    long maxcount = *(long*)arg;
    for (int i = 0; i < maxcount; i++) {
	    counter_dum++;
    }
    return NULL;
}


int main(int argc, char *argv[]) { 
    if (argc != 3) {
	    fprintf(stderr, "usage: %s <NumThread> <MaxCount> \n", argv[0]);
	    exit(1);
    }
    int num_threads = atoi(argv[1]);
    long maxcount = atol(argv[2]);
    
    long long elapsed;
    pthread_t threads[num_threads];
    
    struct timeval start_time;
    struct timeval end_time;

    // Dum increment   

    // Create threads
    gettimeofday(&start_time, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, DumWorker, &maxcount);
    }
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // measure the execution time and print final counter value
    gettimeofday(&end_time, 0);
    elapsed = 0;
    elapsed = (end_time.tv_usec - start_time.tv_usec);
    elapsed += (end_time.tv_sec - start_time.tv_sec)*1000000;   //elapsed time in usec
   
    printf("Dum Time [%lld], final counter value: %lld \n", elapsed, counter_dum);


   // CAS increment

   // Create threads
   gettimeofday(&start_time, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, CASWorker, &maxcount);
    }
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, 0);
    elapsed = 0;
    elapsed = (end_time.tv_usec - start_time.tv_usec);
    elapsed += (end_time.tv_sec - start_time.tv_sec)*1000000;   //elapsed time in usec
    
    printf("CAS Time [%lld], final counter value: %lld \n", elapsed, counter_cas);

    // TAS increment   

   // Create threads
    gettimeofday(&start_time, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, testAndSetWorker, &maxcount);
    }
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, 0);
    elapsed = 0;
    elapsed = (end_time.tv_usec - start_time.tv_usec);
    elapsed += (end_time.tv_sec - start_time.tv_sec)*1000000;   //elapsed time in usec
    
    printf("TAS Time [%lld], final counter value: %lld \n", elapsed, counter_tas);


    // Mutex increment   

   // Create threads
    gettimeofday(&start_time, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, mutexWorker, &maxcount);
    }
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, 0);
    elapsed = 0;
    elapsed = (end_time.tv_usec - start_time.tv_usec);
    elapsed += (end_time.tv_sec - start_time.tv_sec)*1000000;   //elapsed time in usec
    
    printf("MUT Time [%lld], final counter value: %lld \n", elapsed, counter_mutex);

    // Semaphore increment   
    sem_init(&semaphore, 0, 1);   //initialize semaphore

   // Create threads
    gettimeofday(&start_time, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, semaphoreWorker, &maxcount);
    }
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, 0);
    elapsed = 0;
    elapsed = (end_time.tv_usec - start_time.tv_usec);
    elapsed += (end_time.tv_sec - start_time.tv_sec)*1000000;   //elapsed time in usec
    
    printf("SEM Time [%lld], final counter value: %lld \n", elapsed, counter_semaphore);
    
    // Destroy mutex and semaphore
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
    return 0;
}