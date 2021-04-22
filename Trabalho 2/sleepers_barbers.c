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
    /* variaveis de entrada*/
    int num_barber = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int minimum_service_quantity = atoi(argv[3]);
    
    /* CONDIÇÕES BÁSICAS DE FUNCIONAMENTO*/
    if ( num_barber <= 0  || num_chairs <= 0 || minimum_service_quantity <= 0){
        printf("não abriremos hoje!\n");
        return 0;
    }

    /*VALOR INICIAL DO VISOR DA BARBEARIA*/
    int screen = -1;

    /*VETOR PARA ARMAZENAR O TRABALHO DE CADA BARBEIRO*/
    int* barber_service = (int*)malloc(num_barber*sizeof(int));
    for (int i = 0; i < num_barber; i++){
        barber_service[i] = 0;
    }

    /* INICIANDO  SEMÁFOROOS E MUTEXES*/
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
    

    /*CRIANDO A BARBEARIA*/
    barber_shop* pitoco_barber_shop = (barber_shop*)malloc(sizeof(barber_shop));
    pitoco_barber_shop->barber_service = barber_service;
    pitoco_barber_shop->is_open = 1;
    pitoco_barber_shop->number_of_barbers = num_barber;
    pitoco_barber_shop->sem_chairs = &sem_chairs;

    /*INICIALIZANDO OS BARBEIROS*/
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

    /*CRIANDO OS BARBEIROS*/
    for (int i = 0; i < num_barber; i++) {
        pthread_create(&barbers[i].thread_id, NULL, barber_jobs, &(barbers[i]));
    }

    /*PARTE IMPORTANTE DO CÓDIGO!!*/
   
    /*Como comentei com você no whatsapp, O linux não cria um numero de threads 
        maior do que 32000 aproximadamente. eu recebo o erro
        "pthread_create: Resource temporarily unavailable",
        Em testes no WINDOWS, utilizando o Windows subsystem Linux(WSL)
        Ele rodou normalmente, até pra casos com 1000 1000 1000 como valores de entrada.
        Então, caso esteja no linux e vá testar casos grandes, será necessário 
        utilizar as funções que estão comentadas abaixo e também utilizar 
        a função pthread_create para cliente que está comentada 
        com o atributo tattr como parâmetro.*/


    /*https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html*/
    /*https://man7.org/linux/man-pages/man3/pthread_attr_setdetachstate.3.html*/


    /*
    pthread_attr_t tattr;
    */

    /* inicializa variavel atributo thread */
    /*
    if (pthread_attr_init(&tattr) != 0) {
        printf("Erro ao inicializar o thread atributo \n");
        return -1;
    }
    */

    /* define atributo para detached (recursos podem ser reutilizados assim que o encadeamento termina) */
    /*
    if (pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED) != 0) {
        printf("Erro ao definir atributo da thread \n");
        return -1;
    }
    */

    int i = 0;
    
    /* LOOP PARA CRIAR CLIENTES ENQUANTO A BARBEARIA ESTIVER ABERTA */
    while (pitoco_barber_shop->is_open){
        if (pitoco_barber_shop->is_open == 0){
            // printf("barbearia fechada! Os barbeiros ja trabalharam o que devia por hoje... \n");
            return 0;
        }

        /* CRIANDO A STRUCT CLIENT */
        thread_client* client = (thread_client*)malloc(sizeof(thread_client));        
        client->id_client = i;
        client->sem_barber_chair = sem_barber_chair;
        client->sem_cut_hair = sem_cut_hair;
        client->sem_service_chair = sem_service_chair;
        client->screen = &screen;
        client->barber_shop = pitoco_barber_shop;
        client->sem_changes_display = &sem_changes_display;
        client->sem_read_display = &sem_read_display;
        
        /* CASO DESCOMENTE AS FUNÇÕES LÁ EM CIMA, O PTHREAD CREATE É ESSE ABAIXO*/

        /*---------------------------------------------------------------------*/
        /*int error = pthread_create(&client[0].thread_id, &tattr, clients_barber, &(client)[0]);*/
        /*---------------------------------------------------------------------*/

        int error = pthread_create(&client[0].thread_id,NULL, clients_barber, &(client)[0]);
		if (error != 0)
			handle_error_en(error, "pthread_create");

        i += 1;
        // sleep(0.3);

    }

    // printf("todos os barbeiros terminaram seu trabalho\n");
    // printf("\n");

    /* FUNÇÃO QUE ENCERRA O EXPEDIENTE DA BARBEARIA */    
    close_barber_shop(sem_service_chair, &sem_changes_display, num_barber);

    printf("\n");
    close_barber_shop(sem_service_chair, &sem_changes_display, num_barber);
    // sleep(2);

    for (int i = 0; i < num_barber; i++) {
        pthread_join(barbers[i].thread_id, NULL);
    } 

    for(int i = 0; i < num_barber; i++){
        printf("barbeiro %d atendeu -> %d \n",i, barber_service[i]);
    }
    printf("\n");

    return 0;
}