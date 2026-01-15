#ifndef USERS_H
#define USERS_H

#include "structs.h"


void registarUtilizador(Utilizador users[], int *total, Operacao operacoes[], int totalOperacoes);


int loginUtilizador(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes);

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

#endif // USERS_H