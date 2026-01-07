#ifndef INTERFACE_H
#define INTERFACE_H
#include "structs.h"

// --- UTILIZADORES ---

/**
 * @brief Apresenta o menu de visitante (Login, Registo, Recuperar).
 * @return Retorna o ID do utilizador se fizer login, -1 se continuar visitante, ou -10 para Sair do programa.
 */
int menuModoVisitante(Utilizador users[], int *totalUsers);

/**
 * @brief Mostra o menu principal do utilizador logado e pede a opção.
 * @param nome Nome do utilizador para personalizar a mensagem.
 * @return Retorna o número da opção escolhida pelo utilizador.
 */
int mostrarMenuPrincipal(char *nome);

// --- LIVROS ---

/**
 * @brief Gere o menu do Mercado de Livros (Pesquisa, Requisição, Doação).
 * @param books Array de livros.
 * @param totalBooks Apontador para o total de livros (pode mudar se houver doação).
 * @param loans Array de empréstimos (necessário para requisitar).
 * @param totalLoans Apontador para o total de empréstimos.
 * @param idLogado ID do utilizador atual.
 */
void menuMercadoLivros(Livro books[], int *totalBooks, Emprestimo loans[], int *totalLoans, int idLogado);

/**
 * @brief Gere o menu de gestão de inventário pessoal (Listar, Registar, Editar, Eliminar).
 */
void menuMeusLivros(Livro books[], int *totalBooks, int idLogado);

// --- TRANSAÇÕES ---

/**
 * @brief Menu principal para gerir devoluções, aceitar pedidos e dar feedback.
 */
void menuGestaoMovimentos(Emprestimo loans[], int *totalLoans, Livro books[], int *totalBooks, Feedback feedbacks[], int *totalFeedbacks, int idLogado);


#endif // INTERFACE_H