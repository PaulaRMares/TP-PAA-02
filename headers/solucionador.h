#ifndef SOLUCIONADOR_H
#define SOLUCIONADOR_H

#include "mapa.h"

/*
 * Função principal que resolve o problema.
 * Ela aloca as tabelas da PD, calcula a melhor rota,
 * e imprime os resultados na saída padrão conforme 
 * [cite_start]especificado no PDF [cite: 71-76].
 */
void resolver_e_imprimir(Mapa *mapa);

#endif // SOLUCIONADOR_H