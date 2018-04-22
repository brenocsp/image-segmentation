/* ---------------------------------------------------------------------------------------------

UNIVERSIDADE FEDERAL DE MINAS GERAIS
Departamento de Ciencia da Computacao

DCC004 - Algoritmos e Estruturas de Dados II
Profs. Cristiano Arbex Valle e Gisele L. Pappa

TP1 - Segmentacao de imagens
    Data de Entrega: 06/05/2018

Autor: Breno Claudio de Sena Pimenta
    Graduacao em Engenharia Eletrica
    Matrícula: 2017074424

------------------------------------------------------------------------------------------------

>> Objetivo:

    O objetivo desse trabalho e implementar um programa que le uma imagem .pgm e segmenta suas
    regioes utilizando um algoritmo de crescimento de regioes.

>> Funcionamento:

--------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* >> CONSTANTES: ----------------------------------------------------------------------------*/


/* >> TIPOs DE DADOS: ------------------------------------------------------------------------*/

typedef struct tipo_imagem_pgm {
    int altura;
    int largura;
    char nivel_intensidade;
    char **imagem;
}tipo_imagem_pgm;

typedef struct tipo_semente {
    int linha, coluna;
    int R, G, B;
}tipo_semente;

/* >> FUNCOES: -------------------------------------------------------------------------------*/

/* Funcao para abrir os arquivos */
void abrir_arquivos (char **argv, FILE *arquivo_imagem, FILE *arquivo_semente){
    int tamanho_nome = strlen(argv[1]);
    char arquivo_temp[tamanho_nome];

    sprintf(arquivo_temp, "%s.pgm", argv[1]);
    arquivo_imagem = fopen(arquivo_temp, "r");
    if (!arquivo_imagem) {
        printf("Arquivo %s nao encontrado", arquivo_temp);
        exit(EXIT_FAILURE);
    }

    sprintf(arquivo_temp, "%s.txt", argv[1]);
    arquivo_semente = fopen(arquivo_temp, "r");
    if (!arquivo_semente) {
        printf("Arquivo %s nao encontrado", arquivo_temp);
        exit(EXIT_FAILURE);
    }
}
/*<<< FIM ABRIR ARQUIVOS >>>*/


/* >> PROGRAMA PRINCIPAL: -------------------------------------------------------------------*/

int main (int argc, char **argv){
    if (argc == 1) {
        printf("Nenhum arquivo de imagem foi passado\n");
        exit(EXIT_FAILURE);
    }

    FILE *arquivo_imagem = NULL;
    FILE *arquivo_semente = NULL;

    tipo_imagem_pgm imagem_pgm;
    tipo_semente semente;

    abrir_arquivos (argv, arquivo_imagem, arquivo_semente);

    fclose (arquivo_imagem);
    fclose (arquivo_semente);


    return EXIT_SUCCESS;
}
