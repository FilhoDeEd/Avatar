#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "elemento.h"
#include "fisicaDinamica.h"

//Algumas constantes do movimento
#define RAIO 75.0
#define MASSA 1
#define ATRACAO 100.0
#define REPULSAO 0.0
#define dt 0.1

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

void entraLeitor()
{
    pthread_mutex_lock(&mutexGuardaDownLeitores); //Apenas uma thread esperando em mutexTemEscritor
        pthread_mutex_lock(&mutexTemEscritor); //Se tem leitor, espera
            pthread_mutex_lock(&mutexNumLeitores);
                numLeitores++;
                if(numLeitores == 1) pthread_mutex_lock(&mutexListas); //Se for o 1º, bloqueia listas
            pthread_mutex_unlock(&mutexNumLeitores);
        pthread_mutex_unlock(&mutexTemEscritor);
    pthread_mutex_unlock(&mutexGuardaDownLeitores);
}

void saiLeitor()
{
    pthread_mutex_lock(&mutexNumLeitores);
        numLeitores--;
        if(numLeitores == 0) pthread_mutex_unlock(&mutexListas); //Se for o último, libera listas
    pthread_mutex_unlock(&mutexNumLeitores);
}

void entraEscritor()
{
    pthread_mutex_lock(&mutexNumEscritores);
        numEscritores++;
        if(numEscritores == 1) pthread_mutex_lock(&mutexTemEscritor); //Se for o 1º, bloqueia novos leitores
    pthread_mutex_unlock(&mutexNumEscritores);
    pthread_mutex_lock(&mutexListas);
}

void saiEscritor()
{
    pthread_mutex_unlock(&mutexListas);
    pthread_mutex_lock(&mutexNumEscritores);
        numEscritores--;
        if(numEscritores == 0) pthread_mutex_unlock(&mutexTemEscritor); //Se for o último, libera novos leitores
    pthread_mutex_unlock(&mutexNumEscritores);
}

void* thrElemento(void* argsThrElemento)
{
    //Argumento
    Unidade* respectivaUnidade = (Unidade*) argsThrElemento;
    char corInicial = respectivaUnidade->dado.cor;

    //Declarando e inicializando elemento
    Elemento elemento;
    elemento.respectivaUnidade = respectivaUnidade;
    trocaElemento(corInicial,&elemento);

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
    double X;
    double Y;

    //-----------------------------------------------------------------------------------//

    //Esperando a thread monitora dar largada
    while(largada == 0);

    //Repita até a monitora interromper a simulação
    do
    {
        pthread_mutex_lock(&mutexContador);
        contador++;
        pthread_mutex_unlock(&mutexContador);

        //----------------------Escreve instância do ponto no arquivo----------------------//

            //Acesso exclusivo para escrever no arquivo
        Ponto pt = elemento.respectivaUnidade->dado;
        pthread_mutex_lock(&mutexArquivo);
        fseek(arqRastro,0,SEEK_END);
        fprintf(arqRastro,"%i,%c,%.5f,%.5f\n",pt.ID,pt.cor,pt.x,pt.y);
        pthread_mutex_unlock(&mutexArquivo);

        //----------------------Verificar colisão entre elementos----------------------//

        entraLeitor();

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
                saiLeitor();
                entraEscritor();
                trocaUnidadeListas(elemento.respectivaUnidade);
                trocaElemento(unidadeElementoFracoContra->dado.cor,&elemento);
                saiEscritor();

                //Pode sair. Já foi detectado uma colisão
                break;
            }

            //Vericar o próximo da lista
            unidadeElementoFracoContra = unidadeElementoFracoContra->prox;
        }

        if(unidadeElementoFracoContra == NULL) saiLeitor();

        //----------------------Calcular movimento do elemento----------------------//
        
            //Inicializando variáveis para calcular o movimento do elemento
        forcaResultante[0] = 0.0;
        forcaResultante[1] = 0.0;

        entraLeitor();

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

        saiLeitor();

            //Calculando aceleração e atualizando velocidade e posição
        calcula_aceleracao(aceleracao,forcaResultante,MASSA);
        calcula_velocidade(velocidadeAtualElemento,aceleracao,dt);
        calcula_posicao(posicaoAtualElemento,velocidadeAtualElemento,dt);

            //Atualizando a posição na respectiva unidade
        elemento.respectivaUnidade->dado.x = posicaoAtualElemento[0];
        elemento.respectivaUnidade->dado.y = posicaoAtualElemento[1];

        //----------------------Verificar se partícula saiu da área de simulação----------------------//

            //condição para ter saído da área de simulação
            
            X = posicaoAtualElemento[0];
            Y = posicaoAtualElemento[1];
            if(X > LARGURA_TELA || Y > ALTURA_TELA || X < 0 || Y < 0)
            {
                entraEscritor();
                destruirUnidade(elemento.respectivaUnidade);
                saiEscritor();

                //Pode sair. Acabou para essa thread
                pthread_exit(NULL);
            }

    }while(pararSimulacao == 0);

    //----------------------Destruindo respectiva unidade----------------------//

    entraEscritor();
    destruirUnidade(elemento.respectivaUnidade);
    saiEscritor();

    //Pode sair. Acabou para essa thread
    pthread_exit(NULL);
}

void* thrMonitora(void* args)
{
    //Variáveis locais
    int fogoPerdeu = 0;
    int aguaPerdeu = 0;
    int gramaPerdeu = 0;
    int qtdPerdedores = 0;

    //Dar largada as threads principais
    largada = 1;

    //Verificar se deve interromper simulação
    while(1)
    {
        if(!fogoPerdeu && lista_vazia(ListaFogo))
        {
            fogoPerdeu = 1;
            qtdPerdedores++;
        }

        if(!aguaPerdeu && lista_vazia(ListaAgua))
        {
            aguaPerdeu = 1;
            qtdPerdedores++;
        }

        if(!gramaPerdeu && lista_vazia(ListaGrama))
        {
            gramaPerdeu = 1;
            qtdPerdedores++;
        }

        if(qtdPerdedores == 2)
        {
            pararSimulacao = 1;
            break;
        }
    }

    printf("Fogo Perdeu = %i\n",fogoPerdeu);
    printf("Agua Perdeu = %i\n",aguaPerdeu);
    printf("Grama Perdeu = %i\n",gramaPerdeu);
    printf("Contador = %i",contador);

    pthread_exit(NULL);
}
