#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "barber_shop.h"
#include "barbers.h"
#include "client.h"
#include "auxiliary_functions.h"


#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(int argc, char* argv[]){

    int num_barber = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int minimum_service_quantity = atoi(argv[3]);
    
    if ( num_barber <= 0  || num_chairs <= 0 || minimum_service_quantity <= 0){
        printf("não abriremos hoje!\n");
        return 0;
    }

    int screen = -1;

    int* barber_service = (int*)malloc(num_barber*sizeof(int));
    for (int i = 0; i < num_barber; i++){
        barber_service[i] = 0;
    }

    pthread_mutex_t mutex;
    sem_t sem_chairs;
    sem_t sem_changes_display;
    sem_t sem_read_display;
    sem_t* sem_barber_chair = (sem_t*)malloc(num_barber*sizeof(sem_t));
    sem_t* sem_service_chair = (sem_t*)malloc(num_barber*sizeof(sem_t));
    sem_t* sem_cut_hair = (sem_t*)malloc(num_barber*sizeof(sem_t));

    for (int i = 0; i < num_barber; i++) {
        sem_init(&sem_barber_chair[i], 0, 1);
        sem_init(&sem_service_chair[i], 0, 0);
        sem_init(&sem_cut_hair[i], 0, 0);
    }

    pthread_mutex_init(&mutex,NULL);
    sem_init(&sem_chairs, 0, num_chairs);
    sem_init(&sem_changes_display, 0, 1);
    sem_init(&sem_read_display, 0, 0);
    
    barber_shop* pitoco_barber_shop = (barber_shop*)malloc(sizeof(barber_shop));
    pitoco_barber_shop->barber_service = barber_service;
    pitoco_barber_shop->is_open = 1;
    pitoco_barber_shop->number_of_barbers = num_barber;
    pitoco_barber_shop->sem_chairs = &sem_chairs;

    thread_barber* barbers = (thread_barber*)malloc(num_barber*sizeof(thread_barber));
    for (int i = 0; i < num_barber; i++){
        barbers[i].id_barber = i;
        barbers[i].sem_barber_chair = sem_barber_chair;
        barbers[i].sem_cut_hair = sem_cut_hair;
        barbers[i].sem_service_chair = sem_service_chair;
        barbers[i].screen = &screen;
        barbers[i].minimum_service_quantity = minimum_service_quantity;
        barbers[i].barber_shop = pitoco_barber_shop;
        barbers[i].mutex = &mutex;
        barbers[i].sem_changes_display = &sem_changes_display;
        barbers[i].sem_read_display = &sem_read_display;

    }

    for (int i = 0; i < num_barber; i++) {
        pthread_create(&barbers[i].thread_id, NULL, barber_jobs, &(barbers[i]));
    }

    // pthread_attr_t tattr;


    /* inicializa variavel atributo thread */
    // if (pthread_attr_init(&tattr) != 0) {
    //     printf("Erro ao inicializar o thread atributo \n");
    //     return -1;
    // }

    /* define atributo para detached (recursos podem ser reutilizados assim que o encadeamento termina) */
    // if (pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED) != 0) {
    //     printf("Erro ao definir atributo da thread \n");
    //     return -1;
    // }
    // printf("passe daqui \n");

    int i = 0;
    // int barber_shop_open = 1;
    
    while (pitoco_barber_shop->is_open){
        if (pitoco_barber_shop->is_open == 0){
            // barber_shop_open = 0;
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje... \n");
            return 0;
        }
        thread_client* client = (thread_client*)malloc(sizeof(thread_client));        
        client->id_client = i;
        client->sem_barber_chair = sem_barber_chair;
        client->sem_cut_hair = sem_cut_hair;
        client->sem_service_chair = sem_service_chair;
        client->screen = &screen;
        client->barber_shop = pitoco_barber_shop;
        client->sem_changes_display = &sem_changes_display;
        client->sem_read_display = &sem_read_display;
        

        // int error = pthread_create(&client[0].thread_id, &tattr, clients_barber, &(client)[0]);
        int error = pthread_create(&client[0].thread_id,NULL, clients_barber, &(client)[0]);

        // if (error != 0){
        //     printf("error ao criar o cliente \n");
        //     return -1;
        //     // sleep(10);
        // }


		if (error != 0)
			handle_error_en(error, "pthread_create");

        i += 1;
        // sleep(0.3);

    }

    printf("cheguei aqui entao todos os barbeiros terminaram seu trabalho\n");
    printf("\n");
    // int value;
    // sem_getvalue(&sem_chairs, &value);
    // printf("AINDA EXISTEM %d CLIENTES PARA SEREM ATENDIDOS", value);
    
    
    for (int i = 0; i < num_barber; i++){
        sem_post(&barbers[i].sem_service_chair[barbers[i].id_barber]);
    }

    for(int i =0 ; i <num_barber;i++){
        sem_post(&sem_service_chair[i]);
        sem_post(&sem_changes_display);
    }

    printf("\n");


    for (int i = 0; i < num_barber; i++) {
        pthread_join(barbers[i].thread_id, NULL);
    } 

    for(int i = 0; i < num_barber; i++){
        printf("barbeiro %d atendeu -> %d \n",i, barber_service[i]);
    }
    printf("\n");

 
    // for (int i = 0; i < 15; i++) 
    //     pthread_join(clients[i].thread_id, NULL);
    // nao precisa do join cliente
    

    return 0;
}