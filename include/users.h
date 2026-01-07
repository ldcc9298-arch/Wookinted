#ifndef USERS_H
#define USERS_H
#include "structs.h"

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

/**
 * @brief Regista um novo utilizador no sistema.
 * @details Pede os dados ao utilizador, valida e guarda na memória.
 * * @param users Array de estruturas Utilizador (base de dados em memória).
 * @param total Apontador para o contador total de utilizadores.
 */
void registarUtilizador(Utilizador users[], int *total);

/**
 * @brief Realiza a autenticação de um utilizador com validação detalhada.
 * * @param users Array de utilizadores para pesquisa.
 * @param total Número total de utilizadores registados.
 * @return int Retorna:
 * >= 0 : Índice do utilizador (Login com Sucesso)
 * -1 : Email não encontrado
 * -2 : Password incorreta
 * -3 : Conta inativa/eliminada
 */
int loginUtilizador(Utilizador users[], int total);

/**
 * @brief Permite redefinir a password de um utilizador.
 * @details Solicita o email e, como medida de segurança, o número de telemóvel.
 * Se ambos corresponderem a um registo, permite definir uma nova password.
 * * @param users Array de utilizadores.
 * @param total Total de utilizadores registados.
 */
void recuperarPassword(Utilizador users[], int total);

/**
 * @brief Apresenta os dados do utilizador no ecrã.
 * @param user O utilizador a mostrar.
 */
void mostrarPerfil(Utilizador user);

/**
 * @brief Permite ao utilizador alterar o seu Nome e Password.
 * @param user Apontador para o utilizador (para podermos alterar os dados originais).
 */
void editarPerfil(Utilizador *user);

/**
 * @brief Desativa a conta do utilizador (Soft Delete).
 * @param user Apontador para o utilizador.
 * @return int Retorna 1 se a conta foi eliminada (para fazer logout), 0 se cancelou.
 */
int eliminarConta(Utilizador *user);

/**
 * @brief Gere o sub-menu de perfil (Ver, Editar, Eliminar).
 * @details Contém o loop do menu, inputs e chamadas de persistência.
 * * @param users Array completo (necessário para guardar alterações no ficheiro).
 * @param total Total de utilizadores.
 * @param idLogado Apontador para o ID atual (para poder fazer logout se eliminar conta).
 */
void gerirPerfil(Utilizador users[], int total, int *idLogado);

#endif // USERS_H