#ifndef  BARBERSHOP_H
#define  BARBERSHOP_H

typedef struct barber_shop{
    int* barber_service;
    sem_t* sem_chairs;
    int number_of_barbers;
    int is_open;

}barber_shop;

void* close_barber_shop(sem_t* sem_service_chair, sem_t* sem_changes_display, int number_barbers);



#endif