#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/mapa.h"
#include "../headers/solucionador.h"
#include "../headers/Extra1.h"
#include "../headers/Extra2.h"

static char ultimo_arquivo[256] = "";
static int tem_arquivo = 0;

void menu() {
    printf("=====================================\n");
    printf("     TRABALHO 2 - PAA (2025/2)\n");
    printf("=====================================\n");
    printf("1 - Resolver mapa (arquivo existente)\n");
    printf("2 - Gerar mapa aleatorio (Extra4)\n");
    printf("3 - Teste de desempenho (Extra1 - usa ultimo arquivo)\n");
    printf("4 - Visualizar melhor caminho (Extra2 - usa ultimo arquivo)\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    int opcao;
    do {
        menu();
        if (scanf("%d", &opcao) != 1) {
            fprintf(stderr, "Entrada invalida.\n");
            return 1;
        }
        getchar();

        if (opcao == 1) {
            char nome_arquivo[128];
            char path[256];

            printf("Digite o nome do arquivo de entrada (ex: entrada1.txt): ");
            if (scanf("%127s", nome_arquivo) != 1) {
                fprintf(stderr, "Erro ao ler o nome do arquivo.\n");
                return 1;
            }

            snprintf(path, sizeof(path), "entrada/%s", nome_arquivo);

            Mapa mapa;
            char err[256];
            if (!mapa_carregar(path, &mapa, err, sizeof(err))) {
                fprintf(stderr, "Erro ao carregar '%s': %s\n", path, err);
                continue;
            }

            resolver_e_imprimir(&mapa);
            mapa_liberar(&mapa);

            strncpy(ultimo_arquivo, path, sizeof(ultimo_arquivo));
            ultimo_arquivo[sizeof(ultimo_arquivo) - 1] = '\0';
            tem_arquivo = 1;
        }

        else if (opcao == 2) {
            printf("\nExecutando gerador de mapas aleatorios (Extra4)...\n\n");
        #ifdef _WIN32
            int ret = system("cd Extra\\output && Extra4.exe");
        #else
            int ret = system("cd Extra/output && ./Extra4");
        #endif
            if (ret != 0) {
                printf("Nao foi possivel executar o gerador.\n");
                printf("Certifique-se de que ele foi compilado no diretorio Extra/output.\n");
            }
        }

        else if (opcao == 3) {
            if (!tem_arquivo) {
                printf("\nNenhum arquivo foi carregado ainda pela opcao 1.\n");
                printf("Use a opcao 1 para carregar um mapa antes de executar o Extra 1.\n");
            } else {
                printf("\nAnalise de desempenho (Extra1) para o arquivo: %s\n\n", ultimo_arquivo);
                executar_extra1(ultimo_arquivo);
            }
        }

        else if (opcao == 4) {
            if (!tem_arquivo) {
                printf("\nNenhum arquivo foi carregado ainda pela opcao 1.\n");
                printf("Use a opcao 1 para carregar um mapa antes de executar o Extra 2.\n");
            } else {
                printf("\nVisualizacao do melhor caminho (Extra2) para o arquivo: %s\n", ultimo_arquivo);
                executar_extra2(ultimo_arquivo);
            }
        }

        else if (opcao == 0) {
            printf("Encerrando programa...\n");
        }

        else {
            printf("Opcao invalida.\n");
        }

        printf("\n");
    } while (opcao != 0);

    return 0;
}
