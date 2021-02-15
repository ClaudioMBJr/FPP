#ifndef  LABORATORY_H
#define  LABORATORY_H

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

void* lab_production(void* arg);

#endif