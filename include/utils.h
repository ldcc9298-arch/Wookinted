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

int validarApenasNumeros(const char *str);

int validarFormatoEmailIPCA(char *email);

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


void relatorioTopUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes);

/**
 * @brief Gera um relatório dos 5 livros mais requisitados.
 * @param books Array de livros.
 * @param totalBooks Número total de livros.
 */
void relatorioTopLivros(Livro books[], int totalBooks);

/**
 * @brief Calcula a média de avaliações (notas) recebidas por um utilizador.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Número total de feedbacks no array.
 * @param idUser ID do utilizador alvo.
 * @param qtdReviews Ponteiro para armazenar a quantidade de reviews (pode ser NULL).
 * @return Média das notas (float). Retorna 0.0 se não houver avaliações.
 */
float calcularMediaUtilizador(Feedback feedbacks[], int totalFeedbacks, int idUser, int *qtdReviews); 

/**
 * @brief Gera um relatório dos 5 utilizadores com melhor reputação.
 * @param users Array de utilizadores.
 * @param totalUsers Número total de utilizadores.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Número total de feedbacks.
 */
void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks);

/**
 * @brief Lista o histórico completo de operações do utilizador logado.
 * @param operacoes Array de operações.
 * @param totalOperacoes Total de operações.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param users Array de utilizadores.
 * @param totalUsers Total de utilizadores.
 * @param idLogado ID do utilizador logado.
 */
void listarHistoricoCompleto(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado);

/**
 * @brief Valida o formato do ISBN-13.
 * @param isbn String do ISBN (pode conter hífens).
 * @return 1 se válido, 0 se inválido.
 */
int validarISBN(const char *isbn);

/**
 * @brief Verifica se um ISBN já existe no sistema.
 * @param isbn String do ISBN a verificar.
 * @param books Array de livros existentes.
 * @param total Número total de livros no array.
 * @return 1 se o ISBN existir, 0 caso contrário.
 */
int existeISBN(Livro books[], int totalBooks, char *isbn);

/**
 * @brief Converte uma string para minúsculas.
 * @param origem String original.
 * @param destino Buffer onde a string em minúsculas será armazenada.
 */
void paraMinusculas(const char *origem, char *destino);

/**
 * @brief Obtém a data atual no formato AAAAMMDD.
 * @return Data atual como inteiro.
 */
int obterDataAtual();

/**
 * @brief Formata uma data do formato inteiro AAAAMMDD para DD/MM/AAAA.
 * @param dataInt Data no formato inteiro (AAAAMMDD).
 * @param buffer Buffer onde a data formatada será armazenada.
 */
void formatarData(int dataInt, char *buffer);

/**
 * @brief Soma um número de dias a uma data no formato YYYYMMDD.
 * @param dataInicio Data inicial no formato inteiro YYYYMMDD.
 * @param dias Número de dias a somar.
 * @return Nova data no formato inteiro YYYYMMDD.
 */
int somarDias(int dataInicio, int dias);

int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes);

void adminValidarUtilizadores(Utilizador users[], int totalUsers);

void registarLog(int idUser, char *acao, char *detalhes);

int validarData(char *data);

float lerFloat(char* mensagem, float min, float max);


#endif // UTILS_H