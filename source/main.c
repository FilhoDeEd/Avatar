#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "elemento.h"

#define NE 4 //Aumente em 1 para cada erro adicionado no enum
enum erros {EFOGO=-NE,EAGUA,EGRAMA,EFILE};

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

    //Inicializando mutexes, cond e variáveis globais para controlar leitores e escritores
    pthread_mutex_init(&mutexTemEscritor,NULL);
    pthread_mutex_init(&mutexNumLeitores,NULL);
    pthread_cond_init(&condEscritor,NULL);
    temEscritor = 0;
    numLeitores = 0;

    //Inicializando mutex para controlar acesso às listas
    pthread_mutex_init(&mutexListas,NULL);

    //Inicializando mutex para controlar acesso ao arquivo de rastro
    pthread_mutex_init(&mutexArquivo,NULL);

    //Inicializando mutex, cond e flags para controlar threads via monitora
    pthread_mutex_init(&mutexLargada,NULL);
    pthread_cond_init(&condLargada,NULL);
    largada = 0;
    pararSimulacao = 0;

    //Abrindo o arquivo
    arqRastro = fopen("rastrosElementos.csv","w");
    if(arqRastro == NULL)
    {
        printf("Erro ao abrir 'rastrosElementos.csv'.\n");
        exit(EFILE);
    }

    //Setando uma seed com base no tempo
    srand(time(NULL));

    //Inicializando as threads principais em posições iniciais aleatórias
    Ponto randPonto;
    int i;

    for(i = 0; i < NUMTHREADS/3; i++)
    {
        randPonto.ID = i;
        randPonto.cor = FOGO;
        //Primeiro quadrante
        randPonto.x = (float)rand()/(float)(RAND_MAX/LARGURA_TELA);
        randPonto.y = (float)rand()/(float)(RAND_MAX/ALTURA_TELA);
        pthread_create(&threadsElementos[i],NULL,thrElemento,(void*)cria_unidade(randPonto));
    }

    for(i = NUMTHREADS/3; i < 2*NUMTHREADS/3; i++)
    {
        randPonto.ID = i;
        randPonto.cor = AGUA;
        //Segundo quadrante
        randPonto.x = (-1.0)*(float)rand()/(float)(RAND_MAX/LARGURA_TELA);
        randPonto.y = (float)rand()/(float)(RAND_MAX/ALTURA_TELA);
        pthread_create(&threadsElementos[i],NULL,thrElemento,(void*)cria_unidade(randPonto));
    }

    for(i = 2*NUMTHREADS/3; i < NUMTHREADS; i++)
    {
        randPonto.ID = i;
        randPonto.cor = AGUA;
        //Terceiro quadrante
        randPonto.x = (-1.0)*(float)rand()/(float)(RAND_MAX/LARGURA_TELA);
        randPonto.y = (-1.0)*(float)rand()/(float)(RAND_MAX/ALTURA_TELA);
        pthread_create(&threadsElementos[i],NULL,thrElemento,(void*)cria_unidade(randPonto));
    }

    //Chamando join para cada uma das threads principais
    for(i = 0; i < NUMTHREADS; i++) pthread_join(threadsElementos[i],NULL);
    
    //Criando a thread monitora
    

    //Fechando o arquivo
    fclose(arqRastro);

    return 0;
}