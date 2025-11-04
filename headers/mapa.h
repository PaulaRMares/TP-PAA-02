#ifndef MAPA_H
#define MAPA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

typedef enum {
    CELL_EMPTY = 0,   // "000"
    CELL_BLOCKED,     // "***"
    CELL_ANCHOR,      // "AAA"
    CELL_MONSTER      // "001".."999"
} CellType;

typedef struct {
    CellType type;
    int value;        // só usado se type == CELL_MONSTER (001..999)
} Cell;

typedef struct {
    int h, w;         // dimensões
    int F, D, N;      // força inicial, força por descanso, força de Nikador

    // matrizes h x w
    Cell *present;    // mapa do presente (vetor contíguo de h*w células)
    Cell *past;       // mapa do passado  (vetor contíguo de h*w células)
} Mapa;

/* Acesso linear: i linha [0..h-1], j coluna [0..w-1] */
static inline Cell* cell_at(Cell *grid, int h, int w, int i, int j){
    return &grid[i * w + j];
}

bool mapa_load_from_file(const char *path, Mapa *out, char *errbuf, size_t errbuf_sz);
void mapa_free(Mapa *m);

void mapa_print_header(const Mapa *m, FILE *out);
void mapa_print_grid(const Mapa *m, const Cell *grid, FILE *out);

#endif // MAPA_H
