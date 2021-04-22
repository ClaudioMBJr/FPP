#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "barber_shop.h"
#include "auxiliary_functions.h"


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


void* clients_barber(void* arg) {
    thread_client* client = (thread_client*)arg;
    int barber_available;

    if (client->barber_shop->is_open == 0) {
        return NULL;
    }
    else{
        // dar lock pra ve se ta aberta ainda
        if (sem_trywait(client->barber_shop->sem_chairs) == 0) {
            printf("Cliente %d entrou na barbearia\n", client->id_client);
            sem_wait(client->sem_read_display);
            // printf("TATATAATATA");
            barber_available = *(client->screen);
            printf("Cliente %d leu o visor avisando que o barbeiro %d está dormindo na cadeira esperando pra ser acordado e trabalhar \n", client->id_client, barber_available);
            sem_post(client->sem_changes_display);
            // semaforo pra avisar aos clientes que aquele barb ta trabalhando
            sem_wait(&client->sem_barber_chair[barber_available]);
            printf("Cliente %d sentou na cadeira do barbeiro %d.\n", client->id_client, barber_available);
            sem_post(&client->sem_service_chair[barber_available]);
            sem_post(client->barber_shop->sem_chairs);
            sem_wait(&client->sem_cut_hair[barber_available]);
            sem_post(&client->sem_barber_chair[barber_available]);

            printf("Cliente %d deixou a barbearia.\n", client->id_client);
            return NULL;
            
        } 
        else{
            int ocupation;
            sem_getvalue(client->barber_shop->sem_chairs, &ocupation);
            printf("Cliente %d não entrou na barbearia. A quantidade de cadeiras disponiveis eram de %d \n", client->id_client, ocupation);
            return NULL;
        }

    }
}
