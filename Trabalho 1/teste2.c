#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

#define DEAD_VIRUS 1
#define INPUTS 2
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

int jobs(int* list, int objective){
    int j = 0;
    for (int i = 0; i < 6; i++){
        if (list[i] >= objective){
            // printf("ELEMENTO DA LISTA %d E OBJECTVE -> %d \n", list[i], objective);
            // printf("O TAMANHO DO J ATÉ O MOMENTO É: %d \n", j);
            j += 1;
        };
    };
    if (j == 6){
        // printf("TAMANHO DO J É DE: %d \n", j);
        return 0;
    }
    // printf("TAMANHO DO J É DE: %d \n", j);
    return 1;
}

void* lab_production(void* arg) {
    thread_laboratory* laboratory = (thread_laboratory*)arg;
    int slot1, slot2;
    int  production = 1;
    while (production){
        //CRIAR UMA LISTA GLOBAL ONDE GERAL ACESSA E MUDA APENAS SUA POSIÇÃO, ISSO PODE FUNCIONAR
        //ISSO AQUI PODE DAR MERDA
        // pthread_mutex_lock(laboratory->mutex);
        // int all_jobs = jobs(laboratory->work_done, laboratory->minimal_objective);
        // printf("all jobs lab -> %d \n", all_jobs);
        // pthread_mutex_unlock(laboratory->mutex);
        //ISSO AQUI PODE DAR MERDA  
        // if (all_jobs == 1){
        if(laboratory->id_lab == 0){
            pthread_mutex_lock(laboratory->mutex);
            int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
            if (jobs_moment == 0) {
                production = 0;
                pthread_mutex_unlock(laboratory->mutex);
            }
            else {
            // printf("lab 0 pegou a bancada \n");
            sem_getvalue(&laboratory->sem_bench[0], &slot1);
            sem_getvalue(&laboratory->sem_bench[1], &slot2);
            if ( slot1 == 0 && slot2 == 0 ){
                sem_post(&laboratory->sem_bench[0]);
                sem_post(&laboratory->sem_bench[1]);
                laboratory->inputs_bench[0] = INPUTS;
                laboratory->inputs_bench[1] = DEAD_VIRUS;
                laboratory->work_done[0] += 1;
                laboratory->count_production += 1;

                // printf("laboratorio %d entrou e deixou recursos \n", laboratory->id_lab);
                pthread_mutex_unlock(laboratory->mutex);
                // sleep(0.5);
            }
            else {
                // printf("laboratorio %d entrou e não deixou recursos pq estava lotado \n", laboratory->id_lab);
                pthread_mutex_unlock(laboratory->mutex);
                // sleep(1);
                /*** TENHO QUE FAZE ALGO PA ELE DORMIR SEM USAR SLEEP ***/
                }
            }
        }
        else if (laboratory->id_lab == 1){
            pthread_mutex_lock(laboratory->mutex);
            int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
            if (jobs_moment == 0) {
                production = 0;
                pthread_mutex_unlock(laboratory->mutex);
            }
            else{
                // printf("lab 1 pegou a bancada \n");
                sem_getvalue(&laboratory->sem_bench[2], &slot1);
                sem_getvalue(&laboratory->sem_bench[3], &slot2);
                if ( slot1 == 0 && slot2 == 0 ){
                    sem_post(&laboratory->sem_bench[2]);
                    sem_post(&laboratory->sem_bench[3]);
                    laboratory->inputs_bench[2] = INJECTION;
                    laboratory->inputs_bench[3] = DEAD_VIRUS;
                    laboratory->work_done[1] += 1;
                    laboratory->count_production += 1;
                // printf("laboratorio %d entrou e deixou recursos \n", laboratory->id_lab);
                    pthread_mutex_unlock(laboratory->mutex); 
                    // sleep(0.5);         
                }
                else{                
                pthread_mutex_unlock(laboratory->mutex);
                // sleep(1);
                }         
            }
        }
        else{
            pthread_mutex_lock(laboratory->mutex);
            int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
            if (jobs_moment == 0) {
                production = 0;
                pthread_mutex_unlock(laboratory->mutex);
                }
            else {                
                // printf("lab 2 pegou a bancada \n");
                sem_getvalue(&laboratory->sem_bench[4], &slot1);
                sem_getvalue(&laboratory->sem_bench[5], &slot2);
                if ( slot1 == 0 && slot2 == 0 ){
                    sem_post(&laboratory->sem_bench[4]);
                    sem_post(&laboratory->sem_bench[5]);
                    laboratory->inputs_bench[4] = INPUTS;
                    laboratory->inputs_bench[5] = INJECTION;
                    laboratory->work_done[2] += 1;
                    laboratory->count_production += 1;
                    // printf("laboratorio %d entrou e deixou recursos \n", laboratory->id_lab);
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0.5);
                }
                else{
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(1);
                    }
                }
                
            }
        }
    
    return NULL;
}

void* infected_consumer(void* arg) {
    thread_infected* infected = (thread_infected*)arg;
    //MUDAR ISSO AQUI PARA UM ENDEREÇO DE MEMÓRIA COMUM ENTRE AS THREADS
    int  consume = 1; 
    while (consume){
        // //ISSO AQUI PODE DAR MERDA!!
        // int all_jobs = jobs(infected->consumer_done, infected->minimal_objective);
        // //ISSO AQUI PODE DAR MERDA!!
        pthread_mutex_lock(infected->mutex);

        int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
        if (jobs_moment == 0) {
            consume = 0;
            pthread_mutex_unlock(infected->mutex);
        }
        else { 
            // pthread_mutex_lock(infected->mutex);
            int vet[6] = {0}; 
            int insume;
            for (int i = 0; i < 6; i++){
                sem_getvalue(&infected->sem_bench[i], &insume);
                if (insume == 1){
                    vet[i] = infected->inputs_bench[i];
                    // printf("ADD ESSE ELEMENTO NO VETOR %d \n", vet[i]);
                }
            }
            // for (int i = 0; i < 6; i++){
            //     printf(" %d ", vet[i]);  
            // }
            // printf("\n");
            int input1 = -1;
            int input2 = -1;
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
                // printf("input1 -> %d \n", input1);
                // printf("input1 -> %d \n", input2);
                if (input1 != -1 && input2 != -1){
                    sem_wait(&infected->sem_bench[input1]);
                    sem_wait(&infected->sem_bench[input2]);
                    infected->work_done[3] += 1;
                    infected->count_injection += 1;
                    // printf("O infectado 0 está vacinando!!\n");
                    pthread_mutex_unlock(infected->mutex);
                }
                else{
                    pthread_mutex_unlock(infected->mutex);
                    
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
                // printf("input1 -> %d \n", input1);
                // printf("input2 -> %d \n", input2);
                
                //ESSA CONDICIONAL TA ERADAAAAAA!! ------------------------------------------------------------------------------------
                
                if (input1 != -1 && input2 != -1){
                    // printf("input1 -> %d \n", input1);
                    // printf("input1 -> %d \n", input2);
                    sem_wait(&infected->sem_bench[input1]);
                    sem_wait(&infected->sem_bench[input2]);
                    infected->work_done[4] += 1;
                    infected->count_injection += 1;
                    // printf("O infectado 1 está vacinando!!\n");
                    // exit(0);
                    pthread_mutex_unlock(infected->mutex);

                }
                else{
                    pthread_mutex_unlock(infected->mutex);
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
                // printf("input1 -> %d \n", input1);
                // printf("input2 -> %d \n", input2);
                if (input1 != -1 && input2 != -1){
                    sem_wait(&infected->sem_bench[input1]);
                    sem_wait(&infected->sem_bench[input2]);
                    infected->work_done[5] += 1;
                    infected->count_injection += 1;
                    // printf("O infectado 2 está vacinando!!!\n");
                    // exit(0);
                    pthread_mutex_unlock(infected->mutex);

                }
                else{
                    pthread_mutex_unlock(infected->mutex);
                }

            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    thread_laboratory* laboratorys = (thread_laboratory*)malloc(3*sizeof(thread_laboratory));
    thread_infected* infecteds = (thread_infected*)malloc(3*sizeof(thread_infected));
    pthread_mutex_t mutex;
    sem_t* sem_bench = (sem_t*)malloc(6*sizeof(sem_t));
    int* inputs_bench = (int*)malloc(6*sizeof(int));
    int* work_done = (int*)malloc(6*sizeof(int));
    // int* consumer_done = (int*)malloc(3*sizeof(int));

    for (int i = 0; i < 6; i++){
        work_done[i] = 0;
    }
    // for (int i = 0; i < 3; i++){
    //     consumer_done[i] = 0;
    // }

    int num_jobs = atoi(argv[1]);
    // printf("ARGUMENTO DE ENTRADA = %d \n", num_jobs);
 
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


    for(int i = 0; i < 6; i++){
        printf(" JOBS -> %d\n", work_done[i]);
    }
    printf("\n");

    pthread_mutex_destroy(&mutex);
    sem_destroy(sem_bench);

    return 0;
}