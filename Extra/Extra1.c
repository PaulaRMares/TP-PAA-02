#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/mapa.h"
#include "../headers/Extra1.h"

typedef struct {
    int k_origem;
    int i_origem;
} EloCaminhoExtra1;

static inline int idx_extra1(int i, int w, int j) {
    return i * w + j;
}

static int resolver_dp_silencioso(Mapa *mapa) {
    int h = mapa->h;
    int w = mapa->w;
    int F_inicial = mapa->F;
    int D = mapa->D;
    int N = mapa->N;
    size_t tam_matriz = (size_t)h * (size_t)w;

    int* mapas_dp[2];
    mapas_dp[0] = (int*)malloc(tam_matriz * sizeof(int));
    mapas_dp[1] = (int*)malloc(tam_matriz * sizeof(int));

    EloCaminhoExtra1* mapas_caminho[2];
    mapas_caminho[0] = (EloCaminhoExtra1*)malloc(tam_matriz * sizeof(EloCaminhoExtra1));
    mapas_caminho[1] = (EloCaminhoExtra1*)malloc(tam_matriz * sizeof(EloCaminhoExtra1));

    if (!mapas_dp[0] || !mapas_dp[1] || !mapas_caminho[0] || !mapas_caminho[1]) {
        free(mapas_dp[0]);
        free(mapas_dp[1]);
        free(mapas_caminho[0]);
        free(mapas_caminho[1]);
        return -1;
    }

    for (size_t i = 0; i < tam_matriz; i++) {
        mapas_dp[0][i] = -1;
        mapas_dp[1][i] = -1;
        mapas_caminho[0][i].i_origem = -1;
        mapas_caminho[1][i].i_origem = -1;
    }

    for (int i = 0; i < h; i++) {
        Celula* celula_p = cell_at(mapa->presente, h, w, i, 0);
        int F_nova = -1;

        switch (celula_p->type) {
            case CELULA_MONSTRO:
                F_nova = F_inicial - celula_p->value;
                break;
            case CELULA_VAZIA:
                F_nova = F_inicial + D;
                break;
            case CELULA_ANCORA:
                F_nova = F_inicial;
                break;
            case CELULA_BLOQUEADA:
                continue;
        }

        if (F_nova <= 0) continue;

        if (celula_p->type == CELULA_ANCORA) {
            mapas_dp[1][idx_extra1(i, w, 0)] = F_nova;
            mapas_caminho[1][idx_extra1(i, w, 0)].i_origem = i;
            mapas_caminho[1][idx_extra1(i, w, 0)].k_origem = 0;
        } else {
            mapas_dp[0][idx_extra1(i, w, 0)] = F_nova;
            mapas_caminho[0][idx_extra1(i, w, 0)].i_origem = i;
            mapas_caminho[0][idx_extra1(i, w, 0)].k_origem = 0;
        }
    }

    for (int j = 1; j < w; j++) {
        for (int i = 0; i < h; i++) {
            for (int k = 0; k < 2; k++) {
                Celula* mapa_atual = (k == 0) ? mapa->presente : mapa->passado;
                Celula* celula_atual = cell_at(mapa_atual, h, w, i, j);

                if (celula_atual->type == CELULA_BLOQUEADA) continue;

                for (int di = -1; di <= 1; di++) {
                    int i_prev = i + di;
                    if (i_prev < 0 || i_prev >= h) continue;

                    int F_prev = mapas_dp[k][idx_extra1(i_prev, w, j - 1)];
                    if (F_prev <= 0) continue;

                    int F_nova = -1;
                    switch (celula_atual->type) {
                        case CELULA_MONSTRO: F_nova = F_prev - celula_atual->value; break;
                        case CELULA_VAZIA:   F_nova = F_prev + D; break;
                        case CELULA_ANCORA:  F_nova = F_prev; break;
                        default: break;
                    }

                    if (F_nova <= 0) continue;

                    int k_dest = k;
                    if (celula_atual->type == CELULA_ANCORA) {
                        k_dest = 1 - k;
                    }

                    if (F_nova > mapas_dp[k_dest][idx_extra1(i, w, j)]) {
                        mapas_dp[k_dest][idx_extra1(i, w, j)] = F_nova;
                        mapas_caminho[k_dest][idx_extra1(i, w, j)].k_origem = k;
                        mapas_caminho[k_dest][idx_extra1(i, w, j)].i_origem = i_prev;
                    }
                }
            }
        }
    }

    int F_final = -1;

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < h; i++) {
            int f = mapas_dp[k][idx_extra1(i, w, w - 1)];
            if (f > F_final) {
                F_final = f;
            }
        }
    }

    free(mapas_dp[0]);
    free(mapas_dp[1]);
    free(mapas_caminho[0]);
    free(mapas_caminho[1]);

    return F_final;
}

static double medir_tempo_medio(Mapa *mapa, int repeticoes) {
    if (repeticoes <= 0) repeticoes = 1;
    clock_t ini = clock();
    for (int i = 0; i < repeticoes; i++) {
        resolver_dp_silencioso(mapa);
    }
    clock_t fim = clock();
    double total_ms = ((double)(fim - ini) / CLOCKS_PER_SEC) * 1000.0;
    return total_ms / repeticoes;
}

void executar_extra1(const char *arquivo) {
    if (!arquivo || !arquivo[0]) {
        printf("Nenhum arquivo informado para o Extra 1.\n");
        return;
    }

    Mapa m;
    char err[256];

    if (!mapa_carregar(arquivo, &m, err, sizeof(err))) {
        fprintf(stderr, "Erro ao carregar '%s' no Extra 1: %s\n", arquivo, err);
        return;
    }

    double t_ms = medir_tempo_medio(&m, 5);
    long tam = (long)m.h * (long)m.w;

    printf("arquivo;h;w;tam; tempo_ms_medio\n");
    printf("%s;%d;%d;%ld;%.6f\n", arquivo, m.h, m.w, tam, t_ms);

    mapa_liberar(&m);
}
