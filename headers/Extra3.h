#ifndef EXTRA3_H
#define EXTRA3_H

// Executa o algoritmo considerando que viagens temporais (Âncoras)
// têm um custo fixo de 'custo_viagem' (ex: 10 ou 15).
// Imprime o caminho resultante comparando com a lógica padrão.

// Estrutura para o backtracking
typedef struct {
    int k_origem;
    int i_origem;
} EloCaminhoExtra3;

// Estrutura auxiliar para inverter o caminho na hora de imprimir
typedef struct {
    int i, j, k;
} PassoCaminho;

static inline int idx_e3(int i, int w, int j) {
    return i * w + j;
}

void executar_extra3(const char *arquivo, int custo_viagem);

#endif