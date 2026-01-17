#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
//#include "transactions.h"
#include "files.h"
//#include "interface.h"
#include "structs.h"
#include "books.h"


// =============================================================
// FUNÇÕES PRINCIPAIS
// =============================================================

void criarLivro(Livro books[], int *totalBooks, int idLogado) {
    if (*totalBooks >= MAX_LIVROS) {
        printf("[Erro] Limite de livros atingido.\n");
        esperarEnter();
        return;
    }

    limparEcra();
    printf("\n=== REGISTAR NOVO LIVRO ===\n");
    printf("(Digite '0' para CANCELAR)\n");

    char isbnTemp[20];
    char t[MAX_STRING], a[MAX_STRING], c[MAX_STRING];

    do {
        printf("Escreva o ISBN: ");
        lerString(isbnTemp, 20);
        
        if (strcmp(isbnTemp, "0") == 0) { 
            printf("[Info] Registo cancelado.\n"); 
            esperarEnter();
            return; 
        }

        if (strlen(isbnTemp) == 0) {
            printf("[Erro] O ISBN e obrigatorio.\n");
            continue;
        } 
                
        if (validarISBN(isbnTemp) == 0) { 
            printf("[Erro] O ISBN introduzido nao e valido matematicamente.\n");
            continue;
        }

        if (procurarISBNnaBaseDados(isbnTemp, t, a, c)) {
            printf("\n[LIVRO ENCONTRADO]\n");
            printf("--------------------------\n");
            printf("Titulo:    %s\n", t);
            printf("Autor:     %s\n", a);
            printf("Categoria: %s\n", c);
            printf("--------------------------\n");

            // --- NOVO BLOCO DE CONFIRMAÇÃO ---
            printf("Os dados acima estao corretos? (1-Sim / 0-Nao): ");
            int confirma = lerInteiro("", 0, 1);
            
            if (confirma == 0) {
                printf("[Info] Registo interrompido pelo utilizador.\n");
                continue; // Volta ao início do loop para tentar outro ISBN
            }

            // --- CONFIGURAÇÃO DA STRUCT ---
            // Gerar ID sequencial baseado no último ID existente (evita ID 0)
            int novoId = (*totalBooks > 0) ? books[*totalBooks - 1].id + 1 : 1;
            
            books[*totalBooks].id = novoId;
            strcpy(books[*totalBooks].isbn, isbnTemp);
            strcpy(books[*totalBooks].titulo, t);
            strcpy(books[*totalBooks].autor, a);
            strcpy(books[*totalBooks].categoria, c);
            
            // CORREÇÃO DOS IDs DE PROPRIEDADE
            books[*totalBooks].idProprietario = idLogado; // Identifica o dono real
            books[*totalBooks].idDetentor = idLogado;     // Identifica quem tem o livro agora
            
            // ESTADO INICIAL
            books[*totalBooks].estado = LIVRO_DISPONIVEL; // Usa a tua enum
            books[*totalBooks].numRequisicoes = 0;
            books[*totalBooks].eliminado = 0;

            // Incremento do contador global ANTES de usar no Log ou guardar
            (*totalBooks)++;

            // REGISTO DE LOG
            char detalhes[150];
            sprintf(detalhes, "ISBN: %s | Titulo: %s", isbnTemp, t);
            registarLog(idLogado, "LIVRO_ADD", detalhes);

            // GUARDAR NO FICHEIRO (Persistência imediata)
            guardarLivros(books, *totalBooks);

            printf("\n[OK] Livro registado com sucesso no seu inventario.\n");
            break; 
        } 
        else {
            char detalhesErro[100];
            sprintf(detalhesErro, "Tentativa falhada ISBN: %s (Nao consta na BD)", isbnTemp);
            registarLog(idLogado, "LIVRO_ADD_FAIL", detalhesErro);

            printf("[Erro] O ISBN %s nao consta na base de dados oficial.\n", isbnTemp);
            printf("Nao e permitido registar livros fora da base de dados.\n");
        }

    } while (1);

    esperarEnter();
}

/**
 * @brief Lista todos os livros do sistema (Catálogo Global).
 * Mostra se pertence ao Instituto ou a um Utilizador.
 */
void listarLivros(Livro books[], int totalBooks) {
    printf("\n%-4s | %-20s | %-15s | %-15s | %-10s\n", "ID", "Titulo", "Autor", "Categoria", "Estado");
    printf("---------------------------------------------------------------------------\n");
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].eliminado == 0) {
            printf("%-4d | %-20.20s | %-15.15s | %-15.15s | %-10d\n", 
                books[i].id, 
                books[i].titulo, 
                books[i].autor, 
                books[i].categoria, // Acesso direto aqui
                books[i].estado);
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
                campoParaPesquisar = books[i].categoria;
                break;
        }

        // Validação e Comparação
        if (campoParaPesquisar != NULL && strstr(campoParaPesquisar, termo) != NULL) {
            
            // No printf, usar obterNomeCategoria para imprimir texto e não número
            printf("- ID: %d | Titulo: %s | Autor: %s | Cat: %s | Dono: %d\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   books[i].categoria,
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

int eliminarLivro(Livro *book, int idProprietario) {   
    // 1. Segurança: Verificar dono
    if (book->idProprietario != idProprietario) {
        printf("[Erro] Apenas o dono pode eliminar este livro.\n");
        return 0;
    }

    // 2. Segurança: Verificar se o livro está "preso" em alguma operação
    if (book->estado == LIVRO_EMPRESTADO || book->idProprietario != book->idDetentor) {
        printf("[Bloqueado] O livro esta emprestado. Aguarde a devolucao.\n");
        return 0;
    }
    
    if (book->estado == LIVRO_RESERVADO) {
        printf("[Bloqueado] Existe uma proposta ativa para este livro. Rejeite-a primeiro.\n");
        return 0;
    }

    // 3. Confirmação
    printf("\n!!! CONFIRMACAO DE REMOCAO !!!\n");
    printf("Titulo: %s\n", book->titulo);
    int confirm = lerInteiro("Tem a certeza que quer remover este livro? (1-Sim / 0-Nao): ", 0, 1);
    
    if (confirm == 1) {
        book->eliminado = 1; 
        book->estado = LIVRO_INDISPONIVEL;

        // Log de eliminação
        char detalhesDel[150];
        sprintf(detalhesDel, "ISBN: %s | Titulo: %s (Removido)", book->isbn, book->titulo);
        registarLog(idProprietario, "LIVRO_DEL", detalhesDel);

        printf("[Sucesso] Livro removido da sua estante virtual.\n");
        return 1;
    }
    
    printf("[Cancelado] Operacao abortada.\n");
    return 0;
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

void transferirPosseLivro(Livro books[], int totalBooks, int idLivro, int novoDono) {
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].id == idLivro) {
            books[i].idProprietario = novoDono;
            books[i].idDetentor = novoDono;
            // Após uma troca, o livro deve ficar indisponível até o novo dono o disponibilizar
            books[i].estado = LIVRO_INDISPONIVEL; 
            break;
        }
    }
}

int procurarISBNnaBaseDados(char *isbnProcurado, char *titulo, char *autor, char *categoria) {
    FILE *file = fopen("data/isbn_database.txt", "r");
    if (!file) {
        printf("[Erro] Base de dados ISBN nao encontrada (isbn_database.txt).\n");
        return 0;
    }

    char linha[300];
    char isbnFicheiro[20], t[MAX_STRING], a[MAX_STRING], c[50];

    while (fgets(linha, sizeof(linha), file)) {
        // Ignorar linhas vazias ou comentários
        if (strlen(linha) < 10 || linha[0] == '#') continue;

        // Formato: ISBN;Titulo;Autor;Categoria
        // %[^;] lê tudo até ao ponto e vírgula
        if (sscanf(linha, "%[^;];%[^;];%[^;];%[^\n]", isbnFicheiro, t, a, c) == 4) {
            
            // Comparação exata do ISBN
            if (strcmp(isbnFicheiro, isbnProcurado) == 0) {
                strcpy(titulo, t);
                strcpy(autor, a);
                strcpy(categoria, c);
                
                fclose(file);
                return 1; // Encontrou e preencheu as variáveis
            }
        }
    }

    fclose(file);
    return 0; // Não existe na base de dados
}