#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "elemento.h"
#include "fisicaDinamica.h"
#define FOGO 'f'
#define AGUA 'a'
#define GRAMA 'g'
#define RAIO 10.0

typedef struct argumentos
{
    char corInicial;
    Unidade* unidadePosicao;
}Args;

typedef struct elemento
{
    Lista* elementoForteContra;
    Lista* elementoFracoContra;
    Unidade* respectivaUnidade;
}Elemento;

void trocaElemento(char cor, Elemento* elemento)
{
    switch(cor)
    {
        case FOGO:
                elemento->respectivaUnidade->dado.cor = FOGO;
                elemento->elementoForteContra = ListaGrama;
                elemento->elementoFracoContra = ListaAgua;
            break;
        case AGUA:
                elemento->respectivaUnidade->dado.cor = AGUA;
                elemento->elementoForteContra = ListaFogo;
                elemento->elementoFracoContra = ListaGrama;
            break;
        case GRAMA:
                elemento->respectivaUnidade->dado.cor = GRAMA;
                elemento->elementoForteContra = ListaAgua;
                elemento->elementoFracoContra = ListaFogo;
            break;
    }
}

void trocaUnidadeListas(Unidade *un)
{
    int ID = un->dado.ID;
    char cor = un->dado.cor;

    switch(cor)
    {
        case FOGO:
                remove_uni_lista(ListaFogo,ID);
                insere_uni_lista_final(ListaAgua,un);
            break;
        case AGUA:
                remove_uni_lista(ListaAgua,ID);
                insere_uni_lista_final(ListaGrama,un);
            break;
        case GRAMA:
                remove_uni_lista(ListaGrama,ID);
                insere_uni_lista_final(ListaFogo,un);
            break;
    }
}

void* thrElemento(void* endeArgs)
{
    Args* args = (Args*) endeArgs;

    char corInicial = args->corInicial;
    Unidade* unidadePosicao = args->unidadePosicao;
    Elemento elemento;

    //Inicia elemento
    trocaElemento(corInicial,&elemento);

    ///Verificando se deve trocar de elemento
    Unidade* aux = elemento.elementoFracoContra->inicio;
    double posicaoAtual[2];
    double posicaoOutros[2];
    double dist;
    posicaoAtual[0] = elemento.respectivaUnidade->dado.x;
    posicaoAtual[1] = elemento.respectivaUnidade->dado.y;

    //Se houver escritor, espera
    pthread_mutex_lock(&mutexTemEscritor);
    while(temEscritor)
    {
        pthread_cond_wait(&condEscritor,&mutexTemEscritor);
    }
    pthread_mutex_unlock(&mutexTemEscritor);

    pthread_mutex_lock(&mutexNumLeitores);
    numLeitores++;
    if(numLeitores == 1) pthread_mutex_lock(&mutexListas);
    pthread_mutex_unlock(&mutexNumLeitores);

    while(aux != NULL)
    {
        posicaoOutros[0] = aux->dado.x;
        posicaoOutros[1] = aux->dado.y;

        dist = calcula_distancia(posicaoAtual,posicaoOutros);

        //Condição de colisão
        if(dist < 2*RAIO)
        {
            //Sinalizar que há um novo escritor
            pthread_mutex_lock(&mutexTemEscritor);
            temEscritor++;
            pthread_mutex_unlock(&mutexTemEscritor);

            //Hora de escrever
            pthread_mutex_lock(&mutexListas);
            trocaUnidadeListas(elemento.respectivaUnidade);
            trocaElemento(aux->dado.cor,&elemento);
            pthread_mutex_unlock(&mutexListas);

            //Sinalizar que deixou de ser escritor
            pthread_mutex_lock(&mutexTemEscritor);
            temEscritor--;
            pthread_mutex_unlock(&mutexTemEscritor);

            //Acordar os leitores em espera
            pthread_cond_signal(&condEscritor);

            break;
        }
    }

    pthread_mutex_lock(&mutexNumLeitores);
    numLeitores--;
    if(numLeitores == 0) pthread_mutex_unlock(&mutexListas);
    pthread_mutex_unlock(&mutexNumLeitores);
    
    //Calcular força: 
}