#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "elemento.h"
#include "fisicaDinamica.h"

//Algumas constantes do movimento
#define RAIO 10.0
#define MASSA 5.0
#define ATRACAO 1.0
#define REPULSAO -0.1
#define dt 0.042

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

//Crítico. Remove e libera uma unidade com base na cor e no ID
void destruirUnidade(Unidade *uni)
{
    int ID = uni->dado.ID;
    char cor = uni->dado.cor;

    switch(cor)
    {
        case FOGO:
                remove_uni_lista(ListaFogo,ID);
                libera_unidade(uni);
            break;
        case AGUA:
                remove_uni_lista(ListaAgua,ID);
                libera_unidade(uni);
            break;
        case GRAMA:
                remove_uni_lista(ListaGrama,ID);
                libera_unidade(uni);
            break;
    }
}

void* thrElemento(void* argsThrElemento)
{
    //Argumento
    Unidade* respectivaUnidade = (Unidade*) argsThrElemento;
    char corInicial = respectivaUnidade->dado.cor;

    //Declarando e inicializando elemento
    Elemento elemento;
    trocaElemento(corInicial,&elemento);
    elemento.respectivaUnidade = respectivaUnidade;

    //Variáveis locais
    Unidade* unidadeElementoFracoContra;
    Unidade* unidadeElementoForteContra;
    double posicaoAtualElemento[2];
        posicaoAtualElemento[0] = elemento.respectivaUnidade->dado.x;
        posicaoAtualElemento[1] = elemento.respectivaUnidade->dado.y;
    double velocidadeAtualElemento[2];
        velocidadeAtualElemento[0] = 0.0;
        velocidadeAtualElemento[1] = 0.0;
    double posicaoOutrosElementos[2];
    double dist;
    double dist_horizon;
    double dist_verti;
    double forca[2];
    double forcaResultante[2];
    double aceleracao[2];
    double moduloX;
    double moduloY;

    //-----------------------------------------------------------------------------------//

    //Repita até a monitora interromper a simulação
    while(!pararSimulacao)
    {
        //Esperando a thread monitora dar largada
        pthread_mutex_lock(&mutexLargada);
        while(!largada)
        {
            pthread_cond_wait(&condLargada,&mutexLargada);
        }
        pthread_mutex_unlock(&mutexLargada);

        //----------------------Verificar colisão entre elementos----------------------//

            //Se houver escritor, espere
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
        unidadeElementoFracoContra = elemento.elementoFracoContra->inicio;
        while(unidadeElementoFracoContra != NULL)
        {
            posicaoOutrosElementos[0] = unidadeElementoFracoContra->dado.x;
            posicaoOutrosElementos[1] = unidadeElementoFracoContra->dado.y;

            dist = calcula_distancia(posicaoAtualElemento,posicaoOutrosElementos);

            //Condição de colisão entre dois círculos
            if(dist < 2*RAIO)
            {   
                //Há menos um leitor. Se for o último, libere as listas
                pthread_mutex_lock(&mutexNumLeitores);
                numLeitores--;
                if(numLeitores == 0) pthread_mutex_unlock(&mutexListas);
                pthread_mutex_unlock(&mutexNumLeitores);

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

        if(unidadeElementoFracoContra == NULL)
        {
            //Há menos um leitor. Se for o último, libere as listas
            pthread_mutex_lock(&mutexNumLeitores);
            numLeitores--;
            if(numLeitores == 0) pthread_mutex_unlock(&mutexListas);
            pthread_mutex_unlock(&mutexNumLeitores);
        }

        //----------------------Calcular movimento do elemento----------------------//
        
            //Inicializando variáveis para calcular o movimento do elemento
        forcaResultante[0] = 0.0;
        forcaResultante[1] = 0.0; 

            //Se houver escritor, espere
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

            //Lendo a listaForteContra e utilizando seus dados
        unidadeElementoForteContra = elemento.elementoForteContra->inicio;
        while(unidadeElementoForteContra != NULL)
        {
            //Pegando posição de uma partícula que o elemento é forte contra
            posicaoOutrosElementos[0] = unidadeElementoForteContra->dado.x;
            posicaoOutrosElementos[1] = unidadeElementoForteContra->dado.y;

            //Calculando distâncias
            dist_horizon = posicaoOutrosElementos[0] - posicaoAtualElemento[0];
            dist_verti = posicaoOutrosElementos[1] - posicaoAtualElemento[1];
            dist = calcula_distancia(posicaoAtualElemento,posicaoOutrosElementos);
            
            //Calculando força de atração resultante
            calcula_forca(forca,ATRACAO,dist,dist_horizon,dist_verti);
            forcaResultante[0] += forca[0];
            forcaResultante[1] += forca[1];

            //Calcular com próximo da lista
            unidadeElementoForteContra = unidadeElementoForteContra->prox;
        }

            //Lendo a listaFracoContra e utilizando seus dados
        unidadeElementoFracoContra = elemento.elementoFracoContra->inicio;
        while(unidadeElementoFracoContra != NULL)
        {
            //Pegando posição de uma partícula que o elemento é forte contra
            posicaoOutrosElementos[0] = unidadeElementoFracoContra->dado.x;
            posicaoOutrosElementos[1] = unidadeElementoFracoContra->dado.y;

            //Calculando distâncias
            dist_horizon = posicaoOutrosElementos[0] - posicaoAtualElemento[0];
            dist_verti = posicaoOutrosElementos[1] - posicaoAtualElemento[1];
            dist = calcula_distancia(posicaoAtualElemento,posicaoOutrosElementos);
            
            //Calculando força resultante final
            calcula_forca(forca,REPULSAO,dist,dist_horizon,dist_verti);
            forcaResultante[0] += forca[0];
            forcaResultante[1] += forca[1];

            //Calcular com próximo da lista
            unidadeElementoFracoContra = unidadeElementoFracoContra->prox;
        }

            //Há menos um leitor. Se for o último, libere as listas
        pthread_mutex_lock(&mutexNumLeitores);
        numLeitores--;
        if(numLeitores == 0) pthread_mutex_unlock(&mutexListas);
        pthread_mutex_unlock(&mutexNumLeitores);

            //Calculando aceleração e atualizando velocidade e posição
        calcula_aceleracao(aceleracao,forcaResultante,MASSA);
        calcula_velocidade(velocidadeAtualElemento,aceleracao,dt);
        calcula_posicao(posicaoAtualElemento,velocidadeAtualElemento,dt);

            //Atualizando a posição na respectiva unidade
        elemento.respectivaUnidade->dado.x = posicaoAtualElemento[0];
        elemento.respectivaUnidade->dado.y = posicaoAtualElemento[1];

        //----------------------Verificar se partícula saiu da área de simulação----------------------//

            //condição para ter saído da área de simulação
            moduloX = modulo(posicaoAtualElemento[0]);
            moduloY = modulo(posicaoAtualElemento[1]);
            if(moduloX > LARGURA_TELA || moduloY > ALTURA_TELA)
            {
                //Sinalizar que há um novo escritor
                pthread_mutex_lock(&mutexTemEscritor);
                temEscritor++;
                pthread_mutex_unlock(&mutexTemEscritor);

                //Remover e liberar unidade da thread. É necessário acesso exclusivo às lisats
                pthread_mutex_lock(&mutexListas);
                destruirUnidade(elemento.respectivaUnidade);
                pthread_mutex_unlock(&mutexListas);

                //Sinalizar que deixou de ser escritor
                pthread_mutex_lock(&mutexTemEscritor);
                temEscritor--;
                pthread_mutex_unlock(&mutexTemEscritor);

                //Acordar os leitores em espera
                pthread_cond_signal(&condEscritor);

                //Pode sair. Acabou para essa thread
                pthread_exit(NULL);
            }

        //----------------------Escreve instância do ponto no arquivo----------------------//

            //Acesso exclusivo para escrever no arquivo
        pthread_mutex_lock(&mutexArquivo);
        fseek(arqRastro,0,SEEK_END);
        Ponto pt = elemento.respectivaUnidade->dado;
        int codeCor;
        switch (pt.cor)
        {
            case FOGO:
                    codeCor = 0;
                break;
            case AGUA:
                    codeCor = 1;
                break;
            case GRAMA:
                    codeCor = 2;
                break;
        }
        fprintf(arqRastro,"%i,%i,%.5f,%.5f,",pt.ID,codeCor,pt.x,pt.y);
        pthread_mutex_unlock(&mutexArquivo);
    }

    pthread_exit(NULL);
}

void* thrMonitora(void* args)
{

}