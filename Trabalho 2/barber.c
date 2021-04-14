#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

// #define N_CLIENTES 10
// #define N_BARBEIROS 2
// #define N_CADEIRAS 5

typedef struct thread_barber{
    int id_barber;
    int* work_done;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;

}thread_barber;

typedef struct thread_client{
    int id_client;
    int* work_done;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;

}thread_client;



sem_t sem_chairs;
sem_t sem_barber_chair;
sem_t sem_cut_hair;
sem_t sem_service_chair;
sem_t sem_changes_display;
sem_t sem_read_display;

int visor;

void* barber_shop(void * arg) {
    thread_barber* barber = (thread_barber*)arg;
    printf("barbeiro %d chegou para trabalhar! \n", barber->id_barber);

    while(1) {
        sem_wait(&sem_changes_display);
        visor = barber->id_barber;
        printf("barbeiro %d escreveu seu nome no visor!\n", barber->id_barber);
        sem_post(&sem_read_display);
        sem_wait(&barber->sem_service_chair[barber->id_barber]);
        printf("Barbeiro %d cortou o cabelo de um cliente.\n", barber->id_barber);
        sem_post(&barber->sem_cut_hair[barber->id_barber]);
        sleep(random()%3);
    }
    return NULL;
}

void* clients_barber(void* arg) {
    thread_client* client = (thread_client*)arg;
    int my_chair;

    sleep(random()%3);
    if (sem_trywait(&sem_chairs) == 0) {
        printf("Cliente %d entrou na barbearia.\n", client->id_client);
        sem_wait(&sem_read_display);
        my_chair = visor;
        sem_post(&sem_changes_display);
        sem_wait(&client->sem_barber_chair[my_chair]);
        printf("Cliente %d sentou na cadeira do barbeiro %d.\n", client->id_client, my_chair);
        sem_post(&client->sem_service_chair[my_chair]);
        sem_post(&sem_chairs);
        sem_wait(&client->sem_cut_hair[my_chair]);
        sem_post(&client->sem_barber_chair[my_chair]);
        printf("Cliente %d deixou a barbearia.\n", client->id_client);
    } else
        printf("Cliente %d não entrou na barbearia.\n", client->id_client);
    return NULL;
}

int main(int argc, char* argv[]){

    int num_barber = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int minimum_service_quantity = atoi(argv[3]);

    int* work_done = (int*)malloc(num_barber*sizeof(int));

    for (int i = 0; i < num_barber; i++){
        work_done[i] = 0;
    }

    thread_barber* barbers = (thread_barber*)malloc(num_barber*sizeof(thread_barber));
    
    /* MUDAR DEPOIS PARA GERAR INFINITAMENTE */
    thread_client* clients = (thread_client*)malloc(15*sizeof(thread_client));
    /* MUDAR DEPOIS PARA GERAR INFINITAMENTE */



    sem_t* sem_barber_chair = (sem_t*)malloc(num_barber*sizeof(sem_t));
    sem_t* sem_service_chair = (sem_t*)malloc(num_barber*sizeof(sem_t));
    sem_t* sem_cut_hair = (sem_t*)malloc(num_barber*sizeof(sem_t));

    sem_init(&sem_chairs, 0, num_chairs);
    sem_init(&sem_changes_display, 0, 1);
    sem_init(&sem_read_display, 0, 0);

    for (int i = 0; i < num_barber; i++) {
        sem_init(&sem_barber_chair[i], 0, 1);
        sem_init(&sem_service_chair[i], 0, 0);
        sem_init(&sem_cut_hair[i], 0, 0);
    }

    for (int i = 0; i < num_barber; i++){
        barbers[i].id_barber = i;
        barbers[i].work_done = work_done;
        barbers[i].sem_barber_chair = sem_barber_chair;
        barbers[i].sem_cut_hair = sem_cut_hair;
        barbers[i].sem_service_chair = sem_service_chair;
    }


    for (int i = 0; i < 15; i++){
        clients[i].id_client = i;
        clients[i].work_done = work_done;
        clients[i].sem_barber_chair = sem_barber_chair;
        clients[i].sem_cut_hair = sem_cut_hair;
        clients[i].sem_service_chair = sem_service_chair;
    }



    for (int i = 0; i < num_barber; i++) {
        pthread_create(&barbers[i].thread_id, NULL, barber_shop, &(barbers[i]));
    }

    for (int i = 0; i < 15; i++) {
        pthread_create(&clients[i].thread_id, NULL, clients_barber, &(clients[i]));
    }

    for (int i = 0; i < num_barber; i++) {
        pthread_join(barbers[i].thread_id, NULL);
    } 

    for (int i = 0; i < 15; i++) 
        pthread_join(clients[i].thread_id, NULL);
  

    return 0;
}