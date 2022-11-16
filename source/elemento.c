#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "elemento.h"
#include "fisicaDinamica.h"

typedef struct elemento
{
    char tipo;
    Lista* elementoForteContra;
    Lista* elementoFracoContra;
    Unidade* respectivaUnidade;

}Elemento;

void* thrElemento(void* args)
{

}