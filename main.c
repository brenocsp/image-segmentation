/* ---------------------------------------------------------------------------------------------
UNIVERSIDADE FEDERAL DE MINAS GERAIS
Departamento de Ciência da Computação
DCC004 - Algoritmos e Estruturas de Dados II
Profs. Cristiano Arbex Valle e Gisele L. Pappa

TP1 - Segmentação de Imagens
Data de Entrega: 06/05/2018

Autor: Breno Claudio de Sena Pimenta
Graduacao em Engenharia Eletrica
Matrícula: 2017074424
------------------------------------------------------------------------------------------------
>> Objetivo:

    O programa lê uma imagem .pgm e segmenta suas regiões utilizando um algoritmo de crescimento
    de regiões.

>> Funcionamento:

    1. O programa recebe os nomes dos arquivos sem extenção por meio de passagem de argumentos
    e copia os dados da imagem de entrada (.pgm) e do arquivo de sementes (.txt).

    3. A imagem de saída é salva em um arquivo com o mesmo nome da imagem original de formato
    .ppm, com as regiões segmentadas.
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

    int num_sementes = 0;
    FILE *arquivo_imagem = NULL;
    FILE *arquivo_semente = NULL;
    tipo_semente_pai *sementes = NULL;
    tipo_imagem_ppm *imagem_saida = NULL;
    tipo_imagem_pgm *imagem_entrada = NULL;

    arquivo_imagem = abrir_arquivo (argv[1], ".pgm", "r");
    imagem_entrada = armazenar_imagem_entrada (arquivo_imagem);
    imagem_saida = alocar_matriz_saida (imagem_entrada);

    if (usar_sementes_aleatorias ()) {
        segmentar_aleatoriamente (imagem_entrada, imagem_saida, sementes, &num_sementes);
    }

    else {
        arquivo_semente = abrir_arquivo (argv[1], ".txt", "r");
        sementes = armazenar_sementes (arquivo_semente, &num_sementes);
        segmentar_regioes(imagem_entrada, imagem_saida, sementes, num_sementes);
    }

    criar_imagem_saida (imagem_saida, argv[1]);
    desalocar_imagem_entrada (imagem_entrada);
    desalocar_imagem_saida (imagem_saida);
    desalocar_sementes (sementes, num_sementes);

    return EXIT_SUCCESS;
}
