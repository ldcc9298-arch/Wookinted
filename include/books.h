#ifndef BOOKS_H
#define BOOKS_H
#include "structs.h"

/**
 * @brief Regista um novo livro no sistema.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param userId ID do utilizador que está a registar o livro.
 */
void registarLivro(Livro books[], int total, int userId);

/**
 * @brief Lista todos os livros disponíveis no sistema.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 */
void listarLivros(Livro books[], int total);

/**
 * @brief Pesquisa livros por título.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param titulo Título ou parte do título a pesquisar.
 */
void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo);

/**
 * @brief Pesquisa livros por autor.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param autor Nome ou parte do nome do autor a pesquisar.
 */
void pesquisarLivroPorAutor(Livro books[], int total, const char *autor);

/**
 * @brief Edita os detalhes de um livro.
 * @param book Apontador para o livro a editar.
 * @param userId ID do utilizador que está a tentar editar o livro.
 */
void editarLivro(Livro *book, int userId);

/**
 * @brief Elimina um livro do sistema (soft delete).
 * @param book Apontador para o livro a eliminar.
 * @param userId ID do utilizador que está a tentar eliminar o livro.
 * @return int Retorna 1 se o livro foi eliminado, 0 caso contrário.
 */
int eliminarLivro(Livro *book, int userId);

/**
 * @brief Lista os livros registados pelo utilizador.
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param userId ID do utilizador cujos livros serão listados.
 */
void listarMeusLivros(Livro books[], int total, int userId);




#endif // BOOKS_H