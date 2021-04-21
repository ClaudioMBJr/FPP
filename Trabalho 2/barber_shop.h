#ifndef  BARBERSHOP_H
#define  BARBERSHOP_H

typedef struct barber_shop{
    int* barber_service;
    sem_t* sem_chairs;
    int number_of_barbers;
    int is_open;

}barber_shop;

#endif