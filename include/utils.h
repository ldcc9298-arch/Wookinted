#ifndef UTILS_H
#define UTILS_H

#include "structs.h"

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
 * @param mensagem Mensagem a ser exibida ao utilizador.
 * @return int O número inteiro lido.
 */
int lerInteiro(char *mensagem, int min, int max);

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
 * @brief Calcula a idade com base na data de nascimento.
 * @param dataNascimento String no formato "DD-MM-YYYY".
 * @return Idade em anos.
 */
int calcularIdade(char *dataNascimento);

/**
 * @brief Valida o formato do nome (apenas letras e espaços).
 * @param nome A string do nome a validar.
 */
int validarApenasLetras(char *nome, int tamanho);

/**
 * @brief Valida o formato do email (deve conter '@' e '.').
 * @param email A string do email a validar.
 */
int validarFormatoEmail(char *email);

/**
 * @brief Gera o próximo ID único para um novo livro.
 * @param books Array de livros existentes.
 * @param total Número total de livros no array.
 * @return O próximo ID disponível (maior ID existente + 1).
 */
int gerarProximoId(Livro books[], int total);

/**
 * @brief Imprime uma linha formatada com os detalhes do livro.
 * @param book Apontador para o livro a imprimir.
 */
void imprimirLinhaLivro(Livro *book);

/**
 * @brief Gera um relatório dos 5 utilizadores com mais transações.
 * @param users Array de utilizadores.
 * @param totalUsers Número total de utilizadores.
 * @param loans Array de empréstimos.
 * @param totalLoans Número total de empréstimos.
 */
void relatorioTopTransacoes(Utilizador users[], int totalUsers, Operacao loans[], int totalLoans);

/**
 * @brief Gera um relatório dos 5 livros mais requisitados.
 * @param books Array de livros.
 * @param totalBooks Número total de livros.
 */
void relatorioTopLivros(Livro books[], int totalBooks);

/**
 * @brief Gera um relatório dos 5 utilizadores com melhor reputação.
 * @param users Array de utilizadores.
 * @param totalUsers Número total de utilizadores.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Número total de feedbacks.
 */
void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks);

/**
 * @brief Verifica se o utilizador tem histórico de empréstimos.
 * @param idUser ID do utilizador a verificar.
 * @param loans Array de empréstimos.
 * @param totalLoans Número total de empréstimos no array.
 * @return 1 se o utilizador tiver histórico, 0 caso contrário.
 */
int verificarHistoricoUtilizador(int idUser, Operacao loans[], int totalLoans);

/**
 * @brief Valida o formato do ISBN-13.
 * @param isbn String do ISBN (pode conter hífens).
 * @return 1 se válido, 0 se inválido.
 */
int validarISBN(const char *isbn);

#endif // UTILS_H