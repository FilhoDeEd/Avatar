#ifndef FISICADINAMICA_H
#define FISICADINAMICA_H

void calcula_forca(double forca[2], double constante, double distacia_quadrado, double distancia_horizon, double distancia_verti);

void calcula_aceleracao(double aceleracao[2], double forca[2], double massa);

void calcula_velocidade(double velocidade[2], double aceleracao[2]);

void calcula_posicao(double posicao[2], double velocidade[2]);

//Retorna a distância entre dois pontos
double calcula_distancia(double posicao1[2], double posicao2[2]);

#endif /* FISICADINAMICA_H */