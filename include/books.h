/**
 * @file books.h
 * @brief Definições e protótipos para a gestão do catálogo de livros.
 * @details Este ficheiro define a interface pública para manipulação do inventário,
 * pesquisa de títulos e gestão de metadados dos livros no sistema.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef BOOKS_H
#define BOOKS_H

#include "structs.h"

/** @defgroup GESTAO_LIVROS Gestão de Inventário e Identidade
 * @brief Funções para criação, ID e remoção de livros.
 * @{
 */

/**
 * @brief Gera um identificador único incremental para novos livros.
 * @details Percorre o array para encontrar o maior ID atual e incrementa 1.
 * @param books Array de livros para análise.
 * @param total Contagem atual de registos.
 * @return int O próximo ID inteiro disponível.
 */
int gerarProximoId(Livro books[], int total);

/**
 * @brief Orquestra o menu de criação de um novo livro (via teclado ou ISBN).
 * @param books Array onde o livro será inserido.
 * @param totalBooks Ponteiro para atualização do contador global.
 * @param idProprietario ID do utilizador que está a registar o livro.
 */
void criarLivro(Livro books[], int *totalBooks, int idProprietario);

/**
 * @brief Remove logicamente um livro do sistema (Soft Delete).
 * @param book Ponteiro para a estrutura a alterar.
 * @param idProprietario ID para validação de segurança (apenas o dono pode eliminar).
 * @return int 1 se removido com sucesso, 0 se falhar ou se houver empréstimos ativos.
 */
int eliminarLivro(Livro *book, int idProprietario);

/**
 * @brief Finaliza o preenchimento da estrutura Livro e regista no log do sistema.
 * @param books Array de livros.
 * @param totalBooks Ponteiro para o total (será incrementado).
 * @param idLogado ID do utilizador proprietário.
 * @param isbn String com o ISBN validado.
 * @param titulo Título do livro.
 * @param autor Autor da obra.
 * @param categoria Categoria literária.
 */
void finalizarInsercaoLivro(Livro books[], int *totalBooks, int idLogado, char *isbn, char *titulo, char *autor, char *categoria);

/**
 * @brief Transfere a propriedade definitiva de um livro para o sistema (IPCA).
 * @param books Array de livros.
 * @param idxEncontrado Índice do livro no array.
 * @param totalBooks Total de livros no sistema.
 */
void processarDoacao(Livro books[], int idxEncontrado, int totalBooks);

/** @} */

/** @defgroup PESQUISA_LIVROS Motor de Busca e Listagens
 * @brief Funções para localização e ordenação de livros no catálogo.
 * @{
 */

/**
 * @brief Lista o catálogo global de todos os livros que não foram eliminados.
 * @param books Array de livros.
 * @param total Total de registos no sistema.
 */
void listarLivros(Livro books[], int total);

/**
 * @brief Lista o inventário pessoal filtrado por proprietário.
 * @param books Array de livros.
 * @param total Total de registos.
 * @param idProprietario ID do utilizador cujos livros queremos ver.
 */
void listarMeusLivros(Livro books[], int total, int idProprietario);

/**
 * @brief Motor de busca genérico que filtra o catálogo.
 * @param books Array de livros.
 * @param total Total de registos.
 * @param termo String contendo o texto a procurar.
 * @param tipo Enum (TITULO, AUTOR, CATEGORIA) que define o campo de busca.
 */
void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo);

/** @brief Atalho para pesquisa filtrada pelo campo Título. */
void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo);

/** @brief Atalho para pesquisa filtrada pelo campo Autor. */
void pesquisarLivroPorAutor(Livro books[], int total, const char *autor);

/** @brief Atalho para pesquisa filtrada pelo campo Categoria. */
void pesquisarLivroPorCategoria(Livro books[], int total, const char *categoria);

/**
 * @brief Ordena uma lista de referências (ponteiros) por popularidade.
 * @details Utiliza o campo numRequisicoes para ordenar de forma decrescente.
 * @param livros Array original de livros (não é alterado).
 * @param total Quantidade total de livros.
 * @param ptrs Array de ponteiros que será ordenado para exibição.
 */
void ordenarLivrosPorRequisicoes(Livro livros[], int total, Livro *ptrs[]);

/** @} */

#endif // BOOKS_H