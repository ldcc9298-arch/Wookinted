#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "files.h"
#include "structs.h"
#include "books.h"

/* =============================================================
   LÓGICA DE GESTÃO DE IDENTIFICADORES E DADOS
   ============================================================= */

int gerarProximoId(Livro livros[], int total) {
    int maxId = 0; // Variável para controlar o maior ID encontrado
    for (int i = 0; i < total; i++) { // Ciclo que percorre todos os livros no array
        if (livros[i].id > maxId) { // Se o ID do livro atual for maior que o máximo guardado
            maxId = livros[i].id; // Atualizamos o máximo com o ID atual
        }
    }
    return maxId + 1; // Retornamos o novo ID (maior encontrado + 1) para garantir que é único
}

void finalizarInsercaoLivro(Livro books[], int *totalBooks, int idLogado, char *isbn, char *titulo, char *autor, char *categoria) {
    // Cálculo do novo ID baseado na posição imediatamente anterior para manter a sequência
    int novoId = (*totalBooks > 0) ? books[*totalBooks - 1].id + 1 : 1;
    
    // Atribuição dos dados passados por argumento para a struct no índice atual do array
    books[*totalBooks].id = novoId;
    strcpy(books[*totalBooks].isbn, isbn);
    strcpy(books[*totalBooks].titulo, titulo);
    strcpy(books[*totalBooks].autor, autor);
    strcpy(books[*totalBooks].categoria, categoria);
    
    // Configuração de propriedade: o utilizador logado é o dono e o detentor inicial
    books[*totalBooks].idProprietario = idLogado; 
    books[*totalBooks].idDetentor = idLogado;     
    
    // Definição dos estados padrão para um livro recém-criado
    books[*totalBooks].estado = LIVRO_DISPONIVEL; 
    books[*totalBooks].numRequisicoes = 0; // Inicia sem registo de popularidade
    books[*totalBooks].eliminado = 0; // Flag de segurança (0 = ativo no sistema)

    // Formatação de string para o log de auditoria do sistema
    char detalhes[150];
    sprintf(detalhes, "Livro %s adicionado.", titulo);
    registarLog(idLogado, "LIVRO_ADD", detalhes); // Registo da ação para o administrador

    (*totalBooks)++; // Incrementamos o contador real de livros na memória
    guardarLivros(books, *totalBooks); // Chamada imediata da persistência para não perder dados

    printf("\n[OK] Livro registado com sucesso no seu inventario.\n");
}

/* =============================================================
   FUNÇÕES DE INTERAÇÃO (CRIAÇÃO E REMOÇÃO)
   ============================================================= */

void criarLivro(Livro books[], int *totalBooks, int idLogado) {
    // Verificação de segurança para não ultrapassar o limite de memória do array (MAX_LIVROS)
    if (*totalBooks >= MAX_LIVROS) {
        printf("[Erro] Limite de livros atingido.\n");
        esperarEnter();
        return;
    }

    limparEcra();
    printf("\n=== REGISTAR NOVO LIVRO ===\n");
    printf("(Digite '0' para CANCELAR)\n");

    // Declaração de buffers temporários para validação antes de inserir na struct
    char isbnTemp[20];
    char t[MAX_STRING], a[MAX_STRING], c[MAX_STRING];

    do {
        printf("Escreva o ISBN: ");
        lerString(isbnTemp, 20); // Leitura segura de string com limite de caracteres
        
        if (strcmp(isbnTemp, "0") == 0) { // Mecanismo de cancelamento por parte do utilizador
            printf("[Info] Registo cancelado.\n"); 
            esperarEnter();
            return; 
        }

        if (strlen(isbnTemp) == 0) { // Impede que o campo fique vazio
            printf("[Erro] O ISBN e obrigatorio.\n");
            continue;
        } 
                
        if (validarISBN(isbnTemp) == 0) { // Validação do algoritmo matemático do ISBN
            printf("[Erro] O ISBN introduzido nao e valido matematicamente.\n");
            continue;
        }

        // Consulta à base de dados externa (ficheiro TXT) para recuperar dados automáticos
        if (procurarISBNnaBaseDados(isbnTemp, t, a, c)) {
            printf("\n[LIVRO ENCONTRADO]\n");
            printf("--------------------------\n");
            printf("Titulo:    %s\n", t);
            printf("Autor:     %s\n", a);
            printf("Categoria: %s\n", c);
            printf("--------------------------\n");

            printf("Os dados acima estao corretos? (1-Sim / 0-Nao): ");
            int confirma = lerInteiro("", 0, 1); // Confirmação do utilizador
            
            if (confirma == 0) { // Se os dados da BD não forem os esperados, permite trocar ISBN
                printf("[Info] Registo interrompido pelo utilizador.\n");
                continue; 
            }

            // Se tudo estiver OK, chama a função de inserção física no array
            finalizarInsercaoLivro(books, totalBooks, idLogado, isbnTemp, t, a, c);
            break; 
        } 
        else {
            // Caso o ISBN não exista no TXT, gera um log de erro para monitorização
            char detalhesErro[100];
            sprintf(detalhesErro, "Registo falhado: ISBN %s inexistente na BD", isbnTemp);
            registarLog(idLogado, "LIVRO_ADD_FAIL", detalhesErro);

            printf("[Erro] O ISBN %s nao consta na base de dados oficial.\n", isbnTemp);
            printf("Nao e permitido registar livros fora da base de dados.\n");
        }

    } while (1); // Ciclo continua até haver sucesso ou cancelamento

    esperarEnter();
}

int eliminarLivro(Livro *book, int idProprietario) {   
    // Regra de Negócio: Apenas o dono (ID) pode remover o seu registo
    if (book->idProprietario != idProprietario) {
        printf("[Erro] Apenas o dono pode eliminar este livro.\n");
        return 0;
    }

    // Validação de Estado: Impede remover livros que estão fisicamente com outros (emprestados)
    if (book->estado == LIVRO_EMPRESTADO || book->idProprietario != book->idDetentor) {
        printf("[Bloqueado] O livro esta emprestado. Aguarde a devolucao.\n");
        return 0;
    }
    
    // Validação de Transação: Impede remover livros que têm propostas de troca ou reserva pendentes
    if (book->estado == LIVRO_RESERVADO) {
        printf("[Bloqueado] Existe uma proposta ativa para este livro. Rejeite-a primeiro.\n");
        return 0;
    }

    printf("\n!!! CONFIRMACAO DE REMOCAO !!!\n");
    printf("Titulo: %s\n", book->titulo);
    int confirm = lerInteiro("Tem a certeza que quer remover este livro? (1-Sim / 0-Nao): ", 0, 1);
    
    if (confirm == 1) { // Implementação de "Soft Delete"
        book->eliminado = 1; // Marcamos como eliminado mas mantemos o registo para histórico/logs
        book->estado = LIVRO_INDISPONIVEL;

        char detalheRem[100];
        sprintf(detalheRem, "Livro %s removido.", book->titulo);
        registarLog(idProprietario, "LIVRO_REMOVE", detalheRem);

        printf("[Sucesso] Livro removido da sua estante virtual.\n");
        return 1;
    }
    
    printf("[Cancelado] Operacao abortada.\n");
    return 0;
}

void processarDoacao(Livro books[], int idxEncontrado, int totalBooks) {
    // A doação transfere a posse total para o ID 1 (Administração/Instituição)
    books[idxEncontrado].idProprietario = 1; 
    books[idxEncontrado].idDetentor = 1;
    books[idxEncontrado].estado = LIVRO_DISPONIVEL; 

    guardarLivros(books, totalBooks); // Atualização imediata no ficheiro binário

    printf("\n[Sucesso] O livro foi doado ao IPCA. Obrigado pela sua generosidade!\n");
}

/* =============================================================
   FUNÇÕES DE PESQUISA E VISUALIZAÇÃO
   ============================================================= */

void listarLivros(Livro books[], int totalBooks) {
    printf("\n%-4s | %-20s | %-15s | %-15s | %-10s\n", "ID", "Titulo", "Autor", "Categoria", "Estado");
    printf("---------------------------------------------------------------------------\n");
    for (int i = 0; i < totalBooks; i++) { // Itera sobre o array completo
        if (books[i].eliminado == 0) { // Só mostra se a flag de "soft delete" for 0 (ativo)
            printf("%-4d | %-20.20s | %-15.15s | %-15.15s | %-10d\n", 
                books[i].id, 
                books[i].titulo, 
                books[i].autor, 
                books[i].categoria, 
                books[i].estado);
        }
    }
}

void pesquisarLivroGenerico(Livro books[], int total, const char *termo, TipoPesquisa tipo) {
    int encontrados = 0;
    const char *labelTipo;

    // Tradução do ENUM de pesquisa para uma string legível no cabeçalho
    switch (tipo) {
        case PESQUISA_TITULO: labelTipo = "Titulo"; break;
        case PESQUISA_AUTOR: labelTipo = "Autor"; break;
        case PESQUISA_CATEGORIA: labelTipo = "Categoria"; break;
        default: labelTipo = "Geral";
    }

    printf("\nResultados da pesquisa por '%s' (%s):\n", termo, labelTipo);

    for (int i = 0; i < total; i++) {
        if (books[i].eliminado == 1) continue; // Salta livros marcados como apagados

        const char *campoParaPesquisar = NULL;
        
        // Seleção do campo da struct a comparar com base no tipo de pesquisa escolhido
        switch (tipo) {
            case PESQUISA_TITULO: campoParaPesquisar = books[i].titulo; break;
            case PESQUISA_AUTOR: campoParaPesquisar = books[i].autor; break;
            case PESQUISA_CATEGORIA: campoParaPesquisar = books[i].categoria; break;
        }

        // Uso da função strstr para verificar se o termo existe dentro da string do campo
        if (campoParaPesquisar != NULL && strstr(campoParaPesquisar, termo) != NULL) {
            printf("- ID: %d | Titulo: %s | Autor: %s | Cat: %s | Dono: %d\n", 
                   books[i].id, books[i].titulo, books[i].autor, books[i].categoria, books[i].idProprietario);
            encontrados++;
        }
    }

    if (encontrados == 0) printf("Nenhum livro encontrado.\n");
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

void listarMeusLivros(Livro books[], int total, int idLogado) {
    printf("\n--- O MEU INVENTARIO ---\n");
    printf("%-5s | %-25s | %-15s | %s\n", "ID", "TITULO", "AUTOR", "ESTADO ATUAL");
    printf("----------------------------------------------------------------------\n");
    
    int cont = 0;
    for (int i = 0; i < total; i++) {
        // Filtragem: deve pertencer ao utilizador logado e não pode estar eliminado
        if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
            
            char estado[40];
            // Lógica de localização: verifica se o dono é quem tem o livro fisicamente (idDetentor)
            if (books[i].idDetentor == idLogado) {
                strcpy(estado, "Consigo (estado)");
            } else {
                sprintf(estado, "EMPRESTADO (User %d)", books[i].idDetentor);
            }

            printf("%-5d | %-25.25s | %-15.15s | %s\n", 
                   books[i].id, books[i].titulo, books[i].autor, estado);
            cont++;
        }
    }
    
    if (cont == 0) printf("    (Ainda nao registou livros)\n");
    printf("----------------------------------------------------------------------\n");
}

/* =============================================================
   FUNÇÕES DE ORDENAÇÃO
   ============================================================= */

void ordenarLivrosPorRequisicoes(Livro livros[], int total, Livro *ptrs[]) {
    // 1. Preenchimento de um array de ponteiros para evitar mover as structs originais (mais rápido e seguro)
    for(int i = 0; i < total; i++) {
        ptrs[i] = &livros[i];
    }

    // 2. Implementação do algoritmo Bubble Sort para ordenar de forma descendente
    for(int i = 0; i < total - 1; i++) {
        for(int j = 0; j < total - i - 1; j++) {
            // Comparação baseada no contador numRequisicoes
            if(ptrs[j]->numRequisicoes < ptrs[j+1]->numRequisicoes) {
                // Troca apenas o endereço de memória no array de ponteiros
                Livro *temp = ptrs[j]; 
                ptrs[j] = ptrs[j+1]; 
                ptrs[j+1] = temp;
            }
        }
    }
}