#include "listaDinamica.h"

#ifndef ELEMENTO_H
#define ELEMENTO_H

//Constantes para cor
#define FOGO 'f'
#define AGUA 'a'
#define GRAMA 'g'

//Constantes para dimensões da área de simulação
#define LARGURA_TELA 1000
#define ALTURA_TELA 800

//Numero de threads
#define NUMTHREADS 30

//Listas para armazenar a posição de todos os pontos
Lista* ListaFogo;
Lista* ListaAgua;
Lista* ListaGrama;

//Declaração do arquivo de rastro
FILE* arqRastro;

//Controlar leitores e escritores das listas
pthread_mutex_t mutexTemEscritor;
pthread_mutex_t mutexNumLeitores;
pthread_cond_t condEscritor;
int temEscritor;
int numLeitores;

//Controlar acesso às listas
pthread_mutex_t mutexListas;

//Controlar acesso ao arquivo
pthread_mutex_t mutexArquivo;

//Controlar tempo de execução das threads principais
pthread_mutex_t mutexLargada;
pthread_cond_t condLargada;
int largada;
int pararSimulacao;

//Rotina principal da simulação
void* thrElemento(void* argsThrElemento);

//Rotina para monitorar e controlar a simulação
void* thrMonitora(void* args);

#endif /* ELEMENTO_H */