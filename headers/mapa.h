#ifndef MAPA_H
#define MAPA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

typedef enum {
    CELULA_VAZIA = 0,   // "000"
    CELULA_BLOQUEADA,     // "***"
    CELULA_ANCORA,      // "AAA"
    CELULA_MONSTRO      // "001".."999"
} TipoCelula;

typedef struct {
    TipoCelula type;
    int value;        // só usado se type == CELULA_MONSTRO (001..999)
} Celula;

typedef struct {
    int h, w;         // dimensões
    int F, D, N;      // força inicial, força por descanso, força de Nikador

    // matrizes h x w
    Celula *presente;    // mapa do presente (vetor contíguo de h*w células)
    Celula *passado;       // mapa do passado  (vetor contíguo de h*w células)
} Mapa;

/* Acesso linear: i linha [0..h-1], j coluna [0..w-1] */
static inline Celula* cell_at(Celula *matriz, int h, int w, int i, int j){
    (void)h;
    return &matriz[i * w + j];
}

bool mapa_carregar(const char *path, Mapa *out, char *errbuf, size_t errbuf_sz);
void mapa_liberar(Mapa *m);

void mapa_print_info(const Mapa *m, FILE *out);
void mapa_print_matriz(const Mapa *m, const Celula *matriz, FILE *out);

#endif // MAPA_H
