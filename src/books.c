#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"
#include "utils.h"
#include "loans.h"
#include "files.h"
#include "interface.h"

/**
 * @brief Regista um novo livro no sistema.
 * Lógica:
 * - Gera um ID único baseado no total atual.
 * - Associa o livro ao utilizador que o está a registar.
 * - Define o estado inicial como DISPONIVEL e retido como 0 (não retido).
 */
void registarLivro(Livro books[], int total, int idLogado) {
    // Usamos uma variável auxiliar ou escrevemos diretamente no array.
    // Escrever diretamente poupa memória.
    printf("\n--- Registar Novo Livro ---\n");
    books[total].id = gerarProximoId(books, total); // 1. Geração do ID
    books[total].userId = idLogado; // 2. Associação ao Utilizador (Dono)
    books[total].userIdEmprestimo = idLogado; // Se userIdEmprestimo serve para dizer "com quem está o livro agora", começa com o dono.

    // 3. Dados do Livro
    printf("Titulo do Livro: ");
    lerString(books[total].titulo, MAX_STRING); 

    printf("Autor: ");
    lerString(books[total].autor, MAX_STRING);

    printf("Ano de Publicacao: ");
    while (scanf("%d", &books[total].anoPublicacao) != 1) {
        printf("Ano invalido. Insira um numero: ");
        limparBuffer(); // Limpa o enter inválido
    }
    limparBuffer(); // Limpa o enter final do scanf

    // 4. Estados Iniciais
    books[total].retido = 0; // 0 = Não retido (Disponível para troca)
    books[total].Disponivel = DISPONIVEL; 

    printf("\n[Sucesso] Livro '%s' (ID: %d) registado no inventario!\n", books[total].titulo, books[total].id);
}

/**
 * @brief Gera o próximo ID único para um novo livro.
 * Lógica: Percorre todos os livros e encontra o maior ID existente, retornando esse valor + 1.
 */
int gerarProximoId(Livro books[], int total) {
    int maxId = 0;
    for (int i = 0; i < total; i++) {
        // Verifica todos, mesmo os "retidos" (soft deleted), se eles ainda estiverem no array
        if (books[i].id > maxId) {
            maxId = books[i].id;
        }
    }
    return maxId + 1;
}

/**
 * @brief Lista todos os livros do sistema (Catálogo Global).
 * Mostra se pertence ao Instituto ou a um Utilizador.
 */
void listarLivros(Livro books[], int total) {
    printf("\n--- Catalogo Geral de Livros ---\n");
    printf("%-4s | %-20s | %-15s | %-10s | %-12s\n", "ID", "Titulo", "Autor", "Dono", "Estado");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0) {
            char estado[20];
            
            // Traduz o estado do Enum para texto legível
            if (books[i].Disponivel == INDISPONIVEL || books[i].Disponivel == EMPRESTADO) {
                strcpy(estado, "EMPRESTADO");
            } else {
                strcpy(estado, "DISPONIVEL");
            }

            // Identifica se o dono é a Instituição (ID 0) ou um User
            char donoStr[15];
            if (books[i].userId == 0) strcpy(donoStr, "IPCA");
            else sprintf(donoStr, "User %d", books[i].userId);

            printf("%-4d | %-20.20s | %-15.15s | %-10s | %-12s\n",
                   books[i].id,
                   books[i].titulo,
                   books[i].autor,
                   donoStr,
                   estado);
        }
    }
}

/**
 * @brief Imprime uma linha formatada de um livro.
 * Usado para evitar repetição de código nas pesquisas.
 */
void imprimirLinhaLivro(Livro *book) {
    char donoStr[20];
    if (book->userId == 0) strcpy(donoStr, "INSTITUTO");
    else sprintf(donoStr, "User %d", book->userId);

    printf("- ID: %d | Titulo: %-20s | Autor: %-15s | Dono: %s\n", 
           book->id, book->titulo, book->autor, donoStr);
}
/**
 * @brief Pesquisa livros por título ou autor.
 * Lógica:
 * - Itera sobre todos os livros.
 * - Verifica se o termo de pesquisa está contido no título ou autor, dependendo do tipo.
 * - Imprime os livros encontrados.
 */
void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo) {
    int encontrados = 0;
    printf("\nResultados da pesquisa por '%s' (%s):\n", 
            termo, (tipo == PESQUISA_TITULO) ? "Titulo" : "Autor");

    for (int i = 0; i < total; i++) {
        // Ignora livros eliminados (retido == 1)
        if (books[i].retido == 1) continue;

        // SELEÇÃO DO CAMPO:
        // Aponta para o titulo ou para o autor dependendo do tipo
        const char *campoParaPesquisar = (tipo == PESQUISA_TITULO) ? books[i].titulo : books[i].autor;

        // Verifica se o termo existe dentro desse campo
        if (strstr(campoParaPesquisar, termo) != NULL) {
            imprimirLinhaLivro(&books[i]);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum livro encontrado.\n");
    }
}

/**
 * @brief Pesquisa livros por título.
 */
void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo) {
    pesquisarLivroGenerico(books, total, titulo, PESQUISA_TITULO);
}

/**
 * @brief Pesquisa livros por autor.
 */
void pesquisarLivroPorAutor(Livro books[], int total, const char *autor) {
    pesquisarLivroGenerico(books, total, autor, PESQUISA_AUTOR);
}


/**
 * @brief Edita um livro.
 * Segurança: Impede edição se o livro não estiver na posse do dono.
 */
void editarLivro(Livro *book, int userId) {
    // 1. Verificar propriedade
    if (book->userId != userId) {
        printf("Erro: Apenas o dono (User %d) pode editar este livro.\n", book->userId);
        return;
    }

    // 2. Verificar posse (integração com empréstimos)
    if (book->userId != book->userIdEmprestimo) {
        printf("Erro: O livro esta emprestado ao User %d. Recupere-o antes de editar.\n", book->userIdEmprestimo);
        return;
    }

    printf("\n--- Editar Livro ID: %d ---\n", book->id);
    
    printf("Novo Titulo (Atual: %s): ", book->titulo);
    char buffer[MAX_STRING];
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') { // Só altera se o utilizador escrever algo
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->titulo, buffer);
    }

    printf("Novo Autor (Atual: %s): ", book->autor);
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->autor, buffer);
    }

    printf("Novo Ano (Atual: %d): ", book->anoPublicacao);
    int novoAno;
    // Tenta ler int, se falhar consome o buffer
    char check;
    if (scanf("%d%c", &novoAno, &check) != 2 || check != '\n') {
        // Se o user der apenas Enter ou input inválido, mantemos o ano atual (ou ignoramos)
        // Neste caso simples, assumimos que ele não quis mudar se deu enter
    } else {
        book->anoPublicacao = novoAno;
    }
    
    printf("Livro atualizado com sucesso!\n");
}

/**
 * @brief Elimina (Soft Delete) um livro.
 * Segurança: Impede eliminação se o livro estiver emprestado.
 */
int eliminarLivro(Livro *book, int userId) {   
    // 1. Verificar propriedade
    if (book->userId != userId) {
        printf("Erro: Apenas o dono pode eliminar este livro.\n");
        return 0;
    }

    // 2. Verificar posse
    if (book->userId != book->userIdEmprestimo) {
        printf("Erro: Impossivel eliminar! O livro esta atualmente com o Utilizador %d.\n", book->userIdEmprestimo);
        printf("Deve solicitar a devolucao antes de o remover do sistema.\n");
        return 0;
    }

    char confirmacao[15];
    printf("\n!!! ZONA DE PERIGO !!!\n");
    printf("Vai remover '%s' permanentemente.\n", book->titulo);
    printf("Escreva 'ELIMINAR' para confirmar: ");
    
    scanf("%s", confirmacao);
    limparBuffer(); 
    
    if (strcmp(confirmacao, "ELIMINAR") == 0) {
        book->retido = 1; // Soft Delete
        printf("[Sucesso] Livro removido do catalogo.\n");
        return 1; 
    } else {
        printf("[Cancelado] O livro mantem-se ativo.\n");
        return 0; 
    }
}

/**
 * @brief Lista apenas os livros do utilizador logado.
 * Mostra claramente se o livro está "Consigo" ou "Com Outro User".
 */
void listarMeusLivros(Livro books[], int total, int idLogado) {
    printf("\n--- O MEU INVENTARIO ---\n");
    printf("%-5s | %-30s | %-20s | %s\n", "ID", "TITULO", "AUTOR", "ESTADO");
    printf("----------------------------------------------------------------------\n");
    
    int cont = 0;
    for (int i = 0; i < total; i++) {
        // CORREÇÃO: idDono
        if (books[i].userId == idLogado) {
            printf("%-5d | %-30s | %-20s | %s\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   (books[i].retido == 0 ? "Disponivel" : "Emprestado/Retido"));
            cont++;
        }
    }
    
    if (cont == 0) printf("    (Ainda nao registou livros)\n");
    printf("----------------------------------------------------------------------\n");
}
