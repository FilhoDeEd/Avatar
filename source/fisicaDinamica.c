#include <math.h>
#include "fisicaDinamica.h"

#define consAtracao 1
#define consRepulsao 0.1
#define dt 0.0005

void calcula_forca(double forca[2], double constante, double distacia_quadrado, double distancia_horizon, double distancia_verti)
{
    forca[0]=((constante)/distacia_quadrado)*(distancia_horizon/sqrt(distacia_quadrado)); //cosseno
    forca[1]=((constante)/distacia_quadrado)*(distancia_verti/sqrt(distacia_quadrado)); //seno
}

void calcula_aceleracao(double aceleracao[2], double forca[2], double massa)
{
    aceleracao[0]= forca[0]/massa;
    aceleracao[1]= forca[1]/massa;
}

void calcula_velocidade(double velocidade[2], double aceleracao[2])
{
    velocidade[0]= velocidade[0]+aceleracao[0]*dt;
    velocidade[1]= velocidade[1]+aceleracao[1]*dt;
}

void calcula_posicao(double posicao[2], double velocidade[2])
{
    posicao[0]= posicao[0]+velocidade[0]*dt;
    posicao[1]= posicao[1]+velocidade[1]*dt;
}

double calcula_distancia(double posicao1[2], double posicao2[2])
{
    double deltaX = posicao1[0] - posicao2[0];
    double deltaY = posicao1[1] - posicao2[1];

    return sqrt(pow(deltaX,2) + pow(deltaY,2));
}
