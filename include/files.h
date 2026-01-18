/**
 * @file files.h
 * @brief Definições e protótipos para a persistência de dados em ficheiros.
 * @details Este módulo gere a leitura e escrita binária dos dados do sistema,
 * garantindo a integridade da informação entre sessões de utilização.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef FILES_H
#define FILES_H

#include "structs.h"

/** @defgroup PERSISTENCIA Persistência de Dados (Binária)
 * @brief Funções responsáveis pela serialização e desserialização de estruturas em disco.
 * @{ 
 */

/* =============================================================
   GESTÃO DE UTILIZADORES
   ============================================================= */

/**
 * @brief Carrega os utilizadores do ficheiro binário para a memória.
 * @details Abre o ficheiro "utilizadores.dat" em modo de leitura binária.
 * @param users Array de destino onde os dados serão armazenados.
 * @return int Quantidade de utilizadores lidos com sucesso.
 */
int carregarUtilizadores(Utilizador users[]);

/**
 * @brief Guarda o array de utilizadores no ficheiro binário.
 * @details Sobrescreve o ficheiro "utilizadores.dat" com os dados atuais da memória.
 * @param users Array de utilizadores a persistir.
 * @param total Número atual de registos no array.
 */
void guardarUtilizadores(Utilizador users[], int total);

/* =============================================================
   GESTÃO DE LIVROS
   ============================================================= */

/**
 * @brief Carrega os livros do ficheiro binário para a memória.
 * @param books Array de destino para o catálogo de livros.
 * @return int Quantidade de livros carregados.
 */
int carregarLivros(Livro books[]);

/**
 * @brief Guarda o array de livros no ficheiro binário.
 * @param books Array contendo o catálogo de livros.
 * @param total Total de livros a registar.
 */
void guardarLivros(Livro books[], int total);

/* =============================================================
   GESTÃO DE OPERAÇÕES
   ============================================================= */

/**
 * @brief Carrega o histórico de operações do ficheiro binário.
 * @param operacoes Array de destino para as transações.
 * @return int Total de operações recuperadas.
 */
int carregarOperacoes(Operacao operacoes[]);

/**
 * @brief Guarda o histórico de operações no ficheiro binário.
 * @param operacoes Array com o histórico de transações.
 * @param total Quantidade de operações no histórico.
 */
void guardarOperacoes(Operacao operacoes[], int total);

/* =============================================================
   GESTÃO DE FEEDBACKS
   ============================================================= */

/**
 * @brief Carrega os feedbacks registados do ficheiro binário.
 * @param feedbacks Array de destino para as avaliações.
 * @return int Total de feedbacks carregados.
 */
int carregarFeedbacks(Feedback feedbacks[]);

/**
 * @brief Guarda os feedbacks no ficheiro binário.
 * @param feedbacks Array com as avaliações do sistema.
 * @param total Quantidade de feedbacks a guardar.
 */
void guardarFeedbacks(Feedback feedbacks[], int total);

/** @} */ // Fim do grupo PERSISTENCIA

/** @defgroup API_SIMULATION Simulação de Base de Dados Externa
 * @brief Funções de consulta a fontes de dados externas (Simulação de API).
 * @{
 */

/**
 * @brief Consulta a base de dados TXT para validar ISBN e obter metadados.
 * @details Esta função simula uma consulta a uma API externa (como Google Books) 
 * através da leitura de um ficheiro de texto indexado por ISBN.
 * @param isbnProcurado ISBN a validar.
 * @param titulo Buffer para armazenar o título encontrado.
 * @param autor Buffer para armazenar o autor encontrado.
 * @param categoria Buffer para armazenar a categoria encontrada.
 * @return int 1 se o ISBN existir na base de dados, 0 caso contrário.
 */
int procurarISBNnaBaseDados(char *isbnProcurado, char *titulo, char *autor, char *categoria);

/** @} */ // Fim do grupo API_SIMULATION

#endif // FILES_H