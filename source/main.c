#include <stdio.h>
#include <stdlib.h>
#include "elemento.h"

#define NE 3 //Aumente em 1 para cada erro adicionado no enum
enum erros {EFOGO=-NE,EAGUA,EGRAMA};

int main()
{  
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

    return 0;
}