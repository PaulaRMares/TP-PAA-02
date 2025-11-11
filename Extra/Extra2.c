#include <stdio.h>
#include <stdlib.h>
#include "../headers/mapa.h"
#include "../headers/Extra2.h"

typedef struct {
    int k_origem;
    int i_origem;
} EloCaminhoExtra2;

typedef struct {
    int k;
    int i;
    int j;
} PassoExtra2;

static inline int idx_e2(int i, int w, int j) {
    return i * w + j;
}

static void marcar_caminho(PassoExtra2 *passos, int w, int h, int *pathPresente, int *pathPassado) {
    for (int j = 0; j < w; j++) {
        int k = passos[j].k;
        int i = passos[j].i;
        if (i < 0) continue;
        if (k == 0) pathPresente[idx_e2(i, w, j)] = 1;
        else pathPassado[idx_e2(i, w, j)] = 1;
    }
}

static void imprimir_mapa_visual(const Mapa *m, int *pathPresente, int *pathPassado) {
    printf("\nMapa do Presente (P = caminho):\n");
    for (int i = 0; i < m->h; i++) {
        for (int j = 0; j < m->w; j++) {
            char ch;
            if (pathPresente[idx_e2(i, m->w, j)]) ch = 'P';
            else {
                Celula *c = cell_at(m->presente, m->h, m->w, i, j);
                if (c->type == CELULA_VAZIA) ch = '.';
                else if (c->type == CELULA_BLOQUEADA) ch = '#';
                else if (c->type == CELULA_ANCORA) ch = 'A';
                else ch = 'M';
            }
            putchar(ch);
            putchar(' ');
        }
        putchar('\n');
    }

    printf("\nMapa do Passado (P = caminho):\n");
    for (int i = 0; i < m->h; i++) {
        for (int j = 0; j < m->w; j++) {
            char ch;
            if (pathPassado[idx_e2(i, m->w, j)]) ch = 'P';
            else {
                Celula *c = cell_at(m->passado, m->h, m->w, i, j);
                if (c->type == CELULA_VAZIA) ch = '.';
                else if (c->type == CELULA_BLOQUEADA) ch = '#';
                else if (c->type == CELULA_ANCORA) ch = 'A';
                else ch = 'M';
            }
            putchar(ch);
            putchar(' ');
        }
        putchar('\n');
    }
}

void executar_extra2(const char *path) {
    if (!path || path[0] == '\0') {
        printf("Nenhum arquivo valido informado para o Extra2.\n");
        return;
    }

    Mapa mapa;
    char err[256];

    if (!mapa_carregar(path, &mapa, err, sizeof(err))) {
        fprintf(stderr, "Erro ao carregar '%s' no Extra2: %s\n", path, err);
        return;
    }

    int h = mapa.h;
    int w = mapa.w;
    int F_inicial = mapa.F;
    int D = mapa.D;
    int N = mapa.N;
    size_t tam = (size_t)h * (size_t)w;

    int *dp[2];
    EloCaminhoExtra2 *caminho[2];

    dp[0] = (int*)malloc(tam * sizeof(int));
    dp[1] = (int*)malloc(tam * sizeof(int));
    caminho[0] = (EloCaminhoExtra2*)malloc(tam * sizeof(EloCaminhoExtra2));
    caminho[1] = (EloCaminhoExtra2*)malloc(tam * sizeof(EloCaminhoExtra2));

    if (!dp[0] || !dp[1] || !caminho[0] || !caminho[1]) {
        printf("Falha de memoria no Extra2.\n");
        free(dp[0]); free(dp[1]);
        free(caminho[0]); free(caminho[1]);
        mapa_liberar(&mapa);
        return;
    }

    for (size_t i = 0; i < tam; i++) {
        dp[0][i] = -1;
        dp[1][i] = -1;
        caminho[0][i].i_origem = -1;
        caminho[0][i].k_origem = -1;
        caminho[1][i].i_origem = -1;
        caminho[1][i].k_origem = -1;
    }

    for (int i = 0; i < h; i++) {
        Celula *c = cell_at(mapa.presente, h, w, i, 0);
        int Fn = -1;

        if (c->type == CELULA_MONSTRO) Fn = F_inicial - c->value;
        else if (c->type == CELULA_VAZIA) Fn = F_inicial + D;
        else if (c->type == CELULA_ANCORA) Fn = F_inicial;

        if (c->type == CELULA_BLOQUEADA || Fn <= 0) continue;

        if (c->type == CELULA_ANCORA) {
            int id = idx_e2(i, w, 0);
            dp[1][id] = Fn;
            caminho[1][id].i_origem = i;
            caminho[1][id].k_origem = 0;
        } else {
            int id = idx_e2(i, w, 0);
            dp[0][id] = Fn;
            caminho[0][id].i_origem = i;
            caminho[0][id].k_origem = 0;
        }
    }

    for (int j = 1; j < w; j++) {
        for (int i = 0; i < h; i++) {
            for (int k = 0; k < 2; k++) {
                Celula *mat = (k == 0) ? mapa.presente : mapa.passado;
                Celula *c = cell_at(mat, h, w, i, j);
                if (c->type == CELULA_BLOQUEADA) continue;

                for (int di = -1; di <= 1; di++) {
                    int ip = i + di;
                    if (ip < 0 || ip >= h) continue;

                    int Fprev = dp[k][idx_e2(ip, w, j - 1)];
                    if (Fprev <= 0) continue;

                    int Fn = -1;
                    if (c->type == CELULA_MONSTRO) Fn = Fprev - c->value;
                    else if (c->type == CELULA_VAZIA) Fn = Fprev + D;
                    else if (c->type == CELULA_ANCORA) Fn = Fprev;

                    if (Fn <= 0) continue;

                    int kd = k;
                    if (c->type == CELULA_ANCORA) kd = 1 - k;

                    int id = idx_e2(i, w, j);
                    if (Fn > dp[kd][id]) {
                        dp[kd][id] = Fn;
                        caminho[kd][id].k_origem = k;
                        caminho[kd][id].i_origem = ip;
                    }
                }
            }
        }
    }

    int F_final = -1;
    int best_k = -1;
    int best_i = -1;

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < h; i++) {
            int f = dp[k][idx_e2(i, w, w - 1)];
            if (f > F_final) {
                F_final = f;
                best_k = k;
                best_i = i;
            }
        }
    }

    if (F_final <= 0) {
        printf("A calamidade de Nikador e inevitavel\n");
    } else {
        PassoExtra2 *passos = (PassoExtra2*)malloc(w * sizeof(PassoExtra2));
        if (!passos) {
            printf("Falha de memoria no Extra2.\n");
            free(dp[0]); free(dp[1]);
            free(caminho[0]); free(caminho[1]);
            mapa_liberar(&mapa);
            return;
        }

        int j = w - 1;
        int k = best_k;
        int i = best_i;

        while (j >= 0) {
            passos[j].k = k;
            passos[j].i = i;
            passos[j].j = j;
            if (j == 0) break;
            EloCaminhoExtra2 e = caminho[k][idx_e2(i, w, j)];
            k = e.k_origem;
            i = e.i_origem;
            j--;
        }

        int *pathPresente = (int*)calloc(tam, sizeof(int));
        int *pathPassado = (int*)calloc(tam, sizeof(int));

        if (pathPresente && pathPassado) {
            marcar_caminho(passos, w, h, pathPresente, pathPassado);
            imprimir_mapa_visual(&mapa, pathPresente, pathPassado);
        }

        free(pathPresente);
        free(pathPassado);
        free(passos);

        if (F_final >= N)
            printf("\nA ruina de Nikador e iminente\n");
        else
            printf("\nSera necessario mais planejamento para parar a calamidade\n");
    }

    free(dp[0]);
    free(dp[1]);
    free(caminho[0]);
    free(caminho[1]);
    mapa_liberar(&mapa);
}
