#include "../headers/solucionador.h"

// Para guardar o 'pai' de uma célula no caminho ótimo
typedef struct {
    int k_origem; // 0=presente, 1=passado
    int i_origem; // linha da qual viemos
} EloCaminho;

// Para guardar o caminho final
typedef struct {
    int i;
    int j;
} Coordenada;

// Função auxiliar para acesso linear
static inline int idx(int i, int w, int j) {
    return i * w + j;
}

void resolver_e_imprimir(Mapa *mapa) {
    int h = mapa->h;
    int w = mapa->w;
    int F_inicial = mapa->F;
    int D = mapa->D;
    int N = mapa->N;
    size_t tam_matriz = (size_t)h * (size_t)w;

    // --- 1. Alocar Tabelas da PD ---

    // mapas_dp[k][i][j] -> usando 2 ponteiros para os 2 mapas
    // mapas_dp[0] = DP do Presente, mapas_dp[1] = DP do Passado
    int* mapas_dp[2];
    mapas_dp[0] = (int*)malloc(tam_matriz * sizeof(int));
    mapas_dp[1] = (int*)malloc(tam_matriz * sizeof(int));

    // mapas_caminho[k][i][j] -> tabelas de 'pais' para backtracking
    EloCaminho* mapas_caminho[2];
    mapas_caminho[0] = (EloCaminho*)malloc(tam_matriz * sizeof(EloCaminho));
    mapas_caminho[1] = (EloCaminho*)malloc(tam_matriz * sizeof(EloCaminho));

    if (!mapas_dp[0] || !mapas_dp[1] || !mapas_caminho[0] || !mapas_caminho[1]) {
        fprintf(stderr, "Falha de alocação de memória para DP.\n");
        // Libera o que conseguiu alocar
        free(mapas_dp[0]);
        free(mapas_dp[1]);
        free(mapas_caminho[0]);
        free(mapas_caminho[1]);
        return;
    }

    // Inicializar tudo como 'impossível'
    for (size_t i = 0; i < tam_matriz; i++) {
        mapas_dp[0][i] = -1;
        mapas_dp[1][i] = -1;
        mapas_caminho[0][i].i_origem = -1; // -1 = sem pai (início ou impossível)
        mapas_caminho[1][i].i_origem = -1;
    }

    // --- 2. Caso Base (Coluna j = 0) ---
    // O percurso PODE começar em qualquer célula da coluna 0
    
    for (int i = 0; i < h; i++) {
        // Usa os nomes atualizados: 'Celula', 'cell_at', 'presente'
        Celula* celula_p = cell_at(mapa->presente, h, w, i, 0);
        int F_nova = -1;

        // Usa os nomes atualizados: 'TipoCelula'
        switch (celula_p->type) {
            case CELULA_MONSTRO:
                F_nova = F_inicial - celula_p->value;
                break;
            case CELULA_VAZIA:
                F_nova = F_inicial + D;
                break;
            case CELULA_ANCORA:
                F_nova = F_inicial; // Sem custo
                break;
            case CELULA_BLOQUEADA:
                continue; // Impossível começar aqui
        }

        // Se a força acabou já no começo
        if (F_nova <= 0) continue; 

        // Atualizar tabela DP
        if (celula_p->type == CELULA_ANCORA) {
            // Destino é o mapa do Passado
            mapas_dp[1][idx(i, w, 0)] = F_nova;
            mapas_caminho[1][idx(i, w, 0)].i_origem = i; // Ponto de início
            mapas_caminho[1][idx(i, w, 0)].k_origem = 0;
        } else {
            // Destino é o mapa do Presente
            mapas_dp[0][idx(i, w, 0)] = F_nova;
            mapas_caminho[0][idx(i, w, 0)].i_origem = i; // Ponto de início
            mapas_caminho[0][idx(i, w, 0)].k_origem = 0;
        }
    }

    // --- 3. Recorrência (Colunas j = 1 até w-1) ---

    for (int j = 1; j < w; j++) {
        for (int i = 0; i < h; i++) {
            for (int k = 0; k < 2; k++) { // k=0 (Presente), k=1 (Passado)
                
                // Usa os nomes atualizados: 'presente', 'passado'
                Celula* mapa_atual = (k == 0) ? mapa->presente : mapa->passado;
                Celula* celula_atual = cell_at(mapa_atual, h, w, i, j);

                if (celula_atual->type == CELULA_BLOQUEADA) continue;

                // Testar as 3 origens possíveis
                for (int di = -1; di <= 1; di++) {
                    int i_prev = i + di;

                    // Checar se a linha anterior é válida
                    if (i_prev < 0 || i_prev >= h) continue;

                    // Força no passo anterior (no mesmo mapa k)
                    int F_prev = mapas_dp[k][idx(i_prev, w, j - 1)];

                    if (F_prev <= 0) continue; // Caminho de origem inválido

                    int F_nova = -1;
                    switch (celula_atual->type) {
                        case CELULA_MONSTRO: F_nova = F_prev - celula_atual->value; break;
                        case CELULA_VAZIA:   F_nova = F_prev + D; break;
                        case CELULA_ANCORA:  F_nova = F_prev; break;
                        default: break; // Bloqueado já foi tratado
                    }

                    if (F_nova <= 0) continue; // Caminho morreu

                    // --- Atualizar tabela DP ---
                    int k_dest = k;
                    if (celula_atual->type == CELULA_ANCORA) {
                        k_dest = 1 - k; // Troca de mapa
                    }

                    // Se encontramos um caminho *melhor* para (k_dest, i, j)
                    if (F_nova > mapas_dp[k_dest][idx(i, w, j)]) {
                        mapas_dp[k_dest][idx(i, w, j)] = F_nova;
                        mapas_caminho[k_dest][idx(i, w, j)].k_origem = k;
                        mapas_caminho[k_dest][idx(i, w, j)].i_origem = i_prev;
                    }
                }
            }
        }
    }

    // --- 4. Encontrar a Solução (Melhor F na última coluna) ---

    int F_final = -1;
    int best_k = -1;
    int best_i = -1;

    // Pode sair por qualquer célula da última coluna
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < h; i++) {
            int f = mapas_dp[k][idx(i, w, w - 1)];
            if (f > F_final) {
                F_final = f;
                best_k = k;
                best_i = i;
            }
        }
    }

    // --- 5. Imprimir o Resultado ---

    if (F_final <= 0) {
        printf("A calamidade de Nikador é inevitável\n");
    } else {
        // Backtracking: Reconstruir o caminho
        Coordenada* caminho = (Coordenada*)malloc(w * sizeof(Coordenada)); // Máximo 'w' passos
        int passo = w - 1;

        int curr_k = best_k;
        int curr_i = best_i;
        int curr_j = w - 1;

        while (curr_j >= 0) {
            caminho[passo].i = curr_i;
            caminho[passo].j = curr_j;

            if (curr_j == 0) break; // Chegamos ao início

            EloCaminho link = mapas_caminho[curr_k][idx(curr_i, w, curr_j)];
            curr_k = link.k_origem;
            curr_i = link.i_origem;
            curr_j--;
            passo--;
        }

        // Imprimir o caminho na ordem correta
        for (int p = 0; p < w; p++) {
            printf("%d %d\n", caminho[p].i, caminho[p].j);
        }

        // Imprimir mensagem final
        if (F_final >= N) {
            printf("A ruína de Nikador é iminente\n");
        } else {
            printf("Será necessário mais planejamento para parar a calamidade\n");
        }
        free(caminho);
    }

    // --- 6. Liberar Memória ---
    free(mapas_dp[0]);
    free(mapas_dp[1]);
    free(mapas_caminho[0]);
    free(mapas_caminho[1]);
}