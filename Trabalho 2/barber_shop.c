#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "auxiliary_functions.h"

typedef struct barber_shop{
    int* barber_service;
    sem_t* sem_chairs;
    int number_of_barbers;
    int is_open;

}barber_shop;


void close_barber_shop(sem_t* sem_service_chair, sem_t* sem_changes_display, int number_barbers){
    for(int i =0 ; i < number_barbers ;i++){
        sem_post(&sem_service_chair[i]);
        sem_post(sem_changes_display);

    }
    return;
}
