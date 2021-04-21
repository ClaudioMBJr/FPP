#ifndef  CLIENT_H
#define  CLIENT_H


typedef struct thread_client{
    int id_client;
    int* work_done;
    int* screen;
    int number_of_barbers;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;
    barber_shop* barber_shop;
    sem_t* sem_changes_display;
    sem_t* sem_read_display;



}thread_client;


void* clients_barber(void* arg);


#endif
