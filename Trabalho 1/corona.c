
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include "laboratory.h"
#include "infected.h"


int main(int argc, char* argv[]){
    thread_laboratory* laboratorys = (thread_laboratory*)malloc(3*sizeof(thread_laboratory));
    thread_infected* infecteds = (thread_infected*)malloc(3*sizeof(thread_infected));
    pthread_mutex_t mutex;
    sem_t* sem_bench = (sem_t*)malloc(6*sizeof(sem_t));
    int* inputs_bench = (int*)malloc(6*sizeof(int));
    int* work_done = (int*)malloc(6*sizeof(int));

    for (int i = 0; i < 6; i++){
        work_done[i] = 0;
    }

    int num_jobs = atoi(argv[1]);
 
    pthread_mutex_init(&mutex,NULL);
    
    for(int i=0; i<6; i++){
        sem_init(&sem_bench[i],0,0);
    }

    for(int i = 0; i < 3; i++ ){
        laboratorys[i].id_lab = i;
        laboratorys[i].count_production = 0;
        laboratorys[i].condition_stop = 0;
        laboratorys[i].mutex = &mutex;
        laboratorys[i].sem_bench = sem_bench;
        laboratorys[i].inputs_bench = inputs_bench;
        laboratorys[i].minimal_objective = num_jobs;
        laboratorys[i].work_done = work_done;
    }
    for (int i = 0; i < 3; i++){
        infecteds[i].id_infected = i;
        infecteds[i].count_injection = 0;
        infecteds[i].mutex = &mutex;
        infecteds[i].sem_bench = sem_bench;
        infecteds[i].inputs_bench = inputs_bench;
        infecteds[i].condition_stop = 0;
        infecteds[i].work_done = work_done;
        infecteds[i].minimal_objective = num_jobs;
    }

    for(int i = 0 ; i < 3 ; i++){
        pthread_create(&laboratorys[i].thread_id,NULL,lab_production,&(laboratorys[i]));
    }
    for(int i = 0 ; i < 3 ; i++){
        pthread_create(&infecteds[i].thread_id,NULL,infected_consumer,&(infecteds[i]));
    }
    for(int i = 0; i < 3; i++){
        pthread_join(laboratorys[i].thread_id,NULL);      
    }
    for(int i = 0; i < 3; i++){
        pthread_join(infecteds[i].thread_id,NULL);      
    }

    for(int i = 0; i < 3; i++){
        printf(" LABORATORIO %d PRODUZIU -> %d\n",i, work_done[i]);
    }
    printf("\n");
    for(int i = 3; i < 6; i++){
        printf(" INFECTADO %d CONSUMIU -> %d\n",i, work_done[i]);
    }
    printf("\n");

    pthread_mutex_destroy(&mutex);
    sem_destroy(sem_bench);

    return 0;
}