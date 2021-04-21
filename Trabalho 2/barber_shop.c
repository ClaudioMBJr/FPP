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
