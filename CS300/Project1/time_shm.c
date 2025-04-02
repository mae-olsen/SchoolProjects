#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 

//Resources:
//Zybooks Section 3.7.1, Lecture 5 Process API Slides

//creating and writing to the shared memory location
void share(long double start){
    const int size = 4096;  //set size of shared memory 
    int fd;     //memory file descriptor
    void *ptr;  //pointer to shared memory object

    fd = shm_open("shared", O_CREAT | O_RDWR, 0666);    //create shared memory object
    ftruncate(fd, size);    //configure size of object

    //map shared memory in process address space
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        return;
    }

    sprintf(ptr, "%.05Lf", start);  //write time to shared memory

    return;
}

//reading from shared memory
void* accessMem(char *name){
    const int size = 4096; //set size of shared memory
    int fd; //memory file descriptor
    void *ptr;  //pointer to shared memory object
    void *share;  //string to save and return start time

    fd = shm_open(name, O_RDONLY, 0666);    //open existing memory object
    if(fd == -1){   //memory object was not created properly
        printf("shared memory failed\n");
        exit(-1);
    }

    //access memory address of stored information
    ptr = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed (access) \n");
        exit(-1);
    }

    share = ptr;
    
    //remove shared memory and check for completion
    if(shm_unlink(name) == -1){ 
        printf("Error removing %s\n", name);
        exit(-1);
    }

    return share;   //return time
}

//main program
int main(int argc, char *argv[]){

    struct timeval start_time;
    struct timeval end_time;
    void* retrieved_time;

    pid_t pid = fork();
    if(pid < 0){    //new process was not created
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if(pid == 0){  //child process
        gettimeofday(&start_time, 0);   //get start time

        long double time_seconds = start_time.tv_sec;  //seconds
        long double time_useconds = (start_time.tv_usec)/1000000.0;    //converting microseconds to seconds
        long double start_time_num = time_seconds + time_useconds;     //sum for total time
        share(start_time_num);  //add time to the shared memory
        exit(execvp((argv[1]), &argv[1]));  //execute the command and exit
    }
    else{   //parent process
       wait(NULL);  //wait until child is finished
       retrieved_time = accessMem("shared");    //access start time from shared memory
       long double retrieved = atof(retrieved_time);    //convert start time to a float
       gettimeofday(&end_time, 0);  //get end time

       long double end_time_seconds = end_time.tv_sec;  //seconds
       long double end_time_useconds = (end_time.tv_usec)/1000000.0;    //converting microseconds to seconds    
       long double end_time_num = end_time_seconds + end_time_useconds; //sum for total time

       //printf("seconds: %.05Lf\n", end_time_seconds+end_time_useconds); //Testing
       //printf("Start time: %.05Lf, End time: %.05Lf\n", retrieved, end_time_num); //Testing
       printf("Elapsed time: %.05Lf\n", (end_time_num - retrieved));    //Printing elapsed time
    }

    return 0;
}