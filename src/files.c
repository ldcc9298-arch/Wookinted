#include <stdio.h>
#include "files.h"

/**
 * @brief Carrega os utilizadores do ficheiro binário.
 * @param users Array onde os utilizadores serão carregados.
 * @return Número total de utilizadores carregados.
 */
int carregarUtilizadores(Utilizador users[]) {
    FILE *fp = fopen("data/users.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0; // Ficheiro não existe (primeira execução)

    fread(&total, sizeof(int), 1, fp); // Ler quantidade
    if (total > 0) {
        fread(users, sizeof(Utilizador), total, fp); // Ler array
    }
    
    fclose(fp);
    return total;
}

/**
 * @brief Guarda os utilizadores no ficheiro binário.
 * @param users Array de utilizadores a serem guardados.
 * @param total Número total de utilizadores.
 */
void guardarUtilizadores(Utilizador users[], int total) {
    FILE *fp = fopen("data/users.dat", "wb");
    if (fp == NULL) {
        printf("Erro critico: Nao foi possivel escrever em data/users.dat\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(users, sizeof(Utilizador), total, fp);
    fclose(fp);
}

/**
 * @brief Carrega os livros do ficheiro binário.
 * @param books Array onde os livros serão carregados.
 * @return Número total de livros carregados.
 */
int carregarLivros(Livro books[]) {
    FILE *fp = fopen("data/books.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);
    if (total > 0) {
        fread(books, sizeof(Livro), total, fp);
    }
    
    fclose(fp);
    return total;
}

/**
 * @brief Guarda os livros no ficheiro binário.
 * @param books Array de livros a serem guardados.
 * @param total Número total de livros.
 */
void guardarLivros(Livro books[], int total) {
    FILE *fp = fopen("data/books.dat", "wb");
    if (fp == NULL) {
        printf("Erro critico: Nao foi possivel escrever em data/books.dat\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(books, sizeof(Livro), total, fp);
    fclose(fp);
}

/**
 * @brief Carrega os empréstimos do ficheiro binário.
 * @param loans Array onde os empréstimos serão carregados.
 * @return Número total de empréstimos carregados.
 */
int carregarEmprestimos(Emprestimo loans[]) {
    FILE *fp = fopen("data/loans.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);
    if (total > 0) {
        fread(loans, sizeof(Emprestimo), total, fp);
    }
    
    fclose(fp);
    return total;
}

/**
 * @brief Guarda os empréstimos no ficheiro binário.
 * @param loans Array de empréstimos a serem guardados.
 * @param total Número total de empréstimos.
 */
void guardarEmprestimos(Emprestimo loans[], int total) {
    FILE *fp = fopen("data/loans.dat", "wb");
    if (fp == NULL) {
        printf("Erro critico: Nao foi possivel escrever em data/loans.dat\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(loans, sizeof(Emprestimo), total, fp);
    fclose(fp);
}

/**
 * @brief Carrega os feedbacks do ficheiro binário.
 * @param feedbacks Array onde os feedbacks serão carregados.
 * @return Número total de feedbacks carregados.
 */
int carregarFeedbacks(Feedback feedbacks[]) {
    FILE *fp = fopen("data/feedbacks.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);
    if (total > 0) {
        fread(feedbacks, sizeof(Feedback), total, fp);
    }
    
    fclose(fp);
    return total;
}

/**
 * @brief Guarda os feedbacks no ficheiro binário.
 * @param feedbacks Array de feedbacks a serem guardados.
 * @param total Número total de feedbacks.
 */
void guardarFeedbacks(Feedback feedbacks[], int total) {
    FILE *fp = fopen("data/feedbacks.dat", "wb");
    if (fp == NULL) {
        printf("Erro critico: Nao foi possivel escrever em data/feedbacks.dat\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(feedbacks, sizeof(Feedback), total, fp);
    fclose(fp);
}