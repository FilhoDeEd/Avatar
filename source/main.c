#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "elemento.h"

#define NE 3 //Aumente em 1 para cada erro adicionado no enum
enum erros {EFOGO=-NE,EAGUA,EGRAMA};

int main()
{
    //Inicializando listas
    ListaFogo = cria_lista();
    if(ListaFogo == NULL)
    {
        printf("Erro ao criar 'ListaFogo'.\n");
        exit(EFOGO);
    }

    ListaAgua = cria_lista();
    if(ListaAgua == NULL)
    {
        printf("Erro ao criar 'ListaAgua'.\n");
        exit(EAGUA);
    }

    ListaGrama = cria_lista();
    if(ListaGrama == NULL)
    {
        printf("Erro ao criar 'listaGrama'.\n");
        exit(EGRAMA);
    }

    //Inicializando os recursos para controlar leitores e escritores das listas
    pthread_mutex_init(&mutexTemEscritor, NULL);
    pthread_mutex_init(&mutexNumLeitores, NULL);
    pthread_mutex_init(&mutexListas, NULL);
    pthread_cond_init(&condEscritor,NULL); //pode ser null?
    temEscritor = 0;
    numLeitores = 0;

    //Inicializando as threads

    return 0;
}