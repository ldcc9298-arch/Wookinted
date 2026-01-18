/**
 * @file transactions.h
 * @brief Definições e protótipos para a gestão de movimentos (Empréstimos, Trocas, Doações).
 * @details Este ficheiro estabelece a interface para processar o ciclo de vida de uma 
 * transação, garantindo a integridade dos dados entre livros e utilizadores.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include "structs.h"

/** @defgroup CICLO_VIDA Ciclo de Vida da Transação
 * @brief Funções que gerem a mudança de estados das operações (Aceitar, Rejeitar, Devolver).
 * @{ 
 */

/**
 * @brief Processa a aceitação de um pedido, atualizando a posse do livro.
 * @details Altera o estado da operação para ACEITE e atualiza o idDetentor ou idProprietario do livro.
 * @param op Ponteiro para a operação a aceitar.
 * @param books Array de livros para atualização de estado.
 * @param totalBooks Total de livros no sistema.
 * @param operacoes Array de operações.
 * @param totalOperacoes Ponteiro para o total de operações.
 * @param idLogado ID do utilizador que está a aceitar (Dono).
 */
void processarAceitacao(Operacao *op, Livro books[], int totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado);

/**
 * @brief Recusa um pedido e liberta os livros reservados para o mercado.
 * @details Altera o estado da operação para REJEITADO e repõe o estado do livro para DISPONIVEL.
 * @param op Ponteiro para a operação a rejeitar.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param idLogado ID do utilizador que rejeita.
 */
void processarRejeicao(Operacao *op, Livro books[], int totalBooks, int idLogado);

/**
 * @brief Conclui um empréstimo ativo e devolve o livro ao inventário do dono.
 * @param livro Ponteiro para o livro a ser devolvido.
 * @param operacoes Array de histórico para localizar a transação aberta.
 * @param totalOperacoes Total de registos no histórico.
 * @param idLogado ID do utilizador que processa a devolução.
 */
void executarDevolucao(Livro *livro, Operacao operacoes[], int totalOperacoes, int idLogado);

/** @} */

/** @defgroup REQUISICOES Criação de Novos Pedidos
 * @brief Funções para iniciar processos de troca ou empréstimo.
 * @{ 
 */

/**
 * @brief Cria o registo de um pedido de empréstimo no histórico (Estado: Pendente).
 * @param operacoes Array de operações.
 * @param totalOperacoes Ponteiro para o contador global de operações.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param idxLivro Índice do livro pretendido.
 * @param idLogado ID de quem pede.
 * @param dias Duração do empréstimo em dias.
 */
void criarRegistoEmprestimo(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, int idxLivro, int idLogado, int dias);

/**
 * @brief Cria o registo de uma proposta de troca no histórico (Estado: Pendente).
 * @param operacoes Array de operações.
 * @param totalOperacoes Ponteiro para o contador global.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param idxLivroAlvo Índice do livro que se quer receber.
 * @param idxMeuLivro Índice do livro que se oferece em troca.
 * @param idLogado ID de quem propõe a troca.
 */
void criarRegistoTroca(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, int idxLivroAlvo, int idxMeuLivro, int idLogado);

/** @} */

/** @defgroup FEEDBACK_SISTEMA Sistema de Avaliação e Auditoria
 * @brief Funções de pós-transação e registo de logs.
 * @{ 
 */

/**
 * @brief Cria um novo registo de feedback e atualiza o log de auditoria.
 * @param feeds Array de feedbacks do sistema.
 * @param totalFeeds Ponteiro para o total de feedbacks.
 * @param op Ponteiro para a operação avaliada.
 * @param idLogado ID do autor da avaliação.
 * @param nota Pontuação atribuída (1.0 a 5.0).
 * @param comentario Texto descritivo.
 */
void processarNovoFeedback(Feedback feeds[], int *totalFeeds, Operacao *op, int idLogado, float nota, char *comentario);

/**
 * @brief Verifica se um utilizador já avaliou uma operação específica.
 * @param feeds Array de feedbacks.
 * @param totalFeeds Total de feedbacks existentes.
 * @param idOperacao ID da transação a verificar.
 * @param idAvaliador ID do utilizador.
 * @return int 1 se já existir avaliação, 0 caso contrário.
 */
int jaAvaliou(Feedback feeds[], int totalFeeds, int idOperacao, int idAvaliador);

/** @} */

#endif // TRANSACTIONS_H