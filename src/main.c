#include "../headers/mapa.h"

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
    if (!mapa_carregar(path, &mapa, err, sizeof(err))) {
        fprintf(stderr, "Erro ao carregar '%s': %s\n", path, err);
        return 1;
    }

    /* Mostra um resumo para conferir o parsing */
    puts("====== Cabecalho ======");
    mapa_print_info(&mapa, stdout);

    puts("\n====== Mapa do Presente ======");
    mapa_print_matriz(&mapa, mapa.presente, stdout);

    puts("\n====== Mapa do Passado ======");
    mapa_print_matriz(&mapa, mapa.passado, stdout);

    mapa_liberar(&mapa);
    return 0;
}