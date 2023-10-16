#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>

typedef struct{
    int pid;
    char name[100];
    double exec_time;
    double arrival_time;
    double completion_time;
} process;

typedef struct shm_t{
    int queue[1000];
    process * table[1000];
    int indices[100];
    int tot_processes;
    int start;
    int end;
    sem_t mutex;
} shm_t;

void scheduler();
extern shm_t* shm;
extern int ncpu;
extern int tSlice;
int status;
pid_t isTerminated;
int pid;

int hash(int pid){
    int sum= pid%1000;
    if (sum>500){
        return sum-500;
    }
    else{
        return sum;
    }
}

void enqueue(int pid){
    shm->queue[++shm->end]=pid;
}

void scheduler(){
    while (shm->start<shm->end){
        sem_wait(&shm->mutex); //waits for the access of shared memory object

        //starts cycling over the number of processes to run according to ncpu
        for (int i=1; i<=ncpu;i++){ 

            if(i>(shm->end-shm->start)){//if ncpu is greater than number of processes left than it is ignored
                continue;
            }
            kill(shm->queue[shm->start+i],SIGCONT); //continues the execution of the process
        }
        sem_post(&shm->mutex);
        usleep(tSlice*1000); //waits for the time slice
        sem_wait(&shm->mutex);

        for (int i=1; i<=ncpu;i++){

            if(i>(shm->end-shm->start)){ //if the ncpu is greater than number of process left than it is ignored
                continue;
            }
            pid=shm->queue[shm->start+i];//pid of the process we wanna work on


            isTerminated=waitpid(pid, &status, WNOHANG);//checks if the process is terminated

            if (isTerminated==0){
                //if process isn't terminated, it's paused and is queued and exec time is updated
                kill(pid,SIGCONT);
                shm->table[hash(pid)]->exec_time+=((tSlice*1000)/1000000000.0);
                enqueue(pid);
            }
            else{
                //if process is terminates, exec time and completion time are updates
                shm->table[hash(pid)]->exec_time+=((tSlice*1000)/1000000000.0);
                struct timespec end;
                timespec_get(&end, TIME_UTC);
                shm->table[hash(pid)]->completion_time=end.tv_nsec/1000000000.0;
            }
        }

        //increasing the start pointer of the queue
        if(ncpu>(shm->end-shm->start)){
            shm->start=shm->end;
        }  
        else{
            shm->start+=ncpu;
        }     
        sem_post(&shm->mutex);
    }
}