#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[256];
    char **palavras = NULL; // Vetor dinâmico de ponteiros para strings
    int quantidade = 0;

    printf("Digite as palavras (ou 'sair' para encerrar):\n");

    // 1. LEITURA E ALOCAÇÃO DINÂMICA
    while (1) {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        // Remove o '\n' que o fgets insere ao final da string
        buffer[strcspn(buffer, "\n")] = '\0';

        // Condição de parada
        if (strcmp(buffer, "sair") == 0) {
            break;
        }

        // Realoca o vetor principal de ponteiros para caber mais uma palavra
        // Se quantidade for 0, o realloc age como um malloc
        palavras = realloc(palavras, (quantidade + 1) * sizeof(char*));
        if (palavras == NULL) {
            printf("Erro de alocacao de memoria!\n");
            return 1;
        }

        // Aloca o espaço exato para a palavra atual (+1 para o '\0')
        palavras[quantidade] = malloc((strlen(buffer) + 1) * sizeof(char));
        if (palavras[quantidade] == NULL) {
            printf("Erro de alocacao de memoria!\n");
            return 1;
        }

        // Copia a palavra do buffer temporário para o espaço definitivo alocado
        strcpy(palavras[quantidade], buffer);
        quantidade++;
    }

    // 2. ORDENAÇÃO ALFABÉTICA (Bubble Sort adaptado para strings)
    for (int i = 0; i < quantidade - 1; i++) {
        for (int j = 0; j < quantidade - i - 1; j++) {
            // strcmp retorna > 0 se a primeira string for alfabeticamente maior que a segunda
            if (strcmp(palavras[j], palavras[j+1]) > 0) {
                // Troca apenas os ponteiros, não o conteúdo das strings! (Muito mais rápido)
                char *temp = palavras[j];
                palavras[j] = palavras[j+1];
                palavras[j+1] = temp;
            }
        }
    }

    // 3. IMPRESSÃO
    printf("\n--- Palavras em Ordem Alfabetica ---\n");
    for (int i = 0; i < quantidade; i++) {
        printf("%s\n", palavras[i]);
    }

    // 4. LIBERAÇÃO DE MEMÓRIA (Crucial para não vazar memória)
    for (int i = 0; i < quantidade; i++) {
        free(palavras[i]); // Libera cada palavra individualmente
    }
    free(palavras); // Libera o vetor de ponteiros principal

    return 0;
}