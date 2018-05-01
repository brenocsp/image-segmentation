/* >> BIBLIOTECAS: ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "segmentacao-imagens.h"

/* >> CONSTANTES: ----------------------------------------------------------------------------*/

#define ARQUIVO 0
#define MEMORIA 1

/* >> PROGRAMA PRINCIPAL: -------------------------------------------------------------------*/

int main (int argc, char **argv) {

    int num_sementes = 0;
    FILE *arquivo_imagem = NULL;
    FILE *arquivo_semente = NULL;
    tipo_semente_pai *sementes = NULL;
    tipo_imagem_ppm *imagem_saida = NULL;
    tipo_imagem_pgm *imagem_entrada = NULL;

    abrir_arquivos (argc, argv, &arquivo_imagem, &arquivo_semente);
    imagem_entrada = armazenar_imagem_entrada (arquivo_imagem);
    imagem_saida = criar_matriz_saida (imagem_entrada);
    sementes = armazenar_sementes (arquivo_semente, &num_sementes);

    segmentar_regioes (imagem_entrada, imagem_saida, sementes, num_sementes);
    criar_imagem_saida (imagem_saida, argv);

    desalocar_imagem_entrada (imagem_entrada);
    desalocar_imagem_saida (imagem_saida);
    desalocar_sementes (sementes, num_sementes);

    return EXIT_SUCCESS;
}
