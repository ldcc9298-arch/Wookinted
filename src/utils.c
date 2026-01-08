#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // Necessário para isspace
#include "utils.h"

/**
 * @brief Limpa o buffer de entrada para evitar lixo.
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Lê uma string do stdin de forma segura.
 * @param texto Ponteiro para o buffer onde a string será armazenada.
 * @param tamanho Tamanho máximo do buffer.
 */
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

/**
 * @brief Lê um inteiro do stdin de forma segura.
 * @return O inteiro lido.
 */
int lerInteiro() {
    int numero;
    while (scanf("%d", &numero) != 1) {
        printf("Input invalido. Tente numero: ");
        limparBuffer();
    }
    limparBuffer();
    return numero;
}

/**
 * @brief Limpa o ecrã/terminal.
 */
void limparEcra() {
    // Verifica se estamos em Windows (_WIN32) ou Mac/Linux
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief Pausa a execução até o utilizador pressionar ENTER.
 */
void esperarEnter() {
    printf("\n[Pressione ENTER para continuar...]");
    limparBuffer(); // Garante que não há lixo a atrapalhar
    getchar();      // Espera efetivamente pelo Enter
}

/**
 * @brief Verifica se uma string não está vazia (contém pelo menos um caractere não espaço).
 * @param str A string a verificar.
 * @return 1 se não estiver vazia, 0 caso contrário.
 */
int stringNaoVazia(char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    
    // Verifica se existe pelo menos um caractere que não seja espaço
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 1; // Encontrou um caractere válido
        }
    }
    return 0; // Só tem espaços
}

// Retorna 1 se o nome for válido (sem números), 0 se inválido
int validarFormatoNome(char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        // Se encontrar um dígito (0-9), falha imediatamente
        if (isdigit((unsigned char)str[i])) {
            return 0;
        }
        // Opcional: Se quiseres proibir símbolos como @, $, %, etc no nome:
        // Podes adicionar: if (ispunct(str[i]) && str[i] != '-') return 0;
    }
    return 1;
}

// Retorna 1 se o email parecer válido (tem @ e . e não tem espaços)
int validarFormatoEmail(char *str) {
    if (str == NULL || strlen(str) < 5) return 0; // a@b.c = 5 chars min

    // 1. Não pode ter espaços
    if (strchr(str, ' ') != NULL) return 0;

    // 2. Tem de ter um '@'
    char *arroba = strchr(str, '@');
    if (arroba == NULL) return 0;

    // 3. Tem de ter um '.' DEPOIS do '@'
    // strchr procura a partir do ponteiro do arroba
    char *ponto = strchr(arroba, '.'); 
    if (ponto == NULL) return 0;
    
    // 4. Não pode ser o último caracter (ex: "joao@ipca.")
    if (ponto == str + strlen(str) - 1) return 0;

    return 1;
}