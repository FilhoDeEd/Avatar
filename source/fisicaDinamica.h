#ifndef FISICADINAMICA_H
#define FISICADINAMICA_H

//Calcula força entre duas partículas
void calcula_forca(double forca[2], double constante, double distacia_quadrado, double distancia_horizon, double distancia_verti);

//Calcula aceleração atual da partícula
void calcula_aceleracao(double aceleracao[2], double forca[2], double massa);

//Atualiza velocidade da partícula
void calcula_velocidade(double velocidade[2], double aceleracao[2], double dt);

//Atualiza posição da partícula
void calcula_posicao(double posicao[2], double velocidade[2], double dt);

//Retorna a distância entre dois pontos
double calcula_distancia(double posicao1[2], double posicao2[2]);

//Retorna o modulo de valor
double modulo(double valor);

#endif /* FISICADINAMICA_H */