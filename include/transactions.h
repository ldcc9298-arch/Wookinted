#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include "structs.h"

/** 
/// @brief Solicita uma operação de empréstimo ou troca.
/// @param operacoes array de empréstimos.
/// @param totalOperacoes número total de empréstimos.
/// @param book livro alvo.
/// @param idRequisitante ID do utilizador que está a pedir o livro.
*/
void solicitarEmprestimo(Operacao operacoes[], int *totalOperacoes, Livro *book, int idRequisitante);


void gerirPedidosPendentes(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado);


void listarEmprestimos(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, int idLogado);


void devolverLivro(Livro *book, Operacao operacoes[], int totalOperacoes, int idLogado);


void doarLivro(Livro books[], int totalBooks, int idProprietario, Operacao operacoes[], int *totalOperacoes);


void avaliarEmprestimo(Feedback feedbacks[], int *totalFeedbacks, Operacao *operacao, int idLogado);

/** 
/// @brief Lista os feedbacks de um utilizador avaliado.
/// @param feedbacks array de feedbacks.
/// @param totalFeedbacks número total de feedbacks.
/// @param idAvaliado ID do utilizador avaliado.
*/
void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado);


int jaAvaliou(Feedback feeds[], int totalFeeds, int idOperacao, int idAvaliador);

void adicionarOperacao(Operacao operacoes[], int *totalOperacoes, int idRequerente, int idLivro, TipoOperacao tipo, int dias);

void registarDevolucao(Operacao operacoes[], int totalOperacoes, Livro livros[], int totalLivros, int idLivro, int idUtilizador);

#endif // TRANSACTIONS_H