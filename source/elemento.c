#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "elemento.h"
#include "fisicaDinamica.h"
#define RAIO 10.0

typedef struct argumentos
{
    char corInicial;
    Unidade* respectivaUnidade;
}Args;

typedef struct elemento
{
    Lista* elementoForteContra;
    Lista* elementoFracoContra;
    Unidade* respectivaUnidade;
}Elemento;

//Não crítico. Modifica os campos de Elemento e a cor
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

//Crítico. Troca uma unidade de uma lista para outra baseado na cor e no ID
void trocaUnidadeListas(Unidade *uni)
{
    int ID = uni->dado.ID;
    char cor = uni->dado.cor;

    switch(cor)
    {
        case FOGO:
                remove_uni_lista(ListaFogo,ID);
                insere_uni_lista_final(ListaAgua,uni);
            break;
        case AGUA:
                remove_uni_lista(ListaAgua,ID);
                insere_uni_lista_final(ListaGrama,uni);
            break;
        case GRAMA:
                remove_uni_lista(ListaGrama,ID);
                insere_uni_lista_final(ListaFogo,uni);
            break;
    }
}

void* thrElemento(void* endeArgs)
{
    Args* args = (Args*) endeArgs;

    char corInicial = args->corInicial;
    Unidade* respectivaUnidade = args->respectivaUnidade;
    Elemento elemento;

    ///Iniciando elemento
    trocaElemento(corInicial,&elemento);
    elemento.respectivaUnidade = respectivaUnidade;
    ///fim

    ///Verificando se deve trocar de elemento
        //Variáveis necessárias
    Unidade* unidadeElementoFracoContra = elemento.elementoFracoContra->inicio;
    double posicaoAtualElemento[2];
    double posicaoOutrosElementos[2];
    double dist;
    posicaoAtualElemento[0] = elemento.respectivaUnidade->dado.x;
    posicaoAtualElemento[1] = elemento.respectivaUnidade->dado.y;

        //Se houver escritor, espera
    pthread_mutex_lock(&mutexTemEscritor);
    while(temEscritor)
    {
        pthread_cond_wait(&condEscritor,&mutexTemEscritor);
    }
    pthread_mutex_unlock(&mutexTemEscritor);

        //Há mais um leitor. Se for o primeiro, bloqueie as listas
    pthread_mutex_lock(&mutexNumLeitores);
    numLeitores++;
    if(numLeitores == 1) pthread_mutex_lock(&mutexListas);
    pthread_mutex_unlock(&mutexNumLeitores);

        //Lendo a lista e utilizando seus dados
    while(unidadeElementoFracoContra != NULL)
    {
        posicaoOutrosElementos[0] = unidadeElementoFracoContra->dado.x;
        posicaoOutrosElementos[1] = unidadeElementoFracoContra->dado.y;

        dist = calcula_distancia(posicaoAtualElemento,posicaoOutrosElementos);

        //Condição de colisão entre dois círculos
        if(dist < 2*RAIO)
        {
            //Sinalizar que há um novo escritor
            pthread_mutex_lock(&mutexTemEscritor);
            temEscritor++;
            pthread_mutex_unlock(&mutexTemEscritor);

            //Hora de escrever. É necessário acesso exclusivo às listas
            pthread_mutex_lock(&mutexListas);
            trocaUnidadeListas(elemento.respectivaUnidade);
            trocaElemento(unidadeElementoFracoContra->dado.cor,&elemento);
            pthread_mutex_unlock(&mutexListas);

            //Sinalizar que deixou de ser escritor
            pthread_mutex_lock(&mutexTemEscritor);
            temEscritor--;
            pthread_mutex_unlock(&mutexTemEscritor);

            //Acordar os leitores em espera
            pthread_cond_signal(&condEscritor);

            //Pode sair. Já foi detectado uma colisão
            break;
        }

        //Vericar o próximo da lista
        unidadeElementoFracoContra = unidadeElementoFracoContra->prox;
    }
        //Há menos um leitor. Se for o último, libere as listas
    pthread_mutex_lock(&mutexNumLeitores);
    numLeitores--;
    if(numLeitores == 0) pthread_mutex_unlock(&mutexListas);
    pthread_mutex_unlock(&mutexNumLeitores);
    ///fim

    ///Calcular movimento do elemento 
}