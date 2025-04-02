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
//Zybooks Section 3.7.6, Lecture 5 Process API Slides

int READ_END = 0;   //read index in fd pipe array
int WRITE_END = 1;    //write index in fd pipe array

int main(int argc, char *argv[]){
    
    struct timeval start_time;
    struct timeval end_time;

    int fd[2];  
    if(pipe(fd) == -1){ //check if pipe opened successfully
        fprintf(stderr, "Pipe failed");
        return 1;
    }

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

        char message[32];   
        snprintf(message, sizeof(message), "%.05Lf", start_time_num);   //convert start_time_num to string for pipe

        close(fd[READ_END]);    //close unused end of pipe
        write(fd[WRITE_END], message, strlen(message)+1);   //write start time to pipe
        close(fd[WRITE_END]);   //close write end after use

        exit(execvp((argv[1]), &argv[1]));  //execute the command and exit
    }
    else{   //parent process
        wait(NULL); //wait until child is finished

        char retrieved_time[32];
        close(fd[WRITE_END]);   //close unused end of pipe
        read(fd[READ_END], retrieved_time, 32); //read in start time
        long double retrieved = atof(retrieved_time);   //convert start time from string to float

        gettimeofday(&end_time, 0);  //get end time

        long double end_time_seconds = end_time.tv_sec;  //seconds
        long double end_time_useconds = (end_time.tv_usec)/1000000.0;    //converting microseconds to seconds    
        long double end_time_num = end_time_seconds + end_time_useconds; //sum for total time

        printf("Elapsed time: %.05Lf\n", (end_time_num - retrieved));    //Printing elapsed time

        close(fd[READ_END]);    //close read end after use
    }

    return 0;
}