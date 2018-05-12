/* ---------------------------------------------------------------------------------------------
UNIVERSIDADE FEDERAL DE MINAS GERAIS
Departamento de Ciência da Computação
DCC004 - Algoritmos e Estruturas de Dados II
Profs. Cristiano Arbex Valle e Gisele L. Pappa

TP1 - Segmentação de Imagens
Data de Entrega: 11/05/2018

Autor: Breno Claudio de Sena Pimenta
Graduacao em Engenharia Eletrica
Matrícula: 2017074424
------------------------------------------------------------------------------------------------
>> Objetivo:

    O programa lê uma imagem .pgm e segmenta suas regiões utilizando um algoritmo de crescimento
    de regiões.

>> Funcionamento:

    1. O programa recebe os nomes dos arquivos sem extenção por meio de passagem de argumentos
    e copia os dados da imagem de entrada (.pgm).

    2. Caso exista um arquivo de sementes auxiliares pre-definidas, o programa roda um algorit-
    mo de crescimento de regiões iterativo utilizando filas.

    3. Caso contrário, o programa ignora o algoritmo iterativo e gera sementes aleatorias para
    a imagem de entrada e realiza o crescimento das regiões por meio de algoritmo recursivo.

    4. Caso exista um arquivo auxiliar de sementes, o program ira gerar uma imagem de saida com
    tais sementes e também irá gerar outra imagem com sementes aleatorias.

    5. Ao final, toda memoria alocada dinamicamente é deslocada.
--------------------------------------------------------------------------------------------- */

/* >> BIBLIOTECAS: ---------------------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "segmentacao-imagens.h"


/* >> PROGRAMA PRINCIPAL: -------------------------------------------------------------------*/

int main (int argc, char **argv) {

    testar_alocacao(argv[1], ARQUIVO);
    srand(time(NULL));

    /* Declaracao de variaveis: */
    int num_sementes = 0;
    char nome_sementes [MAX_NOME];
    char nome_recursivo [MAX_NOME];

    /* Leitura da imagem de entrada: */
    FILE *arquivo_imagem = abrir_arquivo (argv[1], ".pgm", "r");
    tipo_imagem_pgm *imagem_entrada = armazenar_imagem_entrada (arquivo_imagem);

    /* Usar arquivo de sementes auxiliares: */
    sprintf(nome_sementes, "%s%s", argv[1], ".txt");
    FILE *arquivo_semente = fopen (nome_sementes, "r");
    if (arquivo_semente) {
        tipo_semente_pai *sementes = armazenar_sementes (arquivo_semente, &num_sementes);
        tipo_imagem_ppm *imagem_saida_iterativa = alocar_matriz_saida (imagem_entrada);
        segmentar_regioes(imagem_entrada, imagem_saida_iterativa, sementes, num_sementes);
        criar_imagem_saida (imagem_saida_iterativa, argv[1]);
        desalocar_imagem_saida (imagem_saida_iterativa);
        desalocar_sementes (sementes, num_sementes);
    }
    else {
        printf("Arquivo %s de sementes pre-definidas nao foi encontrado!\n", nome_sementes);
        printf("Sera utilizado somente sementes aleatorias.\n");
    }

    /* Usar algoritmo de semente aleatorias e recursividade: */
    tipo_imagem_ppm *imagem_saida_recursiva = alocar_matriz_saida (imagem_entrada);
    sprintf(nome_recursivo, "%s%s", argv[1], "_recursivo");
    segmentar_aleatoriamente (imagem_entrada, imagem_saida_recursiva);
    criar_imagem_saida (imagem_saida_recursiva, nome_recursivo);
    desalocar_imagem_saida (imagem_saida_recursiva);

    desalocar_imagem_entrada (imagem_entrada);

    return EXIT_SUCCESS;
}
