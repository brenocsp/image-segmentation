#include "segmentacao-imagens.h"

/* >> FUNCOES: -------------------------------------------------------------------------------*/

void testar_alocacao (void *ponteiro, int tipo){
    if (!ponteiro) {
        if (tipo == ARQUIVO) printf("Erro ao abrir arquivo!\n");
        else printf("Erro ao alocar memoria!\n");
        exit (EXIT_FAILURE);
    }
}

void abrir_arquivos (int argc, char **argv, FILE **arquivo_imagem, FILE **arquivo_semente) {
    if (argc == 1) {
        printf("Nenhum arquivo de imagem foi passado\n");
        exit(EXIT_FAILURE);
    }

    int tamanho_nome = strlen(argv[1]);
    char arquivo_temp [tamanho_nome];

    sprintf(arquivo_temp, "%s.pgm", argv[1]);
    *arquivo_imagem = fopen(arquivo_temp, "r");
    testar_alocacao (arquivo_imagem, ARQUIVO);

    sprintf(arquivo_temp, "%s.txt", argv[1]);
    *arquivo_semente = fopen(arquivo_temp, "r");
    testar_alocacao (arquivo_semente, ARQUIVO);
}

tipo_imagem_pgm *armazenar_imagem_entrada (FILE *arquivo) {
    tipo_imagem_pgm *imagem = (tipo_imagem_pgm*) malloc(sizeof(tipo_imagem_pgm));
    testar_alocacao (imagem, MEMORIA);

    fscanf(arquivo, "P2\n%d\n%d\n%d\n", &imagem->colunas, &imagem->linhas, &imagem->nivel);
    imagem->pixels = (tipo_pixel_cinza**)calloc(imagem->linhas, sizeof(tipo_pixel_cinza*));
    testar_alocacao (imagem->pixels, MEMORIA);

    for (int i = 0; i < imagem->linhas; i++) {
        imagem->pixels[i] = (tipo_pixel_cinza*) calloc(imagem->colunas, sizeof(tipo_pixel_cinza));
        testar_alocacao (imagem->pixels[i], MEMORIA);

        for (int j = 0; j < imagem->colunas; j++) {
            fscanf(arquivo, "%d ", &imagem->pixels[i][j].cor);
        }
    fscanf(arquivo, "\n");
    }
    fclose (arquivo);
    return (imagem);
}

tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes) {
    fscanf(arquivo, "%d\n", &*num_sementes);
    tipo_semente_pai *pai = (tipo_semente_pai*) calloc(*num_sementes, sizeof(tipo_semente_pai));
    testar_alocacao (pai, MEMORIA);

    fscanf(arquivo, "%d\n", &pai[0].limiar);
    for (int id = 0; id < *num_sementes; id++) {
        pai[id].limiar = pai[0].limiar;
        fscanf(arquivo, "%d, %d <%d, %d, %d>\n", &pai[id].coluna_inicial, &pai[id].linha_inicial, &pai[id].R, &pai[id].G, &pai[id].B);
    }

    fclose (arquivo);
    return(pai);
}

tipo_imagem_ppm *criar_matriz_saida (tipo_imagem_pgm *imagem_entrada) {
    tipo_imagem_ppm *imagem_saida = (tipo_imagem_ppm*) malloc(sizeof(tipo_imagem_ppm));
    testar_alocacao (imagem_saida, MEMORIA);

    imagem_saida->linhas = imagem_entrada->linhas;
    imagem_saida->colunas = imagem_entrada->colunas;
    imagem_saida->pixels = (tipo_pixel_rgb**)calloc(imagem_saida->linhas, sizeof(tipo_pixel_rgb));
    testar_alocacao (imagem_saida->pixels, MEMORIA);

    for (int i = 0; i < imagem_saida->linhas; i++) {
        imagem_saida->pixels[i] = (tipo_pixel_rgb*) calloc(imagem_saida->colunas, sizeof(tipo_pixel_rgb));
        testar_alocacao (imagem_saida->pixels[i], MEMORIA);

        for (int j = 0; j < imagem_saida->colunas; j++) {
            imagem_saida->pixels[i][j].R = (255 / imagem_entrada->nivel) * imagem_entrada->pixels[i][j].cor;
            imagem_saida->pixels[i][j].G = imagem_saida->pixels[i][j].R;
            imagem_saida->pixels[i][j].B = imagem_saida->pixels[i][j].R;
        }
    }
    return (imagem_saida);
}

void desalocar_imagem_entrada (tipo_imagem_pgm *imagem) {
    for (int i = 0; i < imagem->linhas; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}

void desalocar_imagem_saida (tipo_imagem_ppm *imagem){
    for (int i = 0; i < imagem->linhas; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}

void desalocar_sementes (tipo_semente_pai *pai, int num_sementes) {
    for (int id = 0; id < num_sementes; id++) {
        while (pai[id].num_filhos != 0) {
            tipo_semente_filho *filho_removido = pai[id].primeiro;
            if (pai[id].primeiro != pai[id].ultimo) pai[id].primeiro = pai[id].primeiro->irmao;
            pai[id].num_filhos--;
            free (filho_removido);
        }
    }
    free (pai);
}

void enfileirar_semente (tipo_semente_pai *pai, int id, int i, int j) {
    pai[id].ultimo->irmao = (tipo_semente_filho*) malloc(sizeof(tipo_semente_filho));
    testar_alocacao (pai[id].ultimo->irmao, MEMORIA);
    pai[id].num_filhos++;
    pai[id].ultimo = pai[id].ultimo->irmao;
    pai[id].ultimo->linha = i;
    pai[id].ultimo->coluna = j;
}

void testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai, int id) {
    int i = pai[id].primeiro->linha;
    int j = pai[id].primeiro->coluna;
    int cor_semente = imagem->pixels[i][j].cor;

    /* cima */
    if(i > 0 && !imagem->pixels[i-1][j].passei && abs(cor_semente - imagem->pixels[i-1][j].cor) <= pai[id].limiar) {
        enfileirar_semente (pai, id, i-1, j);
        imagem->pixels[i-1][j].passei = true;
    }

    /* esquerda */
    if(j > 0 && !imagem->pixels[i][j-1].passei && abs(cor_semente - imagem->pixels[i][j-1].cor) <= pai[id].limiar) {
        enfileirar_semente (pai, id, i, j-1);
        imagem->pixels[i][j-1].passei = true;
    }

    /* baixo */
    if(i + 1 < imagem->linhas && !imagem->pixels[i+1][j].passei && abs(cor_semente - imagem->pixels[i+1][j].cor) <= pai[id].limiar) {
        enfileirar_semente (pai, id, i+1, j);
        imagem->pixels[i+1][j].passei = true;
    }

    /* direita */
    if(j + 1 < imagem->colunas && !imagem->pixels[i][j+1].passei && abs(cor_semente - imagem->pixels[i][j+1].cor) <= pai[id].limiar) {
        enfileirar_semente (pai, id, i, j+1);
        imagem->pixels[i][j+1].passei = true;
    }
}

void desenfileirar_semente (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai, int id) {
    imagem_saida->pixels[pai[id].primeiro->linha][pai[id].primeiro->coluna].R = pai[id].R;
    imagem_saida->pixels[pai[id].primeiro->linha][pai[id].primeiro->coluna].G = pai[id].G;
    imagem_saida->pixels[pai[id].primeiro->linha][pai[id].primeiro->coluna].B = pai[id].B;

    if (pai[id].num_filhos > 1) {
        tipo_semente_filho *filho_removido = pai[id].primeiro;
        pai[id].primeiro = pai[id].primeiro->irmao;
        free (filho_removido);
    }
    else {
        free (pai[id].primeiro);
    }
    pai[id].num_filhos--;
}

void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai, int num_sementes) {
    for (int id = 0; id < num_sementes; id++){
        pai[id].primeiro = (tipo_semente_filho*) malloc(sizeof(tipo_semente_filho));
        testar_alocacao (pai[id].primeiro, MEMORIA);
        pai[id].num_filhos = 1;
        pai[id].ultimo = pai[id].primeiro;
        pai[id].primeiro->linha = pai[id].linha_inicial;
        pai[id].primeiro->coluna = pai[id].coluna_inicial;
        imagem_entrada->pixels[pai[id].primeiro->linha][pai[id].primeiro->coluna].passei = true;

        while (pai[id].num_filhos != 0) {
            testar_vizinhos (imagem_entrada, pai, id);
            desenfileirar_semente (imagem_saida, pai, id);
        }
    }
}

void criar_imagem_saida (tipo_imagem_ppm *imagem, char **argv) {
    int tamanho_nome = strlen (argv[1]);
    char arquivo_temp [tamanho_nome];

    sprintf(arquivo_temp, "%s.ppm", argv[1]);
    FILE *arquivo = fopen(arquivo_temp, "w");
    testar_alocacao (arquivo, ARQUIVO);

    fprintf(arquivo, "P3\n%d %d\n255\n", imagem->colunas, imagem->linhas);
    for (int i = 0; i < imagem->linhas; i++) {
        for (int j = 0; j < imagem->colunas; j++) {
            fprintf(arquivo, "%d %d %d", imagem->pixels[i][j].R, imagem->pixels[i][j].G, imagem->pixels[i][j].B);
            if (j != imagem->linhas - 1) fprintf(arquivo, " ");
        }
        fprintf(arquivo, "\n");
    }
    fclose (arquivo);
}
