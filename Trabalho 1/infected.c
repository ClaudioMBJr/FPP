#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "auxiliary_functions.h"


//Struct da thread
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

        //VERIFICO SE TEM INSUMO NA BANCADA! 
        for (int i = 0; i < 6; i++){
            sem_getvalue(&infected->sem_bench[i], &insume);
            if (insume == 1){
                vet[i] = infected->inputs_bench[i];
            }
        }              

        
        if (infected->id_infected == 0){
            //Caso o laboratório for o 1, ele precisa dos INsumos 2 e 3
            //ENtão, eu verifoco se eles estavam disponíveis na bancada
            //OBS IMPORTANTE: EU PEGO SEMPRE O PRIMEIRO PAR DE RECURSO QUE ELA ENCONTRAR!
            //Essa busca poderia ser aleatória na bancada, mas implementei assim.
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
                //Verifico novamente a situação dos semáforos
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
                //verico se os recursos estao la novamente
                if (input1 != -1 && input2 != -1){
                    //se os recursos estão lá, eu verifico se eu ainda preciso consumir
                    //se eu nao preciso consumir mais, eu finalizo o processo da thread
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                        pthread_mutex_unlock(infected->mutex);
                    }
                    //caso contrário, a thread consome e muda os valores dos respectivos semáforos
                    // SInalizando que consumiu e que aquele produto já pode ter reposição
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
                //se os recursos não estavam lá na hora do lock, eu verifico tbm se eu ainda preciso fazer o trabalho
                //se eu nao preciso, eu finalizo o processo e libero o mutex;
                else{
                    int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                    if (jobs_moment == 0){
                        consume = 0;
                    }
                    pthread_mutex_unlock(infected->mutex);
                }
            }
            //Caso não tenha recurso antes de dar o lock, a célula tbm verifica se ainda precisa fazer o trabalho de consumir
            else{
                int jobs_moment = jobs(infected->work_done, infected->minimal_objective);
                if (jobs_moment == 0){
                    consume = 0;
                }

            }
        }

        //Repito o processo da thread anterior para as outras threadss
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