#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>

#define ENTRADA_PATH "../../entrada/"

static void print_token(FILE *out, const char *tok) {
    fputs(tok, out);
}

static void gen_non_anchor_token(FILE *out, int pct_monstro, int pct_bloq) {
    int r = rand() % 100;
    if (r < pct_bloq) {
        print_token(out, "***");
    } else if (r < pct_bloq + pct_monstro) {
        int v = (rand() % 999) + 1;
        fprintf(out, "%03d", v);
    } else {
        print_token(out, "000");
    }
}

static int proximo_numero_arquivo() {
    DIR *dir;
    struct dirent *ent;
    int max_n = 0;

    dir = opendir(ENTRADA_PATH);
    if (dir == NULL) {
        printf("Erro: nao foi possivel abrir a pasta %s\n", ENTRADA_PATH);
        exit(1);
    }

    while ((ent = readdir(dir)) != NULL) {
        int n;
        if (sscanf(ent->d_name, "entrada%d.txt", &n) == 1) {
            if (n > max_n) max_n = n;
        }
    }

    closedir(dir);
    return max_n + 1;
}

int main() {
    srand((unsigned int)time(NULL));

    int h = (rand() % 6) + 5;
    int w = (rand() % 6) + 5;
    int F = (rand() % 101) + 50;
    int D = (rand() % 5) + 1;
    int N = (rand() % 101) + 30;

    int pct_monstro = (rand() % 41) + 10;
    int pct_bloq = (rand() % 16) + 5;
    int pct_ancora = (rand() % 9) + 2;

    int total = h * w;
    int *ancoras = (int *)calloc((size_t)total, sizeof(int));
    if (!ancoras) {
        printf("Falha de alocacao.\n");
        return 1;
    }

    int n = proximo_numero_arquivo();
    char nome_arquivo[256];
    sprintf(nome_arquivo, "%sentrada%d.txt", ENTRADA_PATH, n);

    FILE *out = fopen(nome_arquivo, "w");
    if (!out) {
        printf("Erro ao criar o arquivo %s\n", nome_arquivo);
        free(ancoras);
        return 1;
    }

    fprintf(out, "%d %d %d %d %d\n", h, w, F, D, N);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int idx = i * w + j;
            int r = rand() % 100;
            if (r < pct_ancora) {
                print_token(out, "AAA");
                ancoras[idx] = 1;
            } else {
                gen_non_anchor_token(out, pct_monstro, pct_bloq);
            }
            if (j + 1 < w) fputc(' ', out);
        }
        fputc('\n', out);
    }

    fprintf(out, "///\n");

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int idx = i * w + j;
            if (ancoras[idx]) {
                print_token(out, "AAA");
            } else {
                gen_non_anchor_token(out, pct_monstro, pct_bloq);
            }
            if (j + 1 < w) fputc(' ', out);
        }
        fputc('\n', out);
    }

    fclose(out);
    free(ancoras);

    printf("Mapa gerado com sucesso em: %s\n", nome_arquivo);
    printf("h=%d w=%d F=%d D=%d N=%d | monstros=%d%% bloqueios=%d%% ancoras=%d%%\n",
           h, w, F, D, N, pct_monstro, pct_bloq, pct_ancora);

    return 0;
}
