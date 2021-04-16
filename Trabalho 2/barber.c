#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

// CRIAR UMA BARBEARIA: ELA VAI TER
// QUANTIDADE DE ATENDIMENTO POR BARBEIRO
// QUANTIDADE DE CADEIRAS
// QUANTIDADE DE BARBEIROS
// ATRIBUTO OPEN E CLOSED
// ELA INICIA A O EXPEDIENTE

typedef struct barber_shop{
    int* barber_service;
    sem_t* sem_chairs;
    int number_of_barbers;
    int is_open;

}barber_shop;


typedef struct thread_barber{
    int id_barber;
    int* work_done;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;
    int* screen;
    int minimum_service_quantity;
    int number_of_barbers;

}thread_barber;

typedef struct thread_client{
    int id_client;
    int* work_done;
    pthread_t thread_id;
    sem_t* sem_service_chair;
    sem_t* sem_barber_chair;
    sem_t* sem_cut_hair;
    int* screen;
    int number_of_barbers;

}thread_client;


sem_t sem_chairs;
sem_t sem_barber_chair;
sem_t sem_cut_hair;
sem_t sem_service_chair;
sem_t sem_changes_display;
sem_t sem_read_display;
// int screen;


int jobs(int* list, int objective, int barber_quantity){
    int j = 0;
    for (int i = 0; i < barber_quantity; i++){
        if (list[i] >= objective){
            j += 1;
        }
    }
    if (j == barber_quantity){
        return 0;
    }
    return 1;
}

void* barber_jobs(void * arg) {
    thread_barber* barber = (thread_barber*)arg;
    printf("barbeiro %d chegou para trabalhar! \n", barber->id_barber);
    int is_open = 1;

    while (is_open){
        //Verifico se ainda preciso repor recursos, se nao preciso, eu finalizo o processo
        int jobs_moment = jobs(barber->work_done, barber->minimum_service_quantity,barber->number_of_barbers);
        if (jobs_moment == 0){
            is_open = 0;
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje...");
            return NULL;
        }
        sem_wait(&sem_changes_display);
        *(barber->screen) = barber->id_barber;
        // screen = barber->id_barber;
        printf("barbeiro %d escreveu seu nome no visor! valor do visor é de: %d\n", barber->id_barber, *(barber->screen));
        sem_post(&sem_read_display);
        // printf("aaa \n");
        sem_wait(&barber->sem_service_chair[barber->id_barber]);
        // sleep(10);
        printf("Barbeiro %d cortou o cabelo de um cliente.\n", barber->id_barber);
        barber->work_done[barber->id_barber] += 1;
        sem_post(&barber->sem_cut_hair[barber->id_barber]);
        // sleep(random()%3);
    }
    return NULL;
}

void* clients_barber(void* arg) {
    thread_client* client = (thread_client*)arg;
    int my_chair;

    if (sem_trywait(&sem_chairs) == 0) {
        printf("Cliente %d entrou na barbearia\n", client->id_client);
        sem_wait(&sem_read_display);
        my_chair = *(client->screen);
        // my_chair = screen;
        printf("Cliente %d entrou leu o visor que ta com valor %d.\n", client->id_client, my_chair);
        sem_post(&sem_changes_display);
        // semaforo pra avisar aos clientes que aquele barb ta trabalhando
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
    // thread_client* client;

    // /* MUDAR DEPOIS PARA GERAR INFINITAMENTE */
    // thread_client* clients = (thread_client*)malloc(15*sizeof(thread_client));
    // /* MUDAR DEPOIS PARA GERAR INFINITAMENTE */

    int screen = -1;


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
        barbers[i].screen = &screen;
        barbers[i].minimum_service_quantity = minimum_service_quantity;
        barbers[i].number_of_barbers = num_barber;

    }


    for (int i = 0; i < num_barber; i++) {
        pthread_create(&barbers[i].thread_id, NULL, barber_jobs, &(barbers[i]));
    }
    // for (int i = 0; i < num_barber; i++) {
    //     pthread_join(barbers[i].thread_id, NULL);
    // } 

    // printf("passe daqui");

    int i = 0;
    int barber_shop_open = 1;
    
    while (barber_shop_open){
        int jobs_barber = jobs(work_done, minimum_service_quantity,num_barber);
        if (jobs_barber == 0){
            barber_shop_open = 0;
            printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje... \n");
            barber_shop_open = 0;
            // break;
        }
        thread_client* client = (thread_client*)malloc(sizeof(thread_client));        
        client->id_client = i;
        client->work_done = work_done;
        client->sem_barber_chair = sem_barber_chair;
        client->sem_cut_hair = sem_cut_hair;
        client->sem_service_chair = sem_service_chair;
        client->screen = &screen;
        // client->number_of_barbers = num_barber;
        pthread_create(&client[0].thread_id, NULL, clients_barber, &(client)[0]);
        i += 1;
        // printf("aaa");
    }
    
    for(int i = 0; i < num_barber; i++){
        printf("barbeiro %d atendeu -> %d\n",i, work_done[i]);
    }
    printf("\n");

    // for (int i = 0; i < 15; i++) 
    //     pthread_join(clients[i].thread_id, NULL);
    // nao precisa do join cliente
    

    return 0;
}