#include "segmentacao-imagens.h"

/* >> FUNCOES: -------------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------------------------*/

void testar_alocacao (void *ponteiro, int tipo){
    if (!ponteiro) {
        if (tipo == ARQUIVO) printf("Erro ao abrir arquivo!\n");
        else printf("Erro ao alocar memoria!\n");
        exit (EXIT_FAILURE);
    }
}

/* -------------------------------------------------------------------------------------------*/

FILE *abrir_arquivo (char *nome, char *formato, char *tipo_acesso) {
    FILE *arquivo = NULL;
    char arquivo_temp [MAX_NOME];
    sprintf(arquivo_temp, "%s%s", nome, formato);
    arquivo = fopen (arquivo_temp, tipo_acesso);
    testar_alocacao (arquivo, ARQUIVO);
    return arquivo;
}

/* -------------------------------------------------------------------------------------------*/

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
    return imagem;
}

/* -------------------------------------------------------------------------------------------*/

tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes) {
    fscanf(arquivo, "%d\n", num_sementes);
    tipo_semente_pai *pai = (tipo_semente_pai*) calloc(*num_sementes, sizeof(tipo_semente_pai));
    testar_alocacao (pai, MEMORIA);

    fscanf(arquivo, "%d\n", &pai[0].limiar);
    for (int i = 0; i < *num_sementes; i++) {
        pai[i].limiar = pai[0].limiar;
        fscanf(arquivo, "%d, %d <%d, %d, %d>\n", &pai[i].coluna_inicial, &pai[i].linha_inicial, &pai[i].cor.R, &pai[i].cor.G, &pai[i].cor.B);
    }
    fclose (arquivo);
    return pai;
}

/* -------------------------------------------------------------------------------------------*/

tipo_imagem_ppm *alocar_matriz_saida (tipo_imagem_pgm *imagem_entrada) {
    tipo_imagem_ppm *imagem_saida = (tipo_imagem_ppm*) malloc(sizeof(tipo_imagem_ppm));
    testar_alocacao (imagem_saida, MEMORIA);

    imagem_saida->linhas = imagem_entrada->linhas;
    imagem_saida->colunas = imagem_entrada->colunas;
    imagem_saida->pixels = (tipo_pixel_rgb**) calloc(imagem_saida->linhas, sizeof(tipo_pixel_rgb));
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
    return imagem_saida;
}

/* -------------------------------------------------------------------------------------------*/

void desalocar_imagem_entrada (tipo_imagem_pgm *imagem) {
    for (int i = 0; i < imagem->linhas; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}

/* -------------------------------------------------------------------------------------------*/

void desalocar_imagem_saida (tipo_imagem_ppm *imagem){
    for (int i = 0; i < imagem->linhas; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}

/* -------------------------------------------------------------------------------------------*/

void desalocar_sementes (tipo_semente_pai *sementes, int num_sementes) {
    for (int i = 0; i < num_sementes; i++) {
        while (sementes[i].num_filhos != 0) {
            tipo_semente_filho *filho_removido = sementes[i].primeiro;
            if (sementes[i].primeiro != sementes[i].ultimo) sementes[i].primeiro = sementes[i].primeiro->irmao;
            free (filho_removido);
            sementes[i].num_filhos--;
        }
    }
    free (sementes);
}

/* -------------------------------------------------------------------------------------------*/

void enfileirar (tipo_semente_pai *pai, int i, int j) {
    tipo_semente_filho *novo_filho = (tipo_semente_filho*) malloc(sizeof(tipo_semente_filho));
    testar_alocacao (novo_filho, MEMORIA);
    if (pai->primeiro == NULL) {
        pai->primeiro = novo_filho;
        pai->ultimo = novo_filho;
        novo_filho->irmao = NULL;
    }
    else {
        pai->ultimo->irmao = novo_filho;
        pai->ultimo = novo_filho;
    }
    pai->num_filhos++;
    novo_filho->linha = i;
    novo_filho->coluna = j;
}

/* -------------------------------------------------------------------------------------------*/

bool verificar_condicoes (tipo_imagem_pgm *imagem, int limiar, int cor, int i, int j) {
    bool pode_enfileirar = false;
    if (i >= 0 && j >= 0 && i < imagem->linhas && j < imagem->colunas) {
        if (!imagem->pixels[i][j].passei && abs(cor - imagem->pixels[i][j].cor) <= limiar) {
            pode_enfileirar = true;
            imagem->pixels[i][j].passei = true;
        }
    }
    return pode_enfileirar;
}

/* -------------------------------------------------------------------------------------------*/

void testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai) {
    int limiar = pai->limiar;
    int i = pai->primeiro->linha;
    int j = pai->primeiro->coluna;
    int cor = imagem->pixels[i][j].cor; // cor atual
    imagem->pixels[pai->primeiro->linha][pai->primeiro->coluna].passei = true;

    /* Testar vizinhos na horizontal e vertical: */
    if (verificar_condicoes (imagem, limiar, cor, i-1, j  )) enfileirar (pai, i-1, j  ); // cima
    if (verificar_condicoes (imagem, limiar, cor, i  , j-1)) enfileirar (pai, i  , j-1); // esquerda
    if (verificar_condicoes (imagem, limiar, cor, i+1, j  )) enfileirar (pai, i+1, j  ); // baixo
    if (verificar_condicoes (imagem, limiar, cor, i  , j+1)) enfileirar (pai, i  , j+1); // direita

    /* Testar vizinhos nas diagonais: (caso queira experimentar)*/
    // if (verificar_condicoes (imagem, limiar, cor, i-1, j+1)) enfileirar (pai, i-1, j+1); // diagonal superior direita
    // if (verificar_condicoes (imagem, limiar, cor, i-1, j-1)) enfileirar (pai, i-1, j-1); // diagonal superior esquerda
    // if (verificar_condicoes (imagem, limiar, cor, i+1, j-1)) enfileirar (pai, i+1, j-1); // diagonal inferior esquerda
    // if (verificar_condicoes (imagem, limiar, cor, i+1, j+1)) enfileirar (pai, i+1, j+1); // diagonal inferior direita
}

/* -------------------------------------------------------------------------------------------*/

void colorir_imagem_saida (tipo_imagem_ppm *imagem_saida, tipo_pixel_rgb *cor_rgb, int i, int j) {
    imagem_saida->pixels[i][j].R = cor_rgb->R;
    imagem_saida->pixels[i][j].G = cor_rgb->G;
    imagem_saida->pixels[i][j].B = cor_rgb->B;
}

/* -------------------------------------------------------------------------------------------*/

void desenfileirar (tipo_semente_pai *pai) {
    if (pai->num_filhos > 1) {
        tipo_semente_filho *filho_removido = pai->primeiro;
        pai->primeiro = pai->primeiro->irmao;
        free (filho_removido);
    }
    else {
        free (pai->primeiro);
    }
    pai->num_filhos--;
}

/* -------------------------------------------------------------------------------------------*/

void resetar_matriz_passei (tipo_imagem_pgm *imagem) {
    for (int i = 0; i < imagem->linhas; i++) {
        for (int j = 0; j < imagem->colunas; j++) {
            imagem->pixels[i][j].passei = false;
        }
    }
}

/* -------------------------------------------------------------------------------------------*/

void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida, tipo_semente_pai *sementes, int num_sementes) {
    for (int i = 0; i < num_sementes; i++){
        enfileirar (&sementes[i], sementes[i].linha_inicial, sementes[i].coluna_inicial);
        while (sementes[i].num_filhos != 0) {
            testar_vizinhos (imagem_entrada, &sementes[i]);
            colorir_imagem_saida (imagem_saida, &sementes[i].cor, sementes[i].primeiro->linha, sementes[i].primeiro->coluna);
            desenfileirar (&sementes[i]);
        }
        resetar_matriz_passei (imagem_entrada);
    }
}

/* -------------------------------------------------------------------------------------------*/

void criar_imagem_saida (tipo_imagem_ppm *imagem, char *nome_arquivo) {
    FILE *arquivo = abrir_arquivo (nome_arquivo, ".ppm", "w");
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

/* -------------------------------------------------------------------------------------------*/

/* >> FUNCOES RECURSIVAS E SEMENTES ALEATORIAS: ----------------------------------------------*/

/* -------------------------------------------------------------------------------------------*/

void testar_vizinhos_recursivamente (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida,
                                     tipo_pixel_rgb *cor_rgb, int i, int j) {
    int cor_cinza = imagem_entrada->pixels[i][j].cor; // cor atual
    colorir_imagem_saida (imagem_saida, cor_rgb, i, j);

    /* Testar vizinhos na horizontal e vertical: */
    if (verificar_condicoes (imagem_entrada, LIMIAR, cor_cinza, i - 1, j)) {
        imagem_entrada->pixels[i - 1][j].passei = true;
        testar_vizinhos_recursivamente (imagem_entrada, imagem_saida, cor_rgb, i - 1, j);
    }
    if (verificar_condicoes (imagem_entrada, LIMIAR, cor_cinza, i + 1, j)) {
        imagem_entrada->pixels[i + 1][j].passei = true;
        testar_vizinhos_recursivamente (imagem_entrada, imagem_saida, cor_rgb, i + 1, j);
    }
    if (verificar_condicoes (imagem_entrada, LIMIAR, cor_cinza, i, j + 1)) {
        imagem_entrada->pixels[i][j + 1].passei = true;
        testar_vizinhos_recursivamente (imagem_entrada, imagem_saida, cor_rgb, i, j + 1);
    }
    if (verificar_condicoes (imagem_entrada, LIMIAR, cor_cinza, i, j - 1)) {
        imagem_entrada->pixels[i][j - 1].passei = true;
        testar_vizinhos_recursivamente (imagem_entrada, imagem_saida, cor_rgb, i, j - 1);
    }

    else return;
}

/* -------------------------------------------------------------------------------------------*/

void segmentar_aleatoriamente (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida) {
    tipo_pixel_rgb cor;
    int linha;
    int coluna;
    int limite_funcional = (imagem_entrada->linhas + imagem_entrada->colunas)/2;
    for (int i = 0; i < limite_funcional; i++) {
        cor.R = rand() % 256;
        cor.G = rand() % 256;
        cor.B = rand() % 256;
        linha = rand() % imagem_entrada->linhas;
        coluna = rand() % imagem_entrada->colunas;
        if (!imagem_entrada->pixels[linha][coluna].passei) {
            imagem_entrada->pixels[linha][coluna].passei = true;
            testar_vizinhos_recursivamente (imagem_entrada, imagem_saida, &cor, linha, coluna);
        }
    }
}

/* -------------------------------------------------------------------------------------------*/
