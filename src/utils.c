#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char *texto, int tamanho) {
    if (fgets(texto, tamanho, stdin) != NULL) {
        size_t len = strlen(texto);
        if (len > 0 && texto[len-1] == '\n') {
            texto[len-1] = '\0';
        } else {
            limparBuffer();
        }
    }
}

int lerInteiro() {
    int numero;
    while (scanf("%d", &numero) != 1) {
        printf("Input invalido. Tente numero: ");
        limparBuffer();
    }
    limparBuffer();
    return numero;
}

void limparEcra() {
    // Verifica se estamos em Windows (_WIN32) ou Mac/Linux
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void esperarEnter() {
    printf("\n[Pressione ENTER para continuar...]");
    limparBuffer(); // Garante que não há lixo a atrapalhar
    getchar();      // Espera efetivamente pelo Enter
}