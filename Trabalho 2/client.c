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
        /* Tenta ver se tem cadeira disponivel. Caso não tenha, vai embora*/
        if (sem_trywait(client->barber_shop->sem_chairs) == 0) {
            // printf("Cliente %d entrou na barbearia\n", client->id_client);
            
            /*Aguarda para ler o display*/
            sem_wait(client->sem_read_display);
            barber_available = *(client->screen);
            // printf("Cliente %d leu o visor avisando que o barbeiro %d está dormindo na cadeira esperando pra ser acordado e trabalhar \n", client->id_client, barber_available);
 
            /*Avisa que já leu/entendeu que foi chamado e que pode chamar outro*/
            sem_post(client->sem_changes_display);

            /*semaforo pra avisar aos clientes que aquele barb ta trabalhando*/
            sem_wait(&client->sem_barber_chair[barber_available]);
            // printf("Cliente %d sentou na cadeira do barbeiro %d.\n", client->id_client, barber_available);

            /* COloca o barbeiro pra trabalhar*/
            sem_post(&client->sem_service_chair[barber_available]);

            /*Libera vaga nas cadeiras */
            sem_post(client->barber_shop->sem_chairs);

            /*Espera o cabelo ser cortado */
            sem_wait(&client->sem_cut_hair[barber_available]);

            /* Avisa aos clientes que o barb terminou o trab */
            sem_post(&client->sem_barber_chair[barber_available]);

            // printf("Cliente %d foi embora da barbearia com o cabelo cortado.\n", client->id_client);
            return NULL;
            
        } 
        else{
            // int ocupation;
            // sem_getvalue(client->barber_shop->sem_chairs, &ocupation);
            // printf("Cliente %d não entrou na barbearia. A quantidade de cadeiras disponiveis eram de %d \n", client->id_client, ocupation);
            return NULL;
        }

    }
}
