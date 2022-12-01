#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

typedef struct ponto
{
    int ID;
    char cor;
    double x;
    double y;
}Ponto;

typedef struct unidade
{
    Ponto dado;
    struct unidade* prox;
}Unidade;

typedef struct lista
{
    Unidade* inicio;
    Unidade* final;
    int qtd;
}Lista;

//Inicializa um ponto
Ponto init_ponto(double x, double y, int ID, char cor);

//Cria uma unidade da lista
Unidade* cria_unidade(Ponto p);

//Exclui uma unidade (remova da lista antes de excluir)
void libera_unidade(Unidade* un);

//Cria a lista
Lista* cria_lista();

//Exclui a lista
void libera_lista(Lista* li);

//Retorna o número de elementos na lista. Caso a lista seja inválida, retorna -1
int tamanho_lista(Lista* li);

//Retorna se a lista está vazia ou não. Caso a lista seja inválida, retorna -1
int lista_vazia(Lista* li);

//Insere um elemento ao final da lista e retorna 1 se a remoção ocorreu com êxito.
//Caso a lista seja inválida, retorna -1
int insere_uni_lista_final(Lista* li, Unidade* no);

//Remove um elemento da lista com base no ID e retorna 1 se a remoção ocorreu com 
//êxito ou 0 se falhou. Caso a lista seja inválida, retorna -1
int remove_uni_lista(Lista* li, int ID);

#endif /* LISTADINAMICA_H */