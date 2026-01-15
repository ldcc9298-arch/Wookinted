#ifndef INTERFACE_H
#define INTERFACE_H

#include "structs.h"

// --- LOGIN & MENU PRINCIPAL ---

/**
 * @brief Apresenta o menu de visitante (Login, Registo, Recuperar).
 * @return Retorna o ID do utilizador se fizer login, -1 se continuar visitante, ou -10 para Sair do programa.
 */
int menuModoVisitante(Utilizador users[], int *totalUsers, Operacao operacoes[], int totalOperacoes);

/**
 * @brief Mostra o menu principal do utilizador logado e pede a opção.
 * @param nome Nome do utilizador para personalizar a mensagem.
 * @return Retorna o número da opção escolhida pelo utilizador.
 */
int mostrarMenuPrincipal(char *nome);

// --- SUB-MENU VALIDACOES ADMINISTRADOR ---

void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes);

// --- MENU ADMINISTRADOR ---

/**
 * @brief Gere o menu do Administrador (Validar Utilizadores).
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 */
void menuAdministrador(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks);

// --- SUB-MENU PESQUISA LIVROS ---

/**
 * @brief Submenu para pesquisar livros por título, autor ou categoria.
 */
void submenuPesquisaLivros(Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado);

// --- MENU MERCADO ---

/**
 * @brief Gere o menu do Mercado de Livros (Pesquisa, Requisição, Doação).
 * @param books Array de livros.
 * @param totalBooks Apontador para o total de livros (pode mudar se houver doação).
 * @param operacoes Array de empréstimos (necessário para requisitar).
 * @param totalOperacoes Apontador para o total de empréstimos.
 * @param idLogado ID do utilizador atual.
 */
void menuMercadoLivros(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado);

// --- MENU MEUS LIVROS ---

/**
 * @brief Gere o menu de gestão de inventário pessoal (Listar, Registar, Editar, Eliminar).
 */
void menuMeusLivros(Livro books[], int *totalBooks, int idLogado);

// --- MENU MOVIMENTOS ---

/**
 * @brief Menu principal para gerir devoluções, aceitar pedidos e dar feedback.
 */
void menuGestaoMovimentos(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks, int idLogado);

// --- MENU PERFIL ---

/**
 * @brief Gere o sub-menu de perfil (Ver, Editar, Eliminar).
 * @details Contém o loop do menu, inputs e chamadas de persistência.
 * * @param users Array completo (necessário para guardar alterações no ficheiro).
 * @param total Total de utilizadores.
 * @param idLogado Apontador para o ID atual (para poder fazer logout se eliminar conta).
 */
void menuGestaoPerfil(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado);

void verificarNotificacoes(Operacao operacoes[], int totalOperacoes, int idLogado);

void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes);



#endif // INTERFACE_H