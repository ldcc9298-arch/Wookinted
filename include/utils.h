#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief Protótipos das funções utilitárias.
 * @details Contém funções para manipulação de strings, limpeza de ecrã, etc.
 * @date 2025-12-22
 */
void limparBuffer();

/**
 * @brief Lê uma string do stdin com tamanho limitado.
 * @param texto Apontador para o buffer onde a string será armazenada.
 * @param tamanho Tamanho máximo da string (incluindo o terminador nulo).
 */
void lerString(char *texto, int tamanho);

/**
 * @brief Lê um número inteiro do stdin com validação.
 * @return int O número inteiro lido.
 */
int lerInteiro();

/**
 * @brief Limpa o ecrã do terminal.
 */
void limparEcra();

/**
 * @brief Pausa o programa e pede ao utilizador para carregar em ENTER.
 * Essencial para o utilizador ler as mensagens antes de limparmos o ecrã.
 */
void esperarEnter();

#endif