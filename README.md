# Segmentação de Imagens
Trabalho prático para a disciplina Algoritmos e Estruturas de Dados II do curso de Engenharia Elétrica da Universidade Federal de Minas Gerais (UFMG).

## 1 Introdução
Neste TP, será implementado um algoritmo de segmentação denominado algoritmo de crescimento de regiões. Este algoritmo define um grupo a partir de um pixel aleatório inicial, denominado semente, e expande o grupo para os vizinhos do pixel semente de acordo com um critério baseado na intensidade do pixel. Para um dado pixel P(u, v), sua vizinhança 4-conexa é dada pelos pixels _P(u, v + 1)_, _P(u + 1, v)_, _P(u, v - 1)_ e _P(u - 1, v)_. Se a diferença de intensidade entre dois pixels vizinhos for pequena, o grupo se propaga. Por exemplo, se |_P(u, v) - P(u; v+1)_| _≤_  _T_, então (_u, v_) e (_u, v + 1_) pertencem ao mesmo grupo (região), no qual _T_ é um valor de limiar predefinido. 

## 2 Implementação
De maneira geral, existem dois principais algoritmos para gerar as imagens de saída: um interativo com sementes pré-definidas e outro recursivo e com sementes aleatórias.

### 2.1 Programa Principal
O programa possui duas possibilidades de execução correta. **Primeira**: caso exista um arquivo de sementes pré-definidas, serão geradas duas saídas uma que utiliza o arquivo de semente auxiliar e outra saída utilizando sementes aleatórias e o algoritmo recursivo. **Segunda**: caso não exista um arquivo de sementes auxiliares, será gerado somente uma imagem com as sementes aleatórias.

### 2.2 Organização do Código
O código foi dividido em três arquivos, sendo eles:

* main.c: contém as diretrizes principais do programa.
* segmentacao-imagens.c: contém a definição das funções iterativas e recursivas.
* segmentacao-imagens.h: contém os cabeçalhos das funções iterativas e recursivas.

Para o algoritmo de segmentação iterativa, caso duas sementes pré-definidas estejam na mesma região, a cor que irá prevalecer é a cor da última semente. Porém, no algoritmo recursivo, caso duas sementes aleatórias pertençam à mesma região, a cor que irá prevalecer é a da primeira semente. Isso se deve ao fato do algoritmo recursivo ser mais propenso a causar stack overflow, então a execução do algoritmo deve ser minimizada ao mínimo necessário.

## 3 Resultados

_**FIGURA 1:** Utilizando a imagem brasil. Da esquerda para direita: imagem original, imagem gerada com sementes pré-definidas, imagem gerada com sementes aleatórias_
![alt text](https://github.com/brenocsp/image-segmentation/blob/master/images_jpg/comparacao-01.png)

_**FIGURA 2:** Utilizando a imagem amazon_river. Da esquerda para direita: imagem original, imagem gerada com sementes pré-definidas, imagem gerada com sementes aleatórias_
![alt text](https://github.com/brenocsp/image-segmentation/blob/master/images_jpg/comparacao-02.png)

_**FIGURA 3:** Utilizando a imagem split_2. Da esquerda para direita: imagem original, imagem gerada com sementes pré-definidas, imagem gerada com sementes aleatórias_
![alt text](https://github.com/brenocsp/image-segmentation/blob/master/images_jpg/comparacao-03.png)

_**FIGURA 4:** Utilizando a imagem vessels. Da esquerda para direita: imagem original, imagem gerada com sementes pré-definidas, imagem gerada com sementes aleatórias_
![alt text](https://github.com/brenocsp/image-segmentation/blob/master/images_jpg/comparacao-04.png)

## 5 Conclusão
A implementação do trabalho foi bem sucedida. A opção de utilizar fila foi por maior similaridade à lógica elaborada no planejamento do trabalho. Durante os teste foi percebido que a lógica de testar os seis vizinhos de um pixel trouxe melhores resultados quando comparado ao teste de somente quatro. No código foram mantidas quatro linhas comentadas que podem ser usadas para usar o algoritmo para testar os vizinhos das diagonais. O algoritmo recursivo não é a melhor opção de solução para esse problem, vixto que é instável e pode ocasionar stack overflow em máquinas menos robustas devido à limitação do tamanho da memória stack. O algoritmo iterativo que utiliza sementes pré-definidas gerou imagens dentro dos parâmetros. Além disso, o algoritmo recursivo que utiliza sementes aleatórias, mesmo utilizando um número alto de sementes quando comparado ao arquivo auxiliar, às vezes deixou de segmentar regiões de interesse na imagem. Isso ocorre devido à probabilidade de encontrar todas as regiões relevantes ser baixa.
