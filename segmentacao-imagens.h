/* >> BIBLIOTECAS: ---------------------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* >> CONSTANTES: ----------------------------------------------------------------------------*/

#define ARQUIVO 0 // Parametro para funcao de testar alocacao de ponteiros
#define MEMORIA 1 // Parametro para funcao de testar alocacao de ponteiros
#define MAX_NOME 260 // tamanho maximo para nome de arquivos, padrão do Windows 10

/* >> TIPOs DE DADOS: ------------------------------------------------------------------------*/

/* Objetivo: armazenar os pixels da imagem de entrada (escala de cinza) */
typedef struct tipo_pixel_cinza {
    int cor;
    bool passei;
}tipo_pixel_cinza;


/* Objetivo: armazenar os pixels da imagem de saida (cor RGB) */
typedef struct tipo_pixel_rgb {
    int R, G, B;
}tipo_pixel_rgb;


/* Objetivo: armazenar dados de uma imagem tipo .pgm (escala de cinza) */
typedef struct tipo_imagem_pgm {
    int linhas, colunas, nivel;
    struct tipo_pixel_cinza **pixels;
}tipo_imagem_pgm;


/* Objetivo: armazenar dados de uma imagem tipo .ppm (cor RGB) */
typedef struct tipo_imagem_ppm {
    int linhas, colunas, nivel;
    struct tipo_pixel_rgb **pixels;
}tipo_imagem_ppm;


/* Objetivo: armazenar informações de uma fila de região segmentada para cada semente */
typedef struct tipo_semente_pai {
    int limiar, R, G, B, num_filhos,
        linha_inicial, coluna_inicial;
    struct tipo_semente_filho *primeiro;
    struct tipo_semente_filho *ultimo;
}tipo_semente_pai;


/* Objetivo: é a célula de uma fila de sementes de uma mesma região segmentada */
typedef struct tipo_semente_filho {
    int linha, coluna;
    struct tipo_semente_filho *irmao;
}tipo_semente_filho;


/* >> FUNCOES: -------------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------------------------
Protótipo: void testar_alocacao (void *ponteiro, int tipo);
Função:    Testar se um ponteiro foi alocado corretamente ou se existe. A função interrompe
           o prgrama somente se a alocação foi mal sucedida ou se não existe o ponteiro.
Entrada:   Ponteiro generico, tipo de teste (há o tipo MEMORIA e ARQUIVO).
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void testar_alocacao (void *ponteiro, int tipo);

/* ---------------------------------------------------------------------------------------------
Protótipo: FILE *abrir_arquivo (char *nome, char *formato, char *tipo_acesso);
Função:    Abrir um arquivo.
Entrada:   Nome do arquivo, formato do arquivo, tipo de acesso conforme especificação da
           função "fopen", nativa da biblioteca stdio.h.
Saída:     Retorna um ponteiro para o arquivo aberto.
-----------------------------------------------------------------------------------------------*/
FILE *abrir_arquivo (char *nome, char *formato, char *tipo_acesso);


/* ---------------------------------------------------------------------------------------------
Protótipo: tipo_imagem_pgm *armazenar_imagem_entrada (FILE *arquivo);
Função:    Ler uma imagem tipo .pgm e armazenar em uma matriz dinamica.
Entrada:   Ponteiro do arquivo que contem a imagem .pgm.
Saída:     Retorna um ponteiro para a matriz dinamica do tipo tipo_imagem_pgm.
-----------------------------------------------------------------------------------------------*/
tipo_imagem_pgm *armazenar_imagem_entrada (FILE *arquivo);


/* ---------------------------------------------------------------------------------------------
Protótipo: tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes);
Função:    Ler um arquivo que contem sementes, salvar quantas sementes existem, e armazená-las
           em um vetor de structs de tipo tipo_semente_pai.
Entrada:   Ponteiro do arquivo que contem as sementes e ponteiro para o numero de sementes.
Saída:     Retorna um ponteiro para o vetor dinamico do tipo tipo_semente_pai
-----------------------------------------------------------------------------------------------*/
tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes);


/* ---------------------------------------------------------------------------------------------
Protótipo: tipo_imagem_ppm *criar_matriz_saida (tipo_imagem_pgm *imagem_entrada);
Função:    Criar uma matriz dinamica que irá conter as informações de uma imagem tipo .ppm. Por
           padrão a imagem de saida é preenchida com os pixels em escala de cinza da matriz
           de entrada.
Entrada:   Ponteiro para a matriz dinamica que contem a imagem de entrada (.pgm).
Saída:     Retorna um ponteiro para a matriz dinamica do tipo tipo_imagem_ppm.
-----------------------------------------------------------------------------------------------*/
tipo_imagem_ppm *alocar_matriz_saida (tipo_imagem_pgm *imagem_entrada);


/* ---------------------------------------------------------------------------------------------
Protótipo: void desalocar_imagem_entrada (tipo_imagem_pgm *imagem);
Função:    Desalocar uma matriz dinamica de imagem de entrada.
Entrada:   Ponteiro para a matriz dinamica que contem imagem de entrada tipo_imagem_pgm.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void desalocar_imagem_entrada (tipo_imagem_pgm *imagem);


/* ---------------------------------------------------------------------------------------------
Protótipo: void desalocar_imagem_saida (tipo_imagem_ppm *imagem);
Função:    Desalocar uma matriz dinamica de imagem de saída.
Entrada:   Ponteiro para a matriz dinamica que contem imagem de saida tipo_imagem_ppm.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void desalocar_imagem_saida (tipo_imagem_ppm *imagem);


/* ---------------------------------------------------------------------------------------------
Protótipo: void desalocar_sementes (tipo_semente_pai *pai, int num_sementes);
Função:    Desalocar vetor dinamico que contem as sementes. Primeiramente testa se a semente
           pai possui algum filho e desaloca um a um, em seguida desaloca o vetor.
Entrada:   Ponteiro para o vetor dinamico tipo_semente_pai, numero de sementes.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void desalocar_sementes (tipo_semente_pai *sementes, int num_sementes);


/* ---------------------------------------------------------------------------------------------
Protótipo: void enfileirar (tipo_imagem_pgm *imagem, tipo_semente_pai *pai, int i, int j);
Função:    Colocar uma semente em uma fila para ser processada (testar se a sua vizinhança faz
           parte da mesma região).
Entrada:   Ponteiro para a matriz dinamica que contem a imagem de entrada, ponteiro para o vetor
           dinamico tipo_semente_pai, coordenadas da semente na matriz de imagem.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void enfileirar (tipo_imagem_pgm *imagem, tipo_semente_pai *pai, int i, int j);


/* ---------------------------------------------------------------------------------------------
Protótipo: bool verificar_condicoes (tipo_imagem_pgm *imagem, int cor, int i, int j);
Função:    Realizar séries de condições para verificar se um vizinho é parte de uma mesma região
Entrada:   Ponteiro para a imagem de entrada, valor do limiar da semente pai, valor da cor de
           comparação, numero da linha e da coluna.
Saída:     A função retorna um valor boleano para habilitar o enfileiramento da semente.
-----------------------------------------------------------------------------------------------*/
bool verificar_condicoes (tipo_imagem_pgm *imagem, int limiar, int cor, int i, int j);


/* ---------------------------------------------------------------------------------------------
Protótipo: testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai);
Função:    Testar a vizinhança 4-conexa de um pixel para determinar se haverá a expanção
           do grupo. Se a diferença de intensidade entre dois pixels vizinhos for pequena,
           o grupo se propaga.
Entrada:   Ponteiro para a imagem de entrada, ponteiro para o vetor de sementes pai.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai);


/* ---------------------------------------------------------------------------------------------
Protótipo: void definir_posicao_visitada (tipo_imagem_pgm *imagem_entrada, int i, int j);
Função:    Marcar na imagem de entrada que determinada posição P(i,j) já foi visitada, ou seja,
           o pixel P(i,j) já está ligado a uma determinada região.
Entrada:   Ponteiro para a imagem de entrada, coordenadas da semente na matriz de imagem..
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void definir_posicao_visitada (tipo_imagem_pgm *imagem_entrada, int i, int j);


/* ---------------------------------------------------------------------------------------------
Protótipo: void coloriar_imagem_saida (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai);
Função:    Colorir a imagem de saaída com os dados de cores da semente pai.
Entrada:   Ponteiro para a imagem de saida, ponteiro para o vetor de sementes pai.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void coloriar_imagem_saida (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai);


/* ---------------------------------------------------------------------------------------------
Protótipo: void desenfileirar_semente (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai);
Função:    Desalocar o primeiro elemento da fila de sementes (o primeiro filho).
Entrada:   Ponteiro para a imagem de saida, ponteiro para o vetor de sementes pai.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void desenfileirar (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai);


/* ---------------------------------------------------------------------------------------------
Protótipo: void resetar_matriz_passei (tipo_imagem_pgm *imagem);
Função:    Redefinir o verificador "passei" de todos os pixels da matriz dinamica que contém a
           imagem de entrada (.pgm) para false.
Entrada:   Ponteiro para a imagem de entrada.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void resetar_matriz_passei (tipo_imagem_pgm *imagem);

/* ---------------------------------------------------------------------------------------------
Protótipo: void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm*
           imagem_saida, tipo_semente_pai *pai, int num_sementes);
Função:    Segmentar a imagem de entrada a partir de uma semente inicial.  A função no inicio do
           ciclo salva a semente de partida como o primeiro filho, nesse momento o pai só tem um
           filho. Em seguida, a função testar_vizinhos é chamada e depois a desenfileirar já que
           já foi analisado o primeiro ocupante da fila. A matriz de posições passadas é reseta-
           da a fim de que se houver mais de uma semente para a mesma região,  a semente que irá
           prevalecer seja a última.
Entrada:   Ponteiro para a imagem de entrada tipo_imagem_pgm, ponteiro para a imagem de entrada,
           ponteiro para o vetor de sementes pai tipo_semente_pai.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida,
                        tipo_semente_pai *sementes, int num_sementes);


/* ---------------------------------------------------------------------------------------------
Protótipo: void criar_imagem_saida (tipo_imagem_ppm *imagem, char *nome_arquivo);
Função:    criar um arquivo de imagem com as regiões segmentadas.
Entrada:   Ponteiro para a imagem de saída tipo_imagem_ppm e o nome desejado do arquivo.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void criar_imagem_saida (tipo_imagem_ppm *imagem, char *nome_arquivo);


/* >> FUNCOES SEMENTES ALEATORIAS: ------------------------------------------------------------*/


/* ---------------------------------------------------------------------------------------------
Protótipo: tipo_semente_pai gerar_pai_aleatorio (int i, int j);
Função:    Criar um pai de posição P(i,j) pré-definidos com cores aletarias para iniciar uma re-
           giçao a ser segmentada.
Entrada:   Linha e coluna iniciais.
Saída:     A função retorna um ponteiro para o pai criado.
-----------------------------------------------------------------------------------------------*/
tipo_semente_pai* gerar_pai_aleatorio (int i, int j);


/* ---------------------------------------------------------------------------------------------
Protótipo: void segmentar_aleatoriamente (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm*
           imagem_saida, tipo_semente_pai *pai, int *num_sementes);
Função:    Segmentar a imagem de entrada a partir de uma semente inicial de cor aleatoria e
           posição inicial P(0,0). A partir da segmentação do primeiro ponto P, a função procura
           outro ponto que ainda não foi segmentado para iniciar outra região.
Entrada:   Ponteiro para a imagem de entrada tipo_imagem_pgm, ponteiro para a imagem de entrada,
           ponteiro para o vetor de sementes pai tipo_semente_pai, e o identificador do pai que
           está sendo tratado.
Saída:     A função não retorna valores.
-----------------------------------------------------------------------------------------------*/
void segmentar_aleatoriamente (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida,
                               tipo_semente_pai *pai, int *num_sementes);


/* ---------------------------------------------------------------------------------------------
Protótipo: bool sementes_aleatorias ();
Função:    Verificar com o usuario se ele deseja executar o programa com sementes aleatorias ou
           utilizando o arquivo de sementes pré-definidas.
Entrada:   A função não possui parametros de entrada.
Saída:     A função retorna true caso o usuario queira utilizar sementes aleatorias ou false se
           deseja utilizar sementes pre-definidas. Caso o usuario pressione tecla incorreta, a
           função retorna ela mesma para que alguma tecla correta possa ser pressionada.
-----------------------------------------------------------------------------------------------*/
bool usar_sementes_aleatorias ();


/* ---------------------------------------------------------------------------------------------
Protótipo: bool usar_recurvidade ();
Função:    Verificar com o usuario se deseja executar o programa utilizar o algoritmo de segmen-
           recursivo ou iterativo.
Entrada:   A função não possui parametros de entrada.
Saída:     A função retorna true caso o usuario queira utilizar algoritmo recursivo ou false se
           deseja utilizar o iterativo.  Caso o usuario pressione tecla incorreta, a função re-
           torna ela mesma para que alguma tecla correta possa ser pressionada.
-----------------------------------------------------------------------------------------------*/
bool usar_recurvidade ();
