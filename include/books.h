#ifndef BOOKS_H
#define BOOKS_H

#include "structs.h"

/**
 * @brief Regista um novo livro no sistema.
 * Solicita os dados ao utilizador (título, autor, ISBN, categoria, ano) e guarda no array.
 * @param books Array de livros.
 * @param total Número total de livros registados (índice atual).
 * @param userId ID do utilizador que está a registar o livro.
 */
void registarLivro(Livro books[], int total, int userId);

/**
 * @brief Gera o próximo ID único para um novo livro.
 * Evita conflitos de IDs caso livros antigos tenham sido eliminados.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @return int Próximo ID disponível (maior ID encontrado + 1).
 */
int gerarProximoId(Livro books[], int total);

/**
 * @brief Lista todos os livros disponíveis no sistema (Catálogo Global).
 * Mostra detalhes como ID, ISBN, Título, Autor, Categoria e Dono.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 */
void listarLivros(Livro books[], int total);

/**
 * @brief Imprime uma linha formatada com os dados de um livro.
 * Função auxiliar usada para normalizar a saída nas listagens e pesquisas.
 * @param book Apontador para o livro a imprimir.
 */
void imprimirLinhaLivro(Livro *book);

/**
 * @brief Converte o Enum de categoria para uma String legível.
 * @param cat Valor do Enum CategoriaLivro.
 * @return const char* Nome da categoria (ex: "Ficcao").
 */
const char* obterNomeCategoria(CategoriaLivro cat);

/**
 * @brief Exibe um menu para o utilizador escolher uma categoria.
 * @return CategoriaLivro A categoria selecionada pelo utilizador.
 */
CategoriaLivro escolherCategoria();

/**
 * @brief Pesquisa livros de forma genérica (por título ou autor).
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param termo Termo de pesquisa (string a procurar).
 * @param tipo Define se a pesquisa é no TITULO ou no AUTOR.
 */
void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo);

/**
 * @brief Pesquisa livros por título.
 * Wrapper para a função genérica.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param titulo Título (ou parte dele) a pesquisar.
 */
void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo);

/**
 * @brief Pesquisa livros por autor.
 * Wrapper para a função genérica.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param autor Nome do autor (ou parte dele) a pesquisar.
 */
void pesquisarLivroPorAutor(Livro books[], int total, const char *autor);

/**
 * @brief Pesquisa livros pelo código ISBN exato.
 * @param books Array de livros.
 * @param total Número total de livros.
 * @param isbn String com o ISBN a procurar.
 */
void pesquisarLivroPorISBN(Livro books[], int total, const char *isbn);

/**
 * @brief Pesquisa e lista todos os livros de uma determinada categoria.
 * Solicita ao utilizador qual a categoria que deseja filtrar.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 */
void pesquisarLivroPorCategoria(Livro books[], int total);

/**
 * @brief Edita os detalhes de um livro existente.
 * Permite alterar título, autor, ISBN, ano e categoria. Verifica se o utilizador é o dono.
 * @param book Apontador para o livro a editar.
 * @param userId ID do utilizador que está a tentar editar (para validação de permissões).
 */
void editarLivro(Livro *book, int userId);

/**
 * @brief Elimina um livro do sistema (Soft Delete).
 * Marca o livro como 'retido' em vez de o apagar da memória.
 * @param book Apontador para o livro a eliminar.
 * @param userId ID do utilizador que está a tentar eliminar (para validação).
 * @return int Retorna 1 se o livro foi eliminado com sucesso, 0 caso contrário.
 */
int eliminarLivro(Livro *book, int userId);

/**
 * @brief Lista apenas os livros pertencentes ao utilizador logado.
 * Indica o estado de cada livro (se está com o dono ou emprestado a alguém).
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param userId ID do utilizador logado.
 */
void listarMeusLivros(Livro books[], int total, int userId);

#endif // BOOKS_H