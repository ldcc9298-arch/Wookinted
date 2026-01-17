#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include "structs.h"


void gerirPedidosPendentes(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado);

void listarEmprestimos(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, int idLogado);


void devolverLivro(Livro *book, Operacao operacoes[], int totalOperacoes, int idLogado);


void doarLivro(Livro books[], int totalBooks, int idProprietario, Operacao operacoes[], int *totalOperacoes);


void avaliarEmprestimo(Feedback feedbacks[], int *totalFeedbacks, Operacao *operacao, int idLogado);

void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado);


int jaAvaliou(Feedback feeds[], int totalFeeds, int idOperacao, int idAvaliador);

void adicionarOperacao(Operacao operacoes[], int *totalOperacoes, int idRequerente, int idLivro, TipoOperacao tipo, int dias);

void registarDevolucao(Operacao operacoes[], int totalOperacoes, Livro livros[], int totalLivros, int idLivro, int idUtilizador);

int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes);


#endif // TRANSACTIONS_H