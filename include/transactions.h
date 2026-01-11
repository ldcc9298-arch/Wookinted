#ifndef LOANS_H
#define LOANS_H

#include "structs.h"

/** 
/// @brief Solicita uma operação de empréstimo ou troca.
/// @param loans array de empréstimos.
/// @param totalLoans número total de empréstimos.
/// @param book livro alvo.
/// @param idRequisitante ID do utilizador que está a pedir o livro.
*/
void solicitarEmprestimo(Operacao loans[], int *totalLoans, Livro *book, int idRequisitante);

/**
/// @brief Gere os pedidos pendentes para o utilizador logado.
/// @param loans array de empréstimos.
/// @param totalLoans número total de empréstimos.
/// @param books lista de livros.
/// @param totalBooks número total de livros.
/// @param idLogado ID do utilizador logado.
*/
void gerirPedidosPendentes(Operacao loans[], int totalLoans, Livro books[], int totalBooks, int idLogado);

/**
/// @brief Lista todos os empréstimos no sistema.
/// @param loans array de empréstimos.
/// @param totalLoans número total de empréstimos.
/// @param idLogado ID do utilizador logado.
*/
void listarEmprestimos(Operacao loans[], int totalLoans, int idLogado);

/**
/// @brief Processa a devolução de um livro.
/// @param book livro a ser devolvido.
/// @param loans array de empréstimos.
/// @param totalLoans número total de empréstimos.
/// @param idLogado ID do utilizador logado.
*/
void devolverLivro(Livro *book, Operacao loans[], int totalLoans, int idLogado);

/** 
/// @brief Doa um livro à instituição (transferência de propriedade).
/// @param books array de livros.
/// @param totalBooks número total de livros.
/// @param userId ID do utilizador que está a doar.
/// @param loans array de empréstimos.
/// @param totalLoans apontador para o número total de empréstimos (pode mudar).
*/
void doarLivro(Livro books[], int totalBooks, int userId, Operacao loans[], int *totalLoans);

/**
/// @brief Avalia um empréstimo concluído.
/// @param feedbacks array de feedbacks.
/// @param totalFeedbacks número total de feedbacks.
/// @param loan empréstimo a ser avaliado.
/// @param idLogado ID do utilizador logado.
*/
void avaliarEmprestimo(Feedback feedbacks[], int *totalFeedbacks, Operacao *loan, int idLogado);

/** 
/// @brief Lista os feedbacks de um utilizador avaliado.
/// @param feedbacks array de feedbacks.
/// @param totalFeedbacks número total de feedbacks.
/// @param idAvaliado ID do utilizador avaliado.
*/
void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado);

/**
 * @brief Verifica se o utilizador tem pedidos pendentes e mostra um alerta.
 */
void verificarNotificacoes(Operacao loans[], int totalLoans, int idLogado);

#endif // LOANS_H