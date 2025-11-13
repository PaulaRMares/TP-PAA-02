#include <stdio.h>
#include <stdlib.h>
#include "../headers/mapa.h"
#include "../headers/Extra3.h"

void executar_extra3(const char *arquivo, int custo_viagem) {
    if (!arquivo || !arquivo[0]) {
        printf("Nenhum arquivo selecionado.\n");
        return;
    }

    Mapa mapa;
    char err[256];
    if (!mapa_carregar(arquivo, &mapa, err, sizeof(err))) {
        fprintf(stderr, "Erro ao carregar: %s\n", err);
        return;
    }

    printf("\n--- Extra 3 ---\nCusto de Viagem Temporal = %d\n\n", custo_viagem);

    int h = mapa.h;
    int w = mapa.w;
    size_t tam = (size_t)h * (size_t)w;

    // Alocação das tabelas de PD
    int *dp[2];
    EloCaminhoExtra3 *pai[2];
    
    for(int k=0; k<2; k++) {
        dp[k] = malloc(tam * sizeof(int));
        pai[k] = malloc(tam * sizeof(EloCaminhoExtra3));
        // Inicializa tabelas
        for(size_t x=0; x<tam; x++) {
            dp[k][x] = -1;
            pai[k][x].i_origem = -1;
            pai[k][x].k_origem = -1;
        }
    }

    // --- 1. Caso Base (Coluna 0) ---
    for (int i = 0; i < h; i++) {
        Celula *c = cell_at(mapa.presente, h, w, i, 0);
        int Fn = -1;
        
        switch(c->type){
            case CELULA_MONSTRO: Fn = mapa.F - c->value; break;
            case CELULA_VAZIA:   Fn = mapa.F + mapa.D; break;
            case CELULA_ANCORA:  Fn = mapa.F - custo_viagem; break; // Aplica custo extra
            default: break;
        }

        // Se sobreviveu e não é parede
        if (c->type != CELULA_BLOQUEADA && Fn > 0) {
            int k_dest = (c->type == CELULA_ANCORA) ? 1 : 0;
            dp[k_dest][idx_e3(i, w, 0)] = Fn;
            pai[k_dest][idx_e3(i, w, 0)].i_origem = i;
            pai[k_dest][idx_e3(i, w, 0)].k_origem = 0; 
        }
    }

    // --- 2. Recorrência (Colunas 1 a w-1) ---
    for (int j = 1; j < w; j++) {
        for (int i = 0; i < h; i++) {
            for (int k = 0; k < 2; k++) {
                Celula *atual = (k==0) ? mapa.presente : mapa.passado;
                Celula *c = cell_at(atual, h, w, i, j);

                if (c->type == CELULA_BLOQUEADA) continue;

                // Testa as 3 posições anteriores
                for (int di = -1; di <= 1; di++) {
                    int i_prev = i + di;
                    if (i_prev < 0 || i_prev >= h) continue;

                    int F_prev = dp[k][idx_e3(i_prev, w, j-1)];
                    if (F_prev <= 0) continue;

                    int Fn = -1;
                    switch(c->type){
                        case CELULA_MONSTRO: Fn = F_prev - c->value; break;
                        case CELULA_VAZIA:   Fn = F_prev + mapa.D; break;
                        case CELULA_ANCORA:  Fn = F_prev - custo_viagem; break; // Aplica custo extra
                        default: break;
                    }

                    if (Fn > 0) {
                        int k_dest = k;
                        if (c->type == CELULA_ANCORA) k_dest = 1 - k;

                        if (Fn > dp[k_dest][idx_e3(i, w, j)]) {
                            dp[k_dest][idx_e3(i, w, j)] = Fn;
                            pai[k_dest][idx_e3(i, w, j)].k_origem = k;
                            pai[k_dest][idx_e3(i, w, j)].i_origem = i_prev;
                        }
                    }
                }
            }
        }
    }

    // --- 3. Verifica o Resultado Final ---
    int F_final = -1, best_k = -1, best_i = -1;
    for(int k=0; k<2; k++){
        for(int i=0; i<h; i++){
            int f = dp[k][idx_e3(i, w, w-1)];
            if(f > F_final) { 
                F_final = f; 
                best_k = k; 
                best_i = i; 
            }
        }
    }

    // --- 4. Impressão conforme a Especificação ---
    if (F_final <= 0) {
        // Caso de falha
        printf("A calamidade de Nikador e inevitavel\n");
        
    } else {
        // Recupera o caminho (Backtracking) para um vetor temporário
        PassoCaminho *rota = malloc(w * sizeof(PassoCaminho));
        int idx_rota = w - 1;
        
        int curr_k = best_k;
        int curr_i = best_i;
        int curr_j = w - 1;

        while(curr_j >= 0) {
            rota[idx_rota].i = curr_i;
            rota[idx_rota].j = curr_j;
            rota[idx_rota].k = curr_k;

            if(curr_j == 0) break;

            EloCaminhoExtra3 p = pai[curr_k][idx_e3(curr_i, w, curr_j)];
            curr_k = p.k_origem;
            curr_i = p.i_origem;
            curr_j--;
            idx_rota--;
        }

        for(int step = 0; step < w; step++){
            printf("%d %d", rota[step].i, rota[step].j);
            // Mostrar em qual mapa está, já que é o diferencial do Extra 3
            printf(" [%s]\n", (rota[step].k == 0) ? "Presente" : "Passado");
        }
        free(rota);
        printf("\n");

        if (F_final >= mapa.N) {
            printf("A ruina de Nikador e iminente\n");
        } else {
            printf("Sera necessario mais planejamento para parar a calamidade\n");
        }
    }

    // --- 5. Liberação de Memória ---
    free(dp[0]); free(dp[1]);
    free(pai[0]); free(pai[1]);
    mapa_liberar(&mapa);
}