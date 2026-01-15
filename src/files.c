#include <stdio.h>
#include <string.h>
#include "files.h"
#include "structs.h"

int carregarUtilizadores(Utilizador users[]) {
    FILE *fp = fopen("data/users.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0; // Ficheiro não existe (primeira vez)

    // 1. Ler a quantidade total
    fread(&total, sizeof(int), 1, fp);
    
    // Proteção: Se o ficheiro tiver mais users que o permitido, lemos apenas o máximo
    if (total > MAX_UTILIZADORES) {
        printf("[Aviso] Ficheiro contem mais utilizadores do que o permitido. A cortar.\n");
        total = MAX_UTILIZADORES;
    }

    // 2. Ler o array
    if (total > 0) {
        fread(users, sizeof(Utilizador), total, fp);
    }
    
    fclose(fp);
    printf("[Sistema] Carregados %d utilizadores.\n", total);
    return total;
}

int carregarLivros(Livro books[]) {
    FILE *fp = fopen("data/books.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);

    // Proteção de limites
    if (total > MAX_LIVROS) {
        total = MAX_LIVROS;
    }

    if (total > 0) {
        fread(books, sizeof(Livro), total, fp);
    }
    
    fclose(fp);
    printf("[Sistema] Carregados %d livros.\n", total);
    return total;
}

int carregarOperacoes(Operacao operacoes[]) {
    FILE *fp = fopen("data/operations.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);

    // Proteção de limites
    if (total > MAX_OPERACOES) {
        total = MAX_OPERACOES;
    }

    if (total > 0) {
        fread(operacoes, sizeof(Operacao), total, fp);
    }
    
    fclose(fp);
    printf("[Sistema] Carregados %d movimentos.\n", total);
    return total;
}

/**
 * @brief Carrega os feedbacks do ficheiro binário.
 * @param feedbacks Array onde os feedbacks serão carregados.
 * @return Número total de feedbacks carregados.
 */
int carregarFeedbacks(Feedback feedbacks[]) {
    // Certifica-te que o caminho e extensão batem certo com o teu sistema de ficheiros
    // Se usas "data/" antes, a pasta "data" tem de existir.
    FILE *fp = fopen("data/feedbacks.dat", "rb"); 
    int total = 0;
    
    if (fp == NULL) return 0; // Ficheiro não existe (primeira execução)

    fread(&total, sizeof(int), 1, fp); // 1. Ler a quantidade total
    if (total > 0) {
        // Opção de Segurança: Garantir que não ultrapassa o limite do array
        // if (total > MAX_FEEDBACKS) total = MAX_FEEDBACKS; 
        
        fread(feedbacks, sizeof(Feedback), total, fp); // 2. Ler o array
    }
    
    fclose(fp);
    return total;
}


void guardarUtilizadores(Utilizador users[], int total) {
    FILE *fp = fopen("data/users.dat", "wb"); // Modo de escrita binária
    
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel criar o ficheiro de utilizadores.\n");
        return;
    }

    // 1. Escrever a quantidade total (Cabeçalho do ficheiro)
    fwrite(&total, sizeof(int), 1, fp);

    // 2. Escrever o array de dados
    if (total > 0) {
        fwrite(users, sizeof(Utilizador), total, fp);
    }

    fclose(fp);
}

// Guarda o total de livros seguido do array de estruturas
void guardarLivros(Livro books[], int total) {
    FILE *fp = fopen("data/books.dat", "wb");
    
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel criar o ficheiro de livros.\n");
        return;
    }

    fwrite(&total, sizeof(int), 1, fp);

    if (total > 0) {
        fwrite(books, sizeof(Livro), total, fp);
    }

    fclose(fp);
}

// Guarda o total de empréstimos seguido do array de estruturas
void guardarOperacoes(Operacao operacoes[], int total) {
    FILE *fp = fopen("data/operations.dat", "wb");
    
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel criar o ficheiro de emprestimos.\n");
        return;
    }

    fwrite(&total, sizeof(int), 1, fp);

    if (total > 0) {
        fwrite(operacoes, sizeof(Operacao), total, fp);
    }

    fclose(fp);
}

void guardarFeedbacks(Feedback feedbacks[], int total) {
    FILE *fp = fopen("data/feedbacks.dat", "wb");
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel guardar feedbacks.\n");
        return;
    }

    // 1. Escreve o total PRIMEIRO (Fundamental para a leitura funcionar)
    fwrite(&total, sizeof(int), 1, fp);

    // 2. Escreve o array DEPOIS
    if (total > 0) {
        fwrite(feedbacks, sizeof(Feedback), total, fp);
    }

    fclose(fp);
}

/**
 * @brief Verifica se um email está na lista de emails permitidos.
 * @param email Email a ser verificado.
 * @return 1 se o email estiver na lista, 0 caso contrário.
 */
int emailExisteNaWhitelist(char *email) {
    FILE *f = fopen("whitelist.txt", "r");
    if (f == NULL) return 0; // Se não houver ficheiro, assumimos que não está na lista

    char linha[100];
    while (fgets(linha, sizeof(linha), f)) {
        // Remover o \n do final da linha lida
        linha[strcspn(linha, "\n")] = 0;
        
        if (strcmp(linha, email) == 0) {
            fclose(f);
            return 1; // Encontrado!
        }
    }
    fclose(f);
    return 0; // Não encontrado
}

