#include "../headers/mapa.h"
#include "../headers/solucionador.h" // <--- ADICIONE ESTE INCLUDE

int main(){
    char nome_arquivo[128];
    char path[256];

    printf("Digite o nome do arquivo de entrada (ex: entrada1.txt): ");
    if (scanf("%127s", nome_arquivo) != 1) {
        fprintf(stderr, "Erro ao ler o nome do arquivo.\n");
        return 1;
    }

    // monta o caminho completo: entrada/<nome_arquivo>
    snprintf(path, sizeof(path), "entrada/%s", nome_arquivo);

    Mapa mapa;
    char err[256];
    // Usa o nome atualizado: 'mapa_carregar'
    if (!mapa_carregar(path, &mapa, err, sizeof(err))) {
        fprintf(stderr, "Erro ao carregar '%s': %s\n", path, err);
        return 1;
    }

    /* * Comente ou remova as impressões de debug abaixo 
     * para ter a saída limpa do jeito que o PDF pede.
     */
    // puts("====== Cabecalho ======");
    // mapa_print_info(&mapa, stdout);
    // puts("\n====== Mapa do Presente ======");
    // mapa_print_matriz(&mapa, mapa.presente, stdout);
    // puts("\n====== Mapa do Passado ======");
    // mapa_print_matriz(&mapa, mapa.passado, stdout);


    // --- CHAME O SOLUCIONADOR AQUI ---
    resolver_e_imprimir(&mapa);


    // Usa o nome atualizado: 'mapa_liberar'
    mapa_liberar(&mapa);
    return 0;
}