#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

//Resource: Zybooks Section 4.4

int circlePoints = 0;   //number of points that lie in circle

void *compute(void *points){    //function used by threads to calculate number of points in the circle
    double x;   //x coordinate
    double y;   //y coordinate
   
   //function for a circle: x^2+y^2=1
    for(int i = 0; i < atoi(points); i++){  
        x = (double)rand()/(double)RAND_MAX;    //randomly generated x-coordinate
        y = (double)rand()/(double)RAND_MAX;    //randomly generated y-coordinate

        if((fabs(x*x)+fabs(y*y))<=1) circlePoints++;    //point is on perimeter or inside of circle
    }
    pthread_exit(0);    //terminate thread execution and return
}

int main(int argc, char *argv[]){

    int totalPoints = atoi(argv[1]);    //total number of points to be generated
    double pi;
   
    pthread_t tid;   //thread identifier
    pthread_create(&tid, NULL, compute, argv[1]);   //create thread
    pthread_join(tid, NULL);    //terminates child thread

    pi = 4*((double)circlePoints)/(double)totalPoints;  //pi estimation

    printf("%f\n", pi);

    return 0;
}