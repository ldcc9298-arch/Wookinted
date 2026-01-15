#ifndef FILES_H
#define FILES_H

#include "structs.h"

// Utilizadores
int carregarUtilizadores(Utilizador users[]);
void guardarUtilizadores(Utilizador users[], int total);

// Livros
int carregarLivros(Livro books[]);
void guardarLivros(Livro books[], int total);

// Emprestimos
int carregarOperacoes(Operacao operacoes[]);
void guardarOperacoes(Operacao operacoes[], int total);

// Feedbacks
int carregarFeedbacks(Feedback feedbacks[]);
void guardarFeedbacks(Feedback feedbacks[], int total);

#endif // FILES_H