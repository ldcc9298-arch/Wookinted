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

/**
 * @brief Verifica se uma string não está vazia (contém pelo menos um caractere não espaço).
 * @param str A string a verificar.
 * @return 1 se não estiver vazia, 0 caso contrário.
 */
int stringNaoVazia(char *str);

/**
 * @brief Valida o formato do nome (apenas letras e espaços).
 * @param nome A string do nome a validar.
 */
int validarFormatoNome(char *nome);

/**
 * @brief Valida o formato do email (deve conter '@' e '.').
 * @param email A string do email a validar.
 */
int validarFormatoEmail(char *email);

#endif