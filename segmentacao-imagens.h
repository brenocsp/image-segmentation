/* >> BIBLIOTECAS: ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* >> CONSTANTES: ----------------------------------------------------------------------------*/
//
#define ARQUIVO 0
#define MEMORIA 1

/* >> TIPOs DE DADOS: ------------------------------------------------------------------------*/

typedef struct tipo_pixel_cinza {
    int cor;
    bool passei;
}tipo_pixel_cinza;

typedef struct tipo_pixel_rgb {
    int R, G, B;
}tipo_pixel_rgb;

typedef struct tipo_imagem_ppm {
    int linhas, colunas, nivel;
    struct tipo_pixel_rgb **pixels;
}tipo_imagem_ppm;

typedef struct tipo_imagem_pgm {
    int linhas, colunas, nivel;
    struct tipo_pixel_cinza **pixels;
}tipo_imagem_pgm;

typedef struct tipo_semente_pai {
    int limiar, R, G, B, num_filhos,
        linha_inicial, coluna_inicial;
    struct tipo_semente_filho *primeiro;
    struct tipo_semente_filho *ultimo;
}tipo_semente_pai;

typedef struct tipo_semente_filho {
    int linha, coluna;
    struct tipo_semente_filho *irmao;
}tipo_semente_filho;


/* >> FUNCOES: -------------------------------------------------------------------------------*/

void testar_alocacao (void *ponteiro, int tipo);

void abrir_arquivos (int argc, char **argv, FILE **arquivo_imagem, FILE **arquivo_semente);

tipo_imagem_pgm *armazenar_imagem_entrada (FILE *arquivo);

tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes);

tipo_imagem_ppm *criar_matriz_saida (tipo_imagem_pgm *imagem_entrada);

void desalocar_imagem_entrada (tipo_imagem_pgm *imagem);

void desalocar_imagem_saida (tipo_imagem_ppm *imagem);

void desalocar_sementes (tipo_semente_pai *pai, int num_sementes);

void enfileirar_semente (tipo_semente_pai *pai, int id, int i, int j);

void testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai, int id);

void desenfileirar_semente (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai, int id);

void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai, int num_sementes);

void criar_imagem_saida (tipo_imagem_ppm *imagem, char **argv);
