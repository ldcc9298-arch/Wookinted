/**
 * @file interface.h
 * @brief Protótipos das funções de interface e menus do sistema.
 * @details Este ficheiro contém os contratos para a exibição de menus, tabelas 
 * de dados e gestão visual dos fluxos de utilizador e administrador.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include "structs.h"

/** @defgroup MENUS Menus Principais e Navegação
 * @brief Funções que controlam o fluxo principal da aplicação.
 * @{
 */

/**
 * @brief Menu inicial para utilizadores não autenticados.
 * @return ID do utilizador após login ou 0 para sair.
 */
int menuModoVisitante(Utilizador users[], int *totalUsers, Operacao operacoes[], int totalOperacoes);

/**
 * @brief Exibe as opções do menu principal após o login.
 * @param nome Nome do utilizador logado para saudação.
 * @return Opção escolhida pelo utilizador.
 */
int mostrarMenuPrincipal(char *nome);

/**
 * @brief Fluxo de controlo exclusivo para o perfil de Administrador.
 */
void menuAdministrador(Utilizador users[], int *totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks);

/**
 * @brief Menu de navegação no mercado de trocas e empréstimos.
 */
void menuMercadoLivros(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado, Feedback feedbacks[], int totalFeedbacks);

/**
 * @brief Menu de gestão do inventário pessoal do utilizador.
 */
void menuMeusLivros(Livro books[], int *totalBooks, int idLogado);

/**
 * @brief Menu central de gestão de pedidos, devoluções e avaliações.
 */
void menuGestaoMovimentos(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks, int idLogado);

/**
 * @brief Menu de edição de dados pessoais e segurança da conta.
 * @return 1 se a conta for eliminada, 0 caso contrário.
 */
int menuGestaoPerfil(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado);

/** @} */ // Fim do grupo MENUS

/** @defgroup LISTAGENS Listagens e Consulta de Dados
 * @brief Funções de exibição tabular de informação.
 * @{
 */

/** @brief Imprime formatadamente os dados de um único livro. */
void imprimirLinhaLivro(Livro *book);

/** @brief Lista todos os livros do sistema com detalhes técnicos para o Admin. */
void listarStockAdmin(Livro books[], int totalBooks);

/** @brief Lista livros disponíveis para troca/empréstimo (excluindo os do próprio). */
void listarCatalogoMercado(Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado);

/** * @brief Lista os livros pertencentes ao utilizador logado.
 * @return Quantidade de livros listados.
 */
int listarTabelaMeusLivros(Livro books[], int totalBooks, int idLogado);

/** @brief Exibe resultados de pesquisa filtrados por termo e categoria. */
void listarResultadosPesquisa(Livro books[], int totalBooks, Utilizador users[], int totalUsers, char *termoBusca, int opPesquisa, char *termoOriginal);

/** @brief Lista o histórico de avaliações recebidas por um utilizador. */
void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado);

/** @brief Lista empréstimos ativos para controlo de prazos. */
void listarEmprestimos(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, int idLogado);

/** @brief Exibe o log completo de movimentos de um utilizador. */
void listarHistoricoCompleto(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado);

/** @} */ // Fim do grupo LISTAGENS

/** @defgroup SUBMENUS Submenus e Ações Específicas
 * @{
 */

/** @brief Menu de aprovação de contas e transações pelo Admin. */
void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks);

/** @brief Interface de entrada para os filtros de pesquisa. */
void submenuPesquisaLivros(Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado);

/** @brief Menu de acesso aos diversos relatórios estatísticos. */
void submenuRelatorios(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks);

/** @brief Processo visual de remoção de um livro do inventário. */
void submenuEliminarLivro(Livro books[], int totalBooks, int idLogado, int maxVisualId);

/** @brief Interface para selecionar e confirmar a devolução de um livro. */
void submenuDevolverLivro(Livro books[], int totalBooks, Operacao operacoes[], int *totalOperacoes, Utilizador users[], int totalUsers, int idLogado);

/** @brief Menu para atribuição de notas e comentários a transações concluídas. */
void submenuAvaliarTransacoes(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int *totalFeeds, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado);

/** @brief Listagem cronológica inversa das ações do utilizador. */
void submenuHistoricoPessoal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks, int idLogado);

/** @brief Log global do sistema para auditoria do Administrador. */
void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks);

/** @brief Menu para escolher um livro pessoal para oferecer numa troca. */
int submenuEscolherLivroTroca(Livro books[], int totalBooks, int idLogado);

/** @} */

/** @defgroup GESTAO_UTILIZADORES Gestão de Utilizadores (Interface)
 * @{
 */

/** @brief Interface de recolha de dados para nova conta. */
void registarUtilizador(Utilizador users[], int *total);

/** @brief Interface de autenticação. @return ID do utilizador ou 0 se falhar. */
int loginUtilizador(Utilizador users[], int totalUsers);

/** @brief Fluxo de recuperação de conta via email e telemóvel. */
void recuperarPassword(Utilizador users[], int total);

/** @brief Exibe os dados detalhados do perfil atual. */
void mostrarPerfil(Utilizador user);

/** @brief Menu principal de edição de perfil. */
void editarPerfil(Utilizador *user);

/** @brief Exibe cabeçalho de perfil com média de reputação. */
void mostrarCabecalhoPerfil(Utilizador *user, Feedback feedbacks[], int totalFeedbacks);

/** @brief Ação específica para mudar o nome da conta. */
void acaoAlterarNome(Utilizador *user, Utilizador allUsers[], int totalUsers);

/** @brief Ação específica para mudar a palavra-passe com confirmação. */
void acaoAlterarPassword(Utilizador *user, Utilizador allUsers[], int totalUsers);

/** @brief Ação específica para mudar o contacto telefónico. */
void acaoAlterarTelemovel(Utilizador *user, Utilizador allUsers[], int totalUsers);

/** @brief Fluxo de desativação de conta (Soft Delete). @return 1 se confirmado. */
int acaoEliminarConta(Utilizador *user, Utilizador allUsers[], int totalUsers);

/** @brief Interface de aprovação de novos registos para o Admin. */
void adminValidarUtilizadores(Utilizador users[], int totalUsers);

/** @} */

/** @defgroup NOTIFICACOES Alertas e Notificações
 * @{
 */

/** @brief Verifica e exibe alertas de prazos e novos pedidos para utilizadores. */
void verificarNotificacoes(Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks, int idLogado);

/** @brief Verifica pendências globais (utilizadores e doações) para o Admin. */
void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks, int idLogado);

/** @brief Exibe o ficheiro de logs técnicos do sistema. */
void exibirLogSistema();

/** @} */

/** @defgroup TRANSACCOES Processamento de Transações (Interface)
 * @{
 */

/** @brief Inicia o processo de pedido de empréstimo ou troca. */
void processarRequisicaoLivro(Livro books[], int *totalBooks, Utilizador users[], int totalUsers, Operacao operacoes[], int *totalOperacoes, int idLogado, Feedback feedbacks[], int totalFeedbacks);

/** @brief Menu de decisão para aceitar ou rejeitar pedidos recebidos. */
void gerirPedidosPendentes(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado);

/** @brief Lista os pedidos a aguardar resposta. @return Total de pedidos. */
int listarPedidosPendentes(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado, int mapeamentoOp[]);

/** @brief Processo de doação de um livro para a biblioteca do IPCA. */
void doarLivro(Livro books[], int totalBooks, int idLogado, Operacao operacoes[], int *totalOperacoes);

/** @} */

/** @defgroup RELATORIOS Relatórios Estatísticos
 * @{
 */

/** @brief Exibe ranking de utilizadores com melhor reputação. */
void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks);

/** @brief Exibe ranking de livros mais requisitados. */
void relatorioTopLivros(Livro livros[], int totalLivros);

/** @brief Exibe ranking de utilizadores com mais movimentos. */
void relatorioTopUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes);

/** @} */

#endif