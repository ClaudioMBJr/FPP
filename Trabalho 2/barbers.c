#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "auxiliary_functions.h"
#include <semaphore.h>
#include <unistd.h>
#include "barber_shop.h"

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


void* barber_jobs(void * arg) {
    thread_barber* barber = (thread_barber*)arg;
    printf("barbeiro %d chegou para trabalhar! \n", barber->id_barber);
    int is_open = barber->barber_shop->is_open;
    while (is_open){
        //Verifico se ainda preciso repor recursos, se nao preciso, eu finalizo o processo
        int jobs_moment = jobs(barber->barber_shop->barber_service, barber->minimum_service_quantity, barber->barber_shop->number_of_barbers);
        if (jobs_moment == 0){
            pthread_mutex_lock(barber->mutex);
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje...");
            barber->barber_shop->is_open = 0;
            is_open = 0;
            pthread_mutex_unlock(barber->mutex);
            return NULL;
        }


        sem_wait(barber->sem_changes_display);
        jobs_moment = jobs(barber->barber_shop->barber_service, barber->minimum_service_quantity,barber->barber_shop->number_of_barbers);
        if (jobs_moment == 0){
            pthread_mutex_lock(barber->mutex);
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje...");
            barber->barber_shop->is_open = 0;
            pthread_mutex_unlock(barber->mutex);
            is_open = 0;

            return NULL;
        }



        *(barber->screen) = barber->id_barber;
        // screen = barber->id_barber;
        printf("barbeiro %d escreveu seu nome no visor! valor do visor é de: %d\n", barber->id_barber, *(barber->screen));
        sem_post(barber->sem_read_display);
        // printf("aaa \n");


        sem_wait(&barber->sem_service_chair[barber->id_barber]);
        jobs_moment = jobs(barber->barber_shop->barber_service, barber->minimum_service_quantity,barber->barber_shop->number_of_barbers);
        if (jobs_moment == 0){
            pthread_mutex_lock(barber->mutex);
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje...");
            barber->barber_shop->is_open = 0;
            pthread_mutex_unlock(barber->mutex);
            is_open = 0;

            return NULL;
        }


        // sleep(0.2);
        printf("Barbeiro %d cortou o cabelo de um cliente.\n", barber->id_barber);
        // pthread_mutex_lock(barber->mutex);
        barber->barber_shop->barber_service[barber->id_barber] += 1;
        // pthread_mutex_unlock(barber->mutex);

        sem_post(&barber->sem_cut_hair[barber->id_barber]);
        // sleep(random()%3);
    }
    return NULL;
}