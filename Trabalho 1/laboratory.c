#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "auxiliary_functions.h"


#define DEAD_VIRUS 1
#define SECRET_INPUTS 2
#define INJECTION 3


typedef struct thread_lab{
    //mudar para string
    int id_lab;
    int count_production;
    pthread_t thread_id;
    pthread_mutex_t* mutex;
    sem_t* sem_bench;
    int* inputs_bench;
    int condition_stop;
    int* work_done;
    int minimal_objective;

}thread_laboratory;


void* lab_production(void* arg) {
    thread_laboratory* laboratory = (thread_laboratory*)arg;
    int slot1, slot2;
    int  production = 1;
    while (production){
        int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
            if (jobs_moment == 0){
                production = 0;
            }
        if (laboratory->id_lab == 0){
            sem_getvalue(&laboratory->sem_bench[0], &slot1);
            sem_getvalue(&laboratory->sem_bench[1], &slot2);
            if (slot1 == 0 && slot2 == 0){
                pthread_mutex_lock(laboratory->mutex);
                int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {
                    sem_post(&laboratory->sem_bench[0]);
                    sem_post(&laboratory->sem_bench[1]);
                    laboratory->inputs_bench[0] = SECRET_INPUTS;
                    laboratory->inputs_bench[1] = DEAD_VIRUS;
                    laboratory->work_done[0] += 1;
                    laboratory->count_production += 1;
                    laboratory->condition_stop += 1;
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0.5);
                }
            }

        }
        else if (laboratory->id_lab == 1){
            sem_getvalue(&laboratory->sem_bench[2], &slot1);
            sem_getvalue(&laboratory->sem_bench[3], &slot2);
            if (slot1 == 0 && slot2 == 0){
                pthread_mutex_lock(laboratory->mutex);
                int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {

                    sem_post(&laboratory->sem_bench[2]);
                    sem_post(&laboratory->sem_bench[3]);
                    laboratory->inputs_bench[2] = INJECTION;
                    laboratory->inputs_bench[3] = DEAD_VIRUS;
                    laboratory->work_done[1] += 1;
                    laboratory->count_production += 1;
                    laboratory->condition_stop += 1;
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0.5);
                }
            }
        }       
        else{
            // printf("LABORATORIO %d \n", laboratory->id_lab);
            sem_getvalue(&laboratory->sem_bench[4], &slot1);
            sem_getvalue(&laboratory->sem_bench[5], &slot2);
            if (slot1 == 0 && slot2 == 0){
                pthread_mutex_lock(laboratory->mutex);
                int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {
                    sem_post(&laboratory->sem_bench[4]);
                    sem_post(&laboratory->sem_bench[5]);
                    laboratory->inputs_bench[4] = INJECTION;
                    laboratory->inputs_bench[5] = SECRET_INPUTS;
                    laboratory->work_done[2] += 1;
                    laboratory->count_production += 1;
                    laboratory->condition_stop +=1;         
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0.5);
                }
            }
        }  
    }
    return NULL;
}