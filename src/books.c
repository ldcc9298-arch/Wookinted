#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "transactions.h"
#include "files.h"
#include "interface.h"
#include "structs.h"

// =============================================================
// CATEGORIAS DE LIVROS
// =============================================================

/**
 * @brief Nomes das categorias de livros.
 */
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
 * @brief Obtém o nome da categoria a partir do enum.
 * @param cat Categoria do livro.
 * @return Nome da categoria como string.
 */
const char* obterNomeCategoria(CategoriaLivro cat) {
    // Ajustar o limite conforme o numero de itens no array (0 a 6 neste caso)
    if (cat < 0 || cat > 6) return "Desconhecido";
    return NOME_CATEGORIAS[cat];
}

/**
 * @brief Permite ao utilizador escolher uma categoria de livro.
 * @return CategoriaLivro selecionada.
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
// FUNÇÕES PRINCIPAIS
// =============================================================

void registarLivro(Livro books[], int total, int userId) {
    // 1. Geração de ID (Usar total + 1 se não tiveres a função gerarProximoId, ou manter a tua)
    books[total].id = total + 1; 
    
    // 2. Definição de Propriedade
    books[total].userId = userId;
    books[total].userIdEmprestimo = userId;
    books[total].numRequisicoes = 0;

    printf("Titulo: ");
    lerString(books[total].titulo, MAX_STRING);

    printf("Autor: ");
    lerString(books[total].autor, MAX_STRING);

    // Validação ISBN
    int isbnValido = 0;
    do {
        printf("ISBN (13 digitos): ");
        lerString(books[total].isbn, 20);
        if (validarISBN(books[total].isbn)) isbnValido = 1;
        else printf("[Erro] ISBN invalido.\n");
    } while (!isbnValido);

    // 3. Seleção de Categoria
    books[total].categoria = escolherCategoria(); // Retorna um Enum (0 a 6)

    // Lógica para "OUTRO" (Assumindo que OUTRO é o último valor do Enum, ex: 6)
    if(books[total].categoria == OUTRO) {
        
        printf("Especifique a Categoria (Apenas letras): ");
        // Validação de apenas letras
        int catValida = 0;
        do {
            lerString(books[total].categoriaManual, 50); // <--- Campo novo na struct
            if(validarApenasLetras(books[total].categoriaManual, 50)) { // Função auxiliar
                catValida = 1;
            } else {
                printf("[Erro] Apenas letras sao permitidas. Tente novamente: ");
            }
        } while(!catValida);

        // Define estado como PENDENTE para o Admin aprovar
        books[total].Disponivel = PENDENTE_CATEGORIA; 
        books[total].retido = 0;

        printf("\n[Aviso] Categoria '%s' registada.\n", books[total].categoriaManual);
        printf("O livro ficou PENDENTE de validacao pelo Administrador.\n");

    } else {
        // Caso Normal
        books[total].Disponivel = DISPONIVEL;
        books[total].retido = 0;
        
        // Limpar o campo manual para não ter lixo
        strcpy(books[total].categoriaManual, ""); 

        printf("[Sucesso] Livro registado na categoria '%s'.\n", obterNomeCategoria(books[total].categoria));
    }
}

/**
 * @brief Lista todos os livros do sistema (Catálogo Global).
 * Mostra se pertence ao Instituto ou a um Utilizador.
 */
void listarLivros(Livro books[], int total) {
    printf("\n--- Catalogo Geral de Livros ---\n");
    printf("%-4s | %-20s | %-15s | %-15s | %-15s\n", "ID", "TITULO", "AUTOR", "ISBN", "CATEGORIA");
    printf("-------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0) {
            char estado[20];
            if (books[i].Disponivel == INDISPONIVEL) strcpy(estado, "EMPRESTADO");
            else strcpy(estado, "DISPONIVEL");

            char donoStr[20];
            if (books[i].userId == 0) strcpy(donoStr, "INSTITUTO");
            else sprintf(donoStr, "User %d", books[i].userId);

            printf("%-4d | %-20.20s | %-15.15s | %-15.15s | %-15s\n",
                    books[i].id,
                    books[i].titulo,
                    books[i].autor,
                    books[i].isbn,
                    obterNomeCategoria(books[i].categoria));
        }
    }
}

// =============================================================
// PESQUISA DE LIVROS
// =============================================================

/**
 * @brief Pesquisa livros de forma genérica (por título, autor ou categoria).
 * @param books Array de livros.
 * @param total Número total de livros registados.
 * @param termo Termo de pesquisa (string a procurar).
 * @param tipo Define se a pesquisa é no TITULO, AUTOR ou CATEGORIA.
 */
void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo) {
    int encontrados = 0;
    
    // Definir o nome do tipo para o print inicial
    const char *labelTipo;
    switch (tipo) {
        case PESQUISA_TITULO: labelTipo = "Titulo"; break;
        case PESQUISA_AUTOR: labelTipo = "Autor"; break;
        case PESQUISA_CATEGORIA: labelTipo = "Categoria"; break;
        default: labelTipo = "Geral";
    }

    printf("\nResultados da pesquisa por '%s' (%s):\n", termo, labelTipo);

    for (int i = 0; i < total; i++) {
        // Ignora livros eliminados (retido == 1)
        if (books[i].retido == 1) continue;

        // Seleção do campo correto
        const char *campoParaPesquisar = NULL;
        
        switch (tipo) {
            case PESQUISA_TITULO: 
                campoParaPesquisar = books[i].titulo; 
                break;
            case PESQUISA_AUTOR: 
                campoParaPesquisar = books[i].autor; 
                break;
            case PESQUISA_CATEGORIA: 
                // Converter o Enum para String para poder pesquisar
                campoParaPesquisar = obterNomeCategoria(books[i].categoria); 
                break;
        }

        // Validação e Comparação
        if (campoParaPesquisar != NULL && strstr(campoParaPesquisar, termo) != NULL) {
            
            // No printf, usar obterNomeCategoria para imprimir texto e não número
            printf("- ID: %d | Titulo: %s | Autor: %s | Cat: %s | Dono: %d\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   obterNomeCategoria(books[i].categoria),
                   books[i].userId);
            
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

void pesquisarLivroPorCategoria(Livro books[], int total, const char *categoria) {
    pesquisarLivroGenerico(books, total, categoria, PESQUISA_CATEGORIA);
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

    printf("Categoria atual: %s.\n", obterNomeCategoria(book->categoria));
    printf("Deseja alterar a categoria? (1-Sim, 0-Nao): ");
    int alterar = lerInteiro("", 0, 1);
    if (alterar) {
        book->categoria = escolherCategoria();
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