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
#define INPUTS 2
#define INJECTION 3


typedef struct thread_infected{
    //mudar para string
    int id_infected;
    int count_injection;
    pthread_t thread_id;
    pthread_mutex_t* mutex;
    sem_t* sem_bench;
    int* inputs_bench;
    int condition_stop;
    int* work_done;
    int minimal_objective;
    // int my_insume;

}thread_infected;


void* infected_consumer(void* arg) {
    thread_infected* infected = (thread_infected*)arg;
    int consume = 1;
    while (consume){
        int vet[6] = {0};
        int input1 = -1;
        int input2 = -1;
        int insume;

        for (int i = 0; i < 6; i++){
            sem_getvalue(&infected->sem_bench[i], &insume);
            if (insume == 1){
                vet[i] = infected->inputs_bench[i];
            }
        }              

        if (infected->id_infected == 0){
            for (int j = 0; j < 6; j++){
                if (vet[j] == 2){
                    input1 = j;
                    break;
                }
            }
            for (int j = 0; j < 6; j++){
                if (vet[j] == 3){
                    input2 = j;
                    break;
                }
            }
            //se a bancada tiver os recursos, eu locko ela e verifico novamente
            if (input1 != -1 && input2 != -1){
                pthread_mutex_lock(infected->mutex);
                int vet[6] = {0}; 
                int input1 = -1;
                int input2 = -1;
                int sinal;
                for (int i = 0; i < 6; i++){
                    sem_getvalue(&infected->sem_bench[i], &sinal);
                    if (sinal == 1){
                        vet[i] = infected->inputs_bench[i];
                    }
                }   
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 2){
                        input1 = j;
                        break;
                    }
                }
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 3){
                        input2 = j;
                        break;
                    }
                }
                //verico se os recursos estao ok
                if (input1 != -1 && input2 != -1){
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                        pthread_mutex_unlock(infected->mutex);
                    }
                    else{                    
                        sem_wait(&infected->sem_bench[input1]);
                        sem_wait(&infected->sem_bench[input2]);
                        infected->work_done[3] += 1;
                        infected->count_injection += 1;
                        infected->condition_stop += 1;
                        pthread_mutex_unlock(infected->mutex);
                        // sleep(0.5);
                    }       
                
                }
                else{
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                    }
                    pthread_mutex_unlock(infected->mutex);
                }
            }
            else{
                int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                if (jobs_moment == 0){
                    consume = 0;
                }

            }
        }

        else if (infected->id_infected == 1){
            for (int j = 0; j < 6; j++){
                if (vet[j] == 1){
                    input1 = j;
                    break;
                }
            }
            for (int j = 0; j < 6; j++){
                if (vet[j] == 3){
                    input2 = j;
                    break;
                }
            }
            if (input1 != -1 && input2 != -1){
                pthread_mutex_lock(infected->mutex);
                int vet[6] = {0}; 
                int input1 = -1;
                int input2 = -1;
                int sinal;
                for (int i = 0; i < 6; i++){
                    sem_getvalue(&infected->sem_bench[i], &sinal);
                    if (sinal == 1){
                    vet[i] = infected->inputs_bench[i];
                    }
                }   
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 1){
                        input1 = j;
                        break;
                    }
                }
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 3){
                        input2 = j;
                        break;
                    }
                }
                if (input1 != -1 && input2 != -1){
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                        pthread_mutex_unlock(infected->mutex);
                    }
                    else{                    
                        sem_wait(&infected->sem_bench[input1]);
                        sem_wait(&infected->sem_bench[input2]);
                        infected->work_done[4] += 1;
                        infected->count_injection += 1;
                        infected->condition_stop += 1;
                        pthread_mutex_unlock(infected->mutex);
                        // sleep(0.5);

                    }       
                
                }
                else{
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                    }
                    pthread_mutex_unlock(infected->mutex);
                }

            }
            else{
                int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                if (jobs_moment == 0){
                    consume = 0;
                }
            }

        }
        else{
            for (int j = 0; j < 6; j++){
                if (vet[j] == 1){
                    input1 = j;
                    break;
                }
            }
            for (int j = 0; j < 6; j++){
                if (vet[j] == 2){
                    input2 = j;
                    break;
                }
            }
            if (input1 != -1 && input2 != -1){
                pthread_mutex_lock(infected->mutex);
                int vet[6] = {0}; 
                int input1 = -1;
                int input2 = -1;
                int sinal;
                for (int i = 0; i < 6; i++){
                    sem_getvalue(&infected->sem_bench[i], &sinal);
                    if (sinal == 1){
                    vet[i] = infected->inputs_bench[i];
                    }
                }   
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 1){
                        input1 = j;
                        break;
                    }
                }
                for (int j = 0; j < 6; j++){
                    if (vet[j] == 2){
                        input2 = j;
                        break;
                    }
                }                
                if (input1 != -1 && input2 != -1){
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                        pthread_mutex_unlock(infected->mutex);
                    }
                    else{                    
                        sem_wait(&infected->sem_bench[input1]);
                        sem_wait(&infected->sem_bench[input2]);
                        infected->work_done[5] += 1;
                        infected->count_injection += 1;
                        infected->condition_stop += 1;
                        pthread_mutex_unlock(infected->mutex);
                        // sleep(0.5);

                    }                    
                }
                else{
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                    }
                    pthread_mutex_unlock(infected->mutex);

                }
            }
            else{
                int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                if (jobs_moment == 0){
                    consume = 0;
                }
            }
        }
    }
    return NULL;
}