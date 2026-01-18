/**
 * @file users.h
 * @brief Definições e protótipos para a gestão de utilizadores e segurança.
 * @details Este ficheiro contém as interfaces para autenticação, validação de domínios
 * institucionais e geração de estatísticas de reputação.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef USERS_H
#define USERS_H

#include "structs.h"

/** @defgroup SEGURANCA Autenticação e Validação
 * @brief Funções de controlo de acesso e integridade de dados.
 * @{ 
 */

/**
 * @brief Valida se o email segue as regras do domínio IPCA (@alunos.ipca.pt ou @ipca.pt).
 * @param email String contendo o email a validar.
 * @return int Retorna 1 se válido, 0 caso contrário.
 */
int validarFormatoEmailIPCA(char *email);

/**
 * @brief Realiza o processo de login comparando credenciais e verificando estado da conta.
 * @param users Array de utilizadores.
 * @param totalUsers Total de registos.
 * @param email Email introduzido.
 * @param pass Palavra-passe introduzida.
 * @return int ID do utilizador se o login for bem-sucedido, ou códigos de erro negativos.
 */
int autenticarUtilizador(Utilizador users[], int totalUsers, char *email, char *pass);

/**
 * @brief Verifica a existência de um email e o seu estado de atividade.
 * @param users Array de utilizadores.
 * @param total Total de registos.
 * @param email Email a procurar.
 * @param index Ponteiro para armazenar a posição do utilizador no array.
 * @return int Estado da conta encontrado.
 */
int verificarStatusEmail(Utilizador users[], int total, char *email, int *index);

/**
 * @brief Garante que não existem números de telemóvel duplicados no sistema.
 * @param users Array de utilizadores.
 * @param total Total de registos.
 * @param telemovel String com o telemóvel a validar.
 * @param indexAtual Índice do utilizador atual (para permitir manter o mesmo número na edição).
 * @return int 1 se estiver em uso por outro utilizador, 0 caso contrário.
 */
int telemovelEmUso(Utilizador users[], int total, char *telemovel, int indexAtual);

/**
 * @brief Valida a identidade através de email e telemóvel para recuperação de conta.
 * @return int Índice do utilizador se os dados coincidirem, -1 caso contrário.
 */
int validarIdentidadeRecuperacao(Utilizador users[], int total, char *email, char *telemovel);

/**
 * @brief Cria a conta de Administrador padrão (ID 1) caso o sistema seja iniciado pela primeira vez.
 * @param users Array de utilizadores.
 * @param totalUsers Ponteiro para o contador global.
 */
void garantirAdminPadrao(Utilizador users[], int *totalUsers);

/** @} */

/** @defgroup GESTAO_PERFIL Edição e Manutenção de Conta
 * @brief Funções que alteram o estado ou dados do utilizador.
 * @{ 
 */

/** @brief Atualiza o nome de um utilizador na estrutura via ponteiro. */
void atualizarNomeUtilizador(Utilizador *user, char *novoNome);

/** @brief Atualiza a password de um utilizador na estrutura via ponteiro. */
void atualizarPasswordUtilizador(Utilizador *user, char *novaPass);

/** @brief Altera o estado da conta para Inativo (Soft Delete), mantendo os dados para histórico. */
void desativarContaUtilizador(Utilizador *user);

/** @} */

/** @defgroup ESTATISTICAS Motor de Reputação e Rankings
 * @brief Funções analíticas sobre o comportamento dos utilizadores.
 * @{ 
 */

/**
 * @brief Calcula a média aritmética das notas recebidas por um utilizador.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Total de registos.
 * @param idUser ID do utilizador alvo.
 * @param qtdReviews Ponteiro para retornar o número total de avaliações.
 * @return float Média de 1.0 a 5.0.
 */
float calcularMediaUtilizador(Feedback feedbacks[], int totalFeedbacks, int idUser, int *qtdReviews); 

/**
 * @brief Verifica se o utilizador possui qualquer registo em operações passadas.
 * @return int 1 se possui histórico, 0 caso contrário.
 */
int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes);

/**
 * @brief Gera e ordena os utilizadores por pontuação de feedback.
 * @param users Array de utilizadores.
 * @param totalUsers Total de registos.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Total de feedbacks.
 * @param ranking Array de structs auxiliares para armazenamento dos resultados.
 */
void calcularRankingReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, UserStats ranking[]);

/**
 * @brief Gera e ordena os utilizadores pelo volume de transações (Empréstimos/Trocas) concluídas.
 * @param users Array de utilizadores.
 * @param totalUsers Total de registos.
 * @param operacoes Array de histórico.
 * @param totalOperacoes Total de operações.
 * @param ranking Array de structs auxiliares para armazenamento dos resultados.
 */
void calcularAtividadeUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes, UserActivityRanking ranking[]);

/** @} */

#endif // USERS_H