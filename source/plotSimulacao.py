import pandas as pd
import matplotlib.pyplot as plt

LARGURA = 1000 
ALTURA = 800
NUMTHREADS = 12

arqRastro = open('source/rastrosElementos.csv','r')

#Estrutura para armazenar os rastros

rastros: list[list[tuple]] = []
for i in range(NUMTHREADS):
    rastros.append([])

#Cada linha do arquivo como uma tupla
for linha in arqRastro:

    linhaSplit = linha.split(',')
    linhaSplit[3] = linhaSplit[3].replace('\n','0')
    ponto = (linhaSplit[1],float(linhaSplit[2]),float(linhaSplit[3]))
    rastros[int(linhaSplit[0])].append(ponto)

arqRastro.close()

#Data frame para armazernar os rastros
df_rastros = pd.DataFrame(rastros)
df_rastros = df_rastros.transpose()

print(df_rastros)
