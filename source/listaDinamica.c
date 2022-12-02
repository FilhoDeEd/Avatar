#include <stdlib.h>
#include <stdio.h>
#include "listaDinamica.h"

Ponto init_ponto(double x, double y, int ID, char cor)
{
    Ponto p;

    p.x = x;
    p.y = y;
    p.ID = ID;
    p.cor = cor;

    return p;
}

Unidade* cria_unidade(Ponto p)
{
    Unidade* un = (Unidade*) malloc(sizeof(Unidade));
    if(un != NULL)
    {
        un->dado = p;
        un->prox = NULL;
    }

    return un;
}

void libera_unidade(Unidade* uni)
{
    if(uni != NULL)
    {
        free(uni);
    }
}

Lista* cria_lista()
{
    Lista* li = (Lista*) malloc(sizeof(Lista));
    if(li != NULL)
    {
        li->inicio = NULL;
        li->final = NULL;
        li->qtd = 0;
    }

    return li;
}

void libera_lista(Lista* li)
{
    if(li != NULL)
    {
        Unidade* no;

        while((li->inicio) != NULL)
        {
            no = li->inicio;
            li->inicio = li->inicio->prox;
            free(no);
        }

        free(li);
    }
}

int tamanho_lista(Lista* li)
{
    if(li == NULL) return -1;

    return li->qtd;
}

int lista_vazia(Lista* li)
{
    if(li == NULL) return -1;

    if(li->inicio == NULL) return 1;

    return 0;
}

int insere_uni_lista_final(Lista* li, Unidade* no)
{
    if(li == NULL) return -1;

    if(li->inicio == NULL) li->inicio = no;
    else li->final->prox = no;

    li->final = no;
    (li->qtd)++;
    return 1;
}

int remove_uni_lista(Lista* li, int ID)
{
    if(li == NULL) return -1;

    Unidade *anterior, *no = li->inicio;

    while(no != NULL && no->dado.ID != ID)
    {
        anterior = no;
        no = no->prox;
    }

    if(no == NULL) return 0;

    if(no == li->inicio) li->inicio = no->prox;
    else anterior->prox = no->prox;
    
    (li->qtd)--;
    return 1;
}
