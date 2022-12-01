#include "listaDinamica.h"

#ifndef ELEMENTO_H
#define ELEMENTO_H

//Listas para armazenar a posição de todos os pontos
    Lista* ListaFogo;
    Lista* ListaAgua;
    Lista* ListaGrama;

//Controlar leitores e escritores das listas
pthread_mutex_t mutexTemEscritor;
pthread_mutex_t mutexNumLeitores;
pthread_cond_t condEscritor;
int temEscritor;
int numLeitores;

//Controlar acesso às listas
pthread_mutex_t mutexListas;

#endif /* ELEMENTO_H */