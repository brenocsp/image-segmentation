#include "segmentacao-imagens.h"

/* >> FUNCOES: -------------------------------------------------------------------------------*/

void testar_alocacao (void *ponteiro, int tipo){
    if (!ponteiro) {
        if (tipo == ARQUIVO) printf("Erro ao abrir arquivo!\n");
        else printf("Erro ao alocar memoria!\n");
        exit (EXIT_FAILURE);
    }
}

FILE *abrir_arquivo (char *nome, char *formato, char *tipo_acesso) {
    FILE *arquivo = NULL;
    char arquivo_temp [MAX_NOME];
    sprintf(arquivo_temp, "%s%s", nome, formato);
    arquivo = fopen (arquivo_temp, tipo_acesso);
    testar_alocacao (arquivo, ARQUIVO);
    return arquivo;
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
    return imagem;
}

tipo_semente_pai *armazenar_sementes (FILE *arquivo, int *num_sementes) {
    fscanf(arquivo, "%d\n", &*num_sementes);
    tipo_semente_pai *pai = (tipo_semente_pai*) calloc(*num_sementes, sizeof(tipo_semente_pai));
    testar_alocacao (pai, MEMORIA);

    fscanf(arquivo, "%d\n", &pai[0].limiar);
    for (int i = 0; i < *num_sementes; i++) {
        pai[i].limiar = pai[0].limiar;
        fscanf(arquivo, "%d, %d <%d, %d, %d>\n", &pai[i].coluna_inicial, &pai[i].linha_inicial, &pai[i].R, &pai[i].G, &pai[i].B);
    }
    fclose (arquivo);
    return pai;
}

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

void enfileirar (tipo_imagem_pgm *imagem_entrada, tipo_semente_pai *pai, int i, int j) {
    definir_posicao_visitada (imagem_entrada, i, j);
    tipo_semente_filho *novo_filho = (tipo_semente_filho*) malloc(sizeof(tipo_semente_filho));
    testar_alocacao (novo_filho, MEMORIA);
    if (pai->primeiro == NULL) {
        pai->primeiro = novo_filho;
        pai->ultimo = novo_filho;
    }
    else {
        pai->ultimo->irmao = novo_filho;
        pai->ultimo = novo_filho;
    }
    pai->num_filhos++;
    novo_filho->linha = i;
    novo_filho->coluna = j;
}

bool verificar_condicoes (tipo_imagem_pgm *imagem, int limiar, int cor, int i, int j) {
    bool pode_enfileirar = false;
    if (i > 0 && j > 0 && i < imagem->linhas && j < imagem->colunas) {
        if (!imagem->pixels[i][j].passei && abs(cor - imagem->pixels[i][j].cor) <= limiar) {
            pode_enfileirar = true;
        }
    }
    return pode_enfileirar;
}

void testar_vizinhos (tipo_imagem_pgm *imagem, tipo_semente_pai *pai) {
    int limiar = pai->limiar;
    int i = pai->primeiro->linha;
    int j = pai->primeiro->coluna;
    int cor = imagem->pixels[i][j].cor; // cor atual

    /* Testar vizinhos na horizontal e vertical: */
    if (verificar_condicoes (imagem, limiar, cor, i-1, j  )) enfileirar (imagem, pai, i-1, j  ); // cima
    if (verificar_condicoes (imagem, limiar, cor, i  , j-1)) enfileirar (imagem, pai, i  , j-1); // esquerda
    if (verificar_condicoes (imagem, limiar, cor, i+1, j  )) enfileirar (imagem, pai, i+1, j  ); // baixo
    if (verificar_condicoes (imagem, limiar, cor, i  , j+1)) enfileirar (imagem, pai, i  , j+1); // direita

    /* Testar vizinhos nas diagonais: */
    // if (verificar_condicoes (imagem, limiar, cor, i-1, j+1)) enfileirar (imagem, pai, i-1, j+1); // diagonal superior direita
    // if (verificar_condicoes (imagem, limiar, cor, i-1, j-1)) enfileirar (imagem, pai, i-1, j-1); // diagonal superior esquerda
    // if (verificar_condicoes (imagem, limiar, cor, i+1, j-1)) enfileirar (imagem, pai, i+1, j-1); // diagonal inferior esquerda
    // if (verificar_condicoes (imagem, limiar, cor, i+1, j+1)) enfileirar (imagem, pai, i+1, j+1); // diagonal inferior direita
}

void definir_posicao_visitada (tipo_imagem_pgm *imagem_entrada, int i, int j) {
    imagem_entrada->pixels[i][j].passei = true;
}

void coloriar_imagem_saida (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai) {
    imagem_saida->pixels[pai->primeiro->linha][pai->primeiro->coluna].R = pai->R;
    imagem_saida->pixels[pai->primeiro->linha][pai->primeiro->coluna].G = pai->G;
    imagem_saida->pixels[pai->primeiro->linha][pai->primeiro->coluna].B = pai->B;
}

void desenfileirar (tipo_imagem_ppm *imagem_saida, tipo_semente_pai *pai) {
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

void resetar_matriz_passei (tipo_imagem_pgm *imagem) {
    for (int i = 0; i < imagem->linhas; i++) {
        for (int j = 0; j < imagem->colunas; j++) {
            imagem->pixels[i][j].passei = false;
        }
    }
}

void segmentar_regioes (tipo_imagem_pgm *imagem_entrada, tipo_imagem_ppm *imagem_saida, tipo_semente_pai *sementes, int num_sementes) {
    for (int i = 0; i < num_sementes; i++){
        enfileirar (imagem_entrada, &sementes[i], sementes[i].linha_inicial, sementes[i].coluna_inicial);
        while (sementes[i].num_filhos != 0) {
            testar_vizinhos (imagem_entrada, &sementes[i]);
            coloriar_imagem_saida (imagem_saida, &sementes[i]);
            desenfileirar (imagem_saida, &sementes[i]);
        }
        resetar_matriz_passei (imagem_entrada);
    }
}

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

bool usar_sementes_aleatorias () {
    char opcao;
    printf ("\nDeseja utilizar sementes aleatorias? [S / N]: ");
    scanf ("%c", &opcao);

    if (opcao == 'S' || opcao == 's') return true;
    else if (opcao == 'N' || opcao == 'n') return false;
    else {
        printf("Comando invalido! Tente novamente.\n");
        return usar_sementes_aleatorias ();
    }
}

bool usar_recurvidade () {
    char opcao;
    printf ("\nDeseja utilizar algoritmo recursivo? [S / N]: ");
    scanf ("%c", &opcao);

    if (opcao == 'S' || opcao == 's') return true;
    else if (opcao == 'N' || opcao == 'n') return false;
    else {
        printf("Comando invalido! Tente novamente.\n");
        return usar_recurvidade ();
    }
}
