#include "segmentacao-imagens.h"

/* >> FUNCOES: -------------------------------------------------------------------------------*/

tipo_semente_pai *gerar_pai_aleatorio (int i, int j) {
    tipo_semente_pai *pai = (tipo_semente_pai*) malloc(sizeof(tipo_semente_pai));
    testar_alocacao (pai, MEMORIA);

    pai->limiar = 10;
    pai->num_filhos = 0;
    pai->primeiro = NULL;
    pai->ultimo = NULL;
    pai->linha_inicial = i;
    pai->coluna_inicial = j;

    pai->R = rand() % 255;
    pai->G = rand() % 255;
    pai->B = rand() % 255;

    return (pai);
}

void segmentar_aleatoriamente (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai, int *num_sementes) {
    for (int i = 0; i < imagem_entrada->linhas; i++) {
        for (int j = 0; j < imagem_entrada->colunas; j++) {
            if (!imagem_entrada->pixels[i][j].passei) {
                pai = gerar_pai_aleatorio (i, j);
                enfileirar (imagem_entrada, pai, i, j);
                while (pai->num_filhos != 0) {
                    testar_vizinhos (imagem_entrada, pai);
                    coloriar_imagem_saida (imagem_saida, pai);
                    desenfileirar (imagem_saida, pai);
                }
                free (pai);
            }
        }
    }
}
