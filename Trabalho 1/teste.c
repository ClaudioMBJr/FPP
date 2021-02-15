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
        }
    }
    // PRINT PRA VERIFICAR A SITUAÇÃO DO STARVATION

    // printf("[ ");
    // for (int i =0; i < 6; i++){
    //     printf("%d ", list[i]);
    // }
    // printf(" ] ");
    // printf("\n");
    if (j == 6){
        // printf("TAMANHO DO J É DE: %d \n", j);
        return 0;
    }
    // printf("TAMANHO DO J É DE: %d \n ENTAO PODE CONTINUAR \n", j);
    return 1;
}

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
            // printf("LABORATORIO %d \n", laboratory->id_lab);
            sem_getvalue(&laboratory->sem_bench[0], &slot1);
            sem_getvalue(&laboratory->sem_bench[1], &slot2);
            if (slot1 == 0 && slot2 == 0){
                pthread_mutex_lock(laboratory->mutex);
                int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
                // printf("JOBS MOMENT == %d LABORATORIO == %d \n", jobs_moment, laboratory->id_lab);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {
                    // printf("lab 0 pegou a bancada \n");               
                    sem_post(&laboratory->sem_bench[0]);
                    sem_post(&laboratory->sem_bench[1]);
                    laboratory->inputs_bench[0] = INPUTS;
                    laboratory->inputs_bench[1] = DEAD_VIRUS;
                    laboratory->work_done[0] += 1;
                    laboratory->count_production += 1;
                    laboratory->condition_stop += 1;
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0);
                // }
                }
            }

        }
        else if (laboratory->id_lab == 1){
            // printf("LABORATORIO %d \n", laboratory->id_lab);
            sem_getvalue(&laboratory->sem_bench[2], &slot1);
            sem_getvalue(&laboratory->sem_bench[3], &slot2);
            if (slot1 == 0 && slot2 == 0){
                pthread_mutex_lock(laboratory->mutex);
                // for (int i =0; i<6; i++){
                //     printf("%d ", laboratory->work_done[i]);
                // }
                // printf("\n");
                int jobs_moment = jobs(laboratory->work_done, laboratory->minimal_objective);
                // printf("JOBS MOMENT == %d LABORATORIO == %d \n", jobs_moment, laboratory->id_lab);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {
                    // printf("lab 1 pegou a bancada \n");
                    // sem_getvalue(&laboratory->sem_bench[0], &slot1);
                    // sem_getvalue(&laboratory->sem_bench[1], &slot2);
                    // if ( slot1 == 0 && slot2 == 0 ){
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
                // printf("JOBS MOMENT == %d LABORATORIO == %d \n", jobs_moment, laboratory->id_lab);
                if (jobs_moment == 0) {
                    production = 0;
                    pthread_mutex_unlock(laboratory->mutex);
                }
                else {
                    // printf("lab 2 pegou a bancada \n");
                    sem_post(&laboratory->sem_bench[4]);
                    sem_post(&laboratory->sem_bench[5]);
                    laboratory->inputs_bench[4] = INPUTS;
                    laboratory->inputs_bench[5] = INJECTION;
                    laboratory->work_done[2] += 1;
                    laboratory->count_production += 1;
                    laboratory->condition_stop +=1;         
                    pthread_mutex_unlock(laboratory->mutex);
                    // sleep(0.5);
                }
            }
        }  
    }
    printf("laboratorio %d nao poduzira mais \n", laboratory->id_lab);
    // exit(0);
    return NULL;
}

void* infected_consumer(void* arg) {
    thread_infected* infected = (thread_infected*)arg;
    // printf("INFECTED %d \n", infected->id_infected);
    int consume = 1;
    while (consume){
        int vet[6] = {0};
        int input1 = -1;
        int input2 = -1;
        int insume;
        //verificando a situação da bancada!
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
                        // printf("O INFECTADO %d IRA CONSUMIR OS RECURSOS QUE ESTAO NA POSIÇÃO %d e %d \n", infected->id_infected, input1, input2);
                        // printf("O infectado 0 está vacinando!!\n");
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
                    // pthread_mutex_unlock(infected->mutex);
                }
                // pthread_mutex_unlock(infected->mutex);

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
                // printf("ANTES DE TRAVAR A BANCADA, O INFECTADO %d VIU QUE INPUT1 E INPUT2 TINHAM OS VALORES %d e %d  \n", infected->id_infected, input1, input2);
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
                        // printf("O INFECTADO %d IRA CONSUMIR OS RECURSOS QUE ESTAO NA POSIÇÃO %d e %d \n", infected->id_infected, input1, input2);
                        // printf("O infectado 1 está vacinando!!\n");
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
                    // pthread_mutex_unlock(infected->mutex);
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
                // printf("ANTES DE TRAVAR A BANCADA, O INFECTADO %d VIU QUE INPUT1 E INPUT2 TINHAM OS VALORES %d e %d  \n", infected->id_infected, input1, input2);
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
                        // printf("O INFECTADO %d IRA CONSUMIR OS RECURSOS QUE ESTAO NA POSIÇÃO %d e %d \n", infected->id_infected, input1, input2);
                        // printf("O infectado 2 está vacinando!!\n");
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
                    // pthread_mutex_unlock(infected->mutex);
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
    for (int i = 0; i < 6; i++){
        inputs_bench[i] = 0;
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
        printf(" JOBS -> %d ", work_done[i]);
    }
    printf("\n");

    // pthread_mutex_destroy(&mutex);
    // sem_destroy(sem_bench);

    return 0;
}