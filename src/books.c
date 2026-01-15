#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
//#include "transactions.h"
//#include "files.h"
//#include "interface.h"
#include "structs.h"
#include "books.h"

/**
 * @brief Exibe um menu para o utilizador escolher uma categoria.
 * @return CategoriaLivro A categoria selecionada pelo utilizador.
 */
CategoriaLivro escolherCategoria() {
    printf("\n--- Selecione a Categoria ---\n");
    
    // Loop de 1 a 7
    for (int i = 1; i <= 7; i++) {
        printf("%d - %s\n", i, obterNomeCategoria((CategoriaLivro)i, ""));
    }
    printf("0 - Cancelar\n");

    int opcao = lerInteiro("Escolha uma opcao: ", 0, 7);
    return (CategoriaLivro)opcao;
}

// =============================================================
// FUNÇÕES PRINCIPAIS
// =============================================================

void criarLivro(Livro books[], int *totalBooks, int idLogado) {
    if (*totalBooks >= MAX_LIVROS) {
        printf("[Erro] Limite de livros atingido.\n");
        return;
    }

    limparEcra();
    printf("\n=== REGISTAR NOVO LIVRO ---\n");
    printf("(Digite '0' em qualquer campo para CANCELAR o registo)\n");

    Livro novo;
    
    // =========================================================
    // 1. TÍTULO
    // =========================================================
    do {
        printf("Titulo: ");
        lerString(novo.titulo, MAX_STRING);
        
        // 1. Verificação de Saída
        if (strcmp(novo.titulo, "0") == 0) { 
            printf("[Info] Registo cancelado.\n"); 
            return; 
        }

        // 2. Validação de Vazio
        if (strlen(novo.titulo) == 0) {
            printf("[Erro] O titulo e obrigatorio.\n");
        } else {
            break; // Válido
        }
    } while (1);


    // =========================================================
    // 2. AUTOR (CORRIGIDO)
    // =========================================================
    do {
        printf("Autor: ");
        lerString(novo.autor, MAX_STRING);

        // 1. Verificação de Saída
        if (strcmp(novo.autor, "0") == 0) { 
            printf("[Info] Registo cancelado.\n"); 
            return; 
        }

        // 2. Validação de Vazio
        if (strlen(novo.autor) == 0) {
            printf("[Erro] O autor e obrigatorio.\n");
        } else {
            break; // Válido
        }
    } while (1);


    // =========================================================
    // 3. ISBN
    // =========================================================
    do {
        printf("ISBN: ");
        lerString(novo.isbn, 20);
        
        // A. VERIFICAR CANCELAMENTO
        if (strcmp(novo.isbn, "0") == 0) { 
            printf("[Info] Registo cancelado.\n"); 
            return; 
        }

        // B. VERIFICAR VAZIO
        if (strlen(novo.isbn) == 0) {
            printf("[Erro] O ISBN e obrigatorio.\n");
        } 
        
        // C. VERIFICAR DUPLICADO
        else if (existeISBN(books, *totalBooks, novo.isbn)) {
            printf("[Erro] Este ISBN ja se encontra registado no sistema.\n");
        }
        
        // D. VERIFICAR MATEMÁTICA
        else if (validarISBN(novo.isbn) == 0) { 
            printf("[Erro] O ISBN introduzido nao e valido (formato ou digitos incorretos).\n");
        } 
        
        else {
            break; // Passou
        }
    } while (1);


    // =========================================================
    // 4. CATEGORIA
    // =========================================================
    printf("\n--- Selecione a Categoria ---\n");
    printf("1. Ficcao\n2. Nao Ficcao\n3. Ciencia\n4. Historia\n");
    printf("5. Biografia\n6. Tecnologia\n7. Outro (Requer Aprovacao)\n");
    
    int catOp = lerInteiro("Opcao (0 para cancelar): ", 0, 7);
    if (catOp == 0) {
        printf("[Info] Registo cancelado.\n");
        return;
    }

    novo.categoria = (CategoriaLivro)catOp;
    strcpy(novo.categoriaManual, ""); 

    // Se for OUTRO, pede especificação
    if (novo.categoria == CAT_OUTRO) {
        do {
            printf("Especifique a Categoria: ");
            lerString(novo.categoriaManual, 50);
            
            // Valida Saída
            if (strcmp(novo.categoriaManual, "0") == 0) {
                 printf("[Info] Registo cancelado.\n");
                 return;
            }

            // Valida Vazio
            if(strlen(novo.categoriaManual) == 0) {
                printf("[Erro] A especificacao manual e obrigatoria.\n");
            } else {
                break;
            }
        } while(1);

        novo.estado = LIVRO_PENDENTE_CAT; 
        printf("\n[Nota] O livro ficara PENDENTE de aprovacao.\n");

    } else {
        novo.estado = LIVRO_DISPONIVEL;
    }


    // =========================================================
    // 5. GRAVAR NA MEMÓRIA
    // =========================================================
    novo.id = (*totalBooks) + 1; 
    novo.idProprietario = idLogado;
    novo.idDetentor = idLogado;    
    novo.eliminado = 0;            
    novo.numRequisicoes = 0;

    books[*totalBooks] = novo;
    (*totalBooks)++;

    if (novo.estado == LIVRO_DISPONIVEL)
        printf("\n[Sucesso] Livro '%s' registado e disponivel!\n", novo.titulo);
    else
        printf("\n[Sucesso] Livro '%s' registado. Aguarde validacao do Admin.\n", novo.titulo);
        
    esperarEnter();
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
        if (books[i].eliminado == 0) {
            char estado[20];
            if (books[i].estado == LIVRO_INDISPONIVEL) strcpy(estado, "EMPRESTADO");
            else strcpy(estado, "DISPONIVEL");

            char donoStr[20];
            if (books[i].idProprietario == 0) strcpy(donoStr, "INSTITUTO");
            else sprintf(donoStr, "User %d", books[i].idProprietario);

            printf("%-4d | %-20.20s | %-15.15s | %-15.15s | %-15s\n",
                    books[i].id,
                    books[i].titulo,
                    books[i].autor,
                    books[i].isbn,
                    obterNomeCategoria(books[i].categoria, books[i].categoriaManual));
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
        // Ignora livros eliminados (eliminado == 1)
        if (books[i].eliminado == 1) continue;

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
                campoParaPesquisar = (char*)obterNomeCategoria(books[i].categoria, books[i].categoriaManual);
                break;
        }

        // Validação e Comparação
        if (campoParaPesquisar != NULL && strstr(campoParaPesquisar, termo) != NULL) {
            
            // No printf, usar obterNomeCategoria para imprimir texto e não número
            printf("- ID: %d | Titulo: %s | Autor: %s | Cat: %s | Dono: %d\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   obterNomeCategoria(books[i].categoria, books[i].categoriaManual),
                   books[i].idProprietario);
            
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
void editarLivro(Livro *book, int idProprietario) {
    // 1. Verificar propriedade
    if (book->idProprietario != idProprietario) {
        printf("Erro: Apenas o dono (User %d) pode editar este livro.\n", book->idProprietario);
        return;
    }

    // 2. Verificar posse (integração com empréstimos)
    if (book->idProprietario != book->idDetentor) {
        printf("Erro: O livro esta emprestado ao User %d. Recupere-o antes de editar.\n", book->idDetentor);
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

    printf("Categoria atual: %s.\n", obterNomeCategoria(book->categoria, book->categoriaManual));    
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
int eliminarLivro(Livro *book, int idProprietario) {   
    // 1. Verificar propriedade
    if (book->idProprietario != idProprietario) {
        printf("Erro: Apenas o dono pode eliminar este livro.\n");
        return 0;
    }

    // 2. Verificar posse
    if (book->idProprietario != book->idDetentor) {
        printf("Erro: Impossivel eliminar! O livro esta atualmente com o Utilizador %d.\n", book->idDetentor);
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
        book->eliminado = 1; // Soft Delete
        printf("[Sucesso] Livro removido do catalogo.\n");
        return 1; 
    } else {
        printf("[Cancelado] O livro mantem-se estado.\n");
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
        // Filtra por Dono e garante que não mostra livros apagados (eliminado == 1)
        if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
            
            char estado[40];
            // Lógica para mostrar onde o livro está fisicamente
            if (books[i].idDetentor == idLogado) {
                strcpy(estado, "Consigo (estado)");
            } else {
                sprintf(estado, "EMPRESTADO (User %d)", books[i].idDetentor);
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