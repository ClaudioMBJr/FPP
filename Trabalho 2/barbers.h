
#ifndef  BARBERS_H
#define  BARBERS_H


typedef struct thread_barber{
    int id_barber;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;
    int* screen;
    int minimum_service_quantity;
    int number_of_barbers;
    barber_shop* barber_shop;
    pthread_mutex_t* mutex;
    sem_t* sem_changes_display;
    sem_t* sem_read_display;

}thread_barber;

void* barber_jobs(void * arg);





#endif
