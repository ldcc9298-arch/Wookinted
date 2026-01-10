#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"
#include "utils.h"
#include "loans.h"
#include "files.h"
#include "interface.h" // Se aplicável

// =============================================================
// GESTÃO DE CATEGORIAS
// =============================================================

// A ordem DEVE corresponder ao Enum CategoriaLivro no structs.h
const char *NOME_CATEGORIAS[] = {
    "Ficcao", 
    "Nao Ficcao", 
    "Ciencia", 
    "Historia",    
    "Biografia",   
    "Tecnologia", 
    "Outro"
};

/**
 * @brief Converte o Enum de categoria para String.
 */
const char* obterNomeCategoria(CategoriaLivro cat) {
    // Ajustar o limite conforme o numero de itens no array (0 a 6 neste caso)
    if (cat < 0 || cat > 6) return "Desconhecido";
    return NOME_CATEGORIAS[cat];
}

/**
 * @brief Menu auxiliar para escolher categoria.
 */
CategoriaLivro escolherCategoria() {
    printf("\n--- Selecione a Categoria ---\n");
    for (int i = 0; i <= 6; i++) {
        printf("%d - %s\n", i, NOME_CATEGORIAS[i]);
    }
    // Usa lerInteiro com limites seguros
    int opcao = lerInteiro("Escolha uma opcao: ", 0, 6);
    return (CategoriaLivro)opcao;
}

// =============================================================
// FUNÇÕES AUXILIARES
// =============================================================

/**
 * @brief Gera o próximo ID único para um novo livro.
 * Lógica: Percorre todos os livros e encontra o maior ID existente, retornando esse valor + 1.
 */
int gerarProximoId(Livro books[], int total) {
    int maxId = 0;
    for (int i = 0; i < total; i++) {
        // Verifica todos, mesmo os "retidos" (soft deleted), para evitar duplicar IDs antigos
        if (books[i].id > maxId) {
            maxId = books[i].id;
        }
    }
    return maxId + 1;
}

/**
 * @brief Imprime uma linha formatada de um livro.
 * Usado para evitar repetição de código nas pesquisas.
 */
void imprimirLinhaLivro(Livro *book) {
    char donoStr[20];
    if (book->userId == 0) strcpy(donoStr, "INSTITUTO");
    else sprintf(donoStr, "User %d", book->userId);

    printf("- ID: %d | ISBN: %-13s | Titulo: %-20.20s | Autor: %-15.15s | Cat: %s\n", 
           book->id,
           book->isbn,
           book->titulo, 
           book->autor, 
           obterNomeCategoria(book->categoria));
}

// =============================================================
// FUNÇÕES PRINCIPAIS
// =============================================================

/**
 * @brief Regista um novo livro no sistema.
 * Lógica:
 * - Gera um ID único baseado no maior ID existente.
 * - Associa o livro ao utilizador que o está a registar.
 * - Define o estado inicial como DISPONIVEL e retido como 0.
 */
void registarLivro(Livro books[], int total, int userId) {
    // 1. Geração de ID Seguro
    books[total].id = gerarProximoId(books, total);
    
    // 2. Definição de Propriedade e Posse Inicial
    books[total].userId = userId;
    books[total].userIdEmprestimo = userId;

    printf("Titulo: ");
    lerString(books[total].titulo, MAX_STRING);

    printf("Autor: ");
    lerString(books[total].autor, MAX_STRING);

    printf("ISBN (ex: 978-3-16-148410-0): ");
    lerString(books[total].isbn, 20);
    
    // 3. Seleção de Categoria
    books[total].categoria = escolherCategoria();

    printf("Ano: ");
    // Uso de lerInteiro para robustez
    books[total].anoPublicacao = lerInteiro("", 1000, 2030);

    // 4. Estados Iniciais
    books[total].retido = 0;
    books[total].Disponivel = DISPONIVEL;

    printf("Livro registado com ID %d na categoria '%s'!\n", 
            books[total].id, obterNomeCategoria(books[total].categoria));
}

/**
 * @brief Lista todos os livros do sistema (Catálogo Global).
 * Mostra se pertence ao Instituto ou a um Utilizador.
 */
void listarLivros(Livro books[], int total) {
    printf("\n--- Catalogo Geral de Livros ---\n");
    printf("%-4s | %-13s | %-20s | %-15s | %-10s | %-12s | %-10s\n", "ID", "ISBN", "Titulo", "Autor", "Categoria", "Dono", "Estado");
    printf("-------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0) {
            char estado[20];
            if (books[i].Disponivel == INDISPONIVEL) strcpy(estado, "EMPRESTADO");
            else strcpy(estado, "DISPONIVEL");

            char donoStr[20];
            if (books[i].userId == 0) strcpy(donoStr, "INSTITUTO");
            else sprintf(donoStr, "User %d", books[i].userId);

            printf("%-4d | %-13s | %-20.20s | %-15.15s | %-10s | %-12s | %-10s\n",
                   books[i].id,
                   books[i].isbn,
                   books[i].titulo,
                   books[i].autor,
                   obterNomeCategoria(books[i].categoria),
                   donoStr,
                   estado);
        }
    }
}

// =============================================================
// PESQUISA (DRY Implementation)
// =============================================================

/**
 * @brief Pesquisa livros por título ou autor.
 * Lógica Genérica:
 * - Itera sobre todos os livros ativos.
 * - Compara o termo com o campo selecionado (Titulo ou Autor).
 */
void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo) {
    int encontrados = 0;
    printf("\nResultados da pesquisa por '%s' (%s):\n", 
            termo, (tipo == PESQUISA_TITULO) ? "Titulo" : "Autor");

    for (int i = 0; i < total; i++) {
        // Ignora livros eliminados
        if (books[i].retido == 1) continue;

        // Seleção do ponteiro para o campo correto
        const char *campoParaPesquisar = (tipo == PESQUISA_TITULO) ? books[i].titulo : books[i].autor;

        if (strstr(campoParaPesquisar, termo) != NULL) {
            imprimirLinhaLivro(&books[i]);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum livro encontrado.\n");
    }
}

void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo) {
    pesquisarLivroGenerico(books, total, titulo, PESQUISA_TITULO);
}

void pesquisarLivroPorAutor(Livro books[], int total, const char *autor) {
    pesquisarLivroGenerico(books, total, autor, PESQUISA_AUTOR);
}

void pesquisarLivroPorISBN(Livro books[], int total, const char *isbn) {
    printf("\nResultados da pesquisa por ISBN '%s':\n", isbn);
    int encontrados = 0;
    
    for (int i = 0; i < total; i++) {
        // strcmp devolve 0 se as strings forem idênticas
        if (books[i].retido == 0 && strcmp(books[i].isbn, isbn) == 0) {
            imprimirLinhaLivro(&books[i]);
            encontrados++;
        }
    }
    
    if (encontrados == 0) printf("Nenhum livro encontrado com esse ISBN.\n");
}

/**
 * @brief Pesquisa livros por Categoria.
 * Solicita ao utilizador a categoria e lista os resultados.
 */
void pesquisarLivroPorCategoria(Livro books[], int total) {
    // 1. Pedir a categoria ao utilizador
    CategoriaLivro catAlvo = escolherCategoria();

    printf("\n--- Livros de %s ---\n", obterNomeCategoria(catAlvo));

    int encontrados = 0;
    for (int i = 0; i < total; i++) {
        // Verifica se o livro está ativo E se a categoria corresponde
        if (books[i].retido == 0 && books[i].categoria == catAlvo) {
            imprimirLinhaLivro(&books[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum livro encontrado nesta categoria.\n");
    }
}


// =============================================================
// AÇÕES DE UTILIZADOR
// =============================================================

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
    
    // Buffer para leituras
    char buffer[MAX_STRING];

    printf("Novo Titulo (Atual: %s) [Enter mantem]: ", book->titulo);
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->titulo, buffer);
    }

    printf("Novo Autor (Atual: %s) [Enter mantem]: ", book->autor);
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->autor, buffer);
    }

    printf("Novo Ano (Atual: %d): ", book->anoPublicacao);
    // Usamos lerInteiro que trata do buffer
    book->anoPublicacao = lerInteiro("", 1000, 2030);

    printf("Categoria atual: %s.\n", obterNomeCategoria(book->categoria));
    printf("Deseja alterar a categoria? (1-Sim, 0-Nao): ");
    int alterar = lerInteiro("", 0, 1);
    if (alterar) {
        book->categoria = escolherCategoria();
    }

    printf("Novo ISBN (Atual: %s) [Enter mantem]: ", book->isbn);
    // Reutilizamos o buffer declarado acima (sem re-declarar 'char buffer')
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->isbn, buffer);
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
    printf("%-5s | %-25s | %-15s | %s\n", "ID", "TITULO", "AUTOR", "ESTADO ATUAL");
    printf("----------------------------------------------------------------------\n");
    
    int cont = 0;
    for (int i = 0; i < total; i++) {
        // Filtra por Dono e garante que não mostra livros apagados (retido == 1)
        if (books[i].userId == idLogado && books[i].retido == 0) {
            
            char estado[40];
            // Lógica para mostrar onde o livro está fisicamente
            if (books[i].userIdEmprestimo == idLogado) {
                strcpy(estado, "Consigo (Disponivel)");
            } else {
                sprintf(estado, "EMPRESTADO (User %d)", books[i].userIdEmprestimo);
            }

            printf("%-5d | %-25.25s | %-15.15s | %s\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   estado);
            cont++;
        }
    }
    
    if (cont == 0) printf("    (Ainda nao registou livros)\n");
    printf("----------------------------------------------------------------------\n");
}