#ifndef  INFECTED_H
#define  INFECTED_H


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

void* infected_consumer(void* arg);

#endif