#include <stdio.h>
#include <string.h>
#include "files.h"
#include "structs.h"

/* =============================================================
   CARREGAMENTO DE DADOS (READ)
   ============================================================= */

int carregarUtilizadores(Utilizador users[]) {
    FILE *fp = fopen("data/users.dat", "rb"); // Abre para leitura binária (read binary)
    int total = 0;
    
    if (fp == NULL) return 0; // Se o ficheiro não existe, o sistema assume 0 registos

    // Primeiro lê o inteiro que indica quantos registos existem no ficheiro
    fread(&total, sizeof(int), 1, fp);
    
    // Verificação de segurança: impede o estouro do array caso o ficheiro esteja corrompido ou seja externo
    if (total > MAX_UTILIZADORES) {
        printf("[Aviso] Ficheiro contem mais utilizadores do que o permitido. A cortar.\n");
        total = MAX_UTILIZADORES;
    }

    // Se houver dados, lê o bloco completo de estruturas diretamente para o array na RAM
    if (total > 0) {
        fread(users, sizeof(Utilizador), total, fp);
    }
    
    fclose(fp); // Fecha o fluxo do ficheiro
    printf("[Sistema] Carregados %d utilizadores.\n", total);
    return total;
}

int carregarLivros(Livro books[]) {
    FILE *fp = fopen("data/books.dat", "rb");
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp);

    // Garante que a leitura respeita o limite definido na constante MAX_LIVROS
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

int carregarFeedbacks(Feedback feedbacks[]) {
    FILE *fp = fopen("data/feedbacks.dat", "rb"); 
    int total = 0;
    
    if (fp == NULL) return 0;

    fread(&total, sizeof(int), 1, fp); // Lê a contagem de feedbacks
    if (total > 0) {
        fread(feedbacks, sizeof(Feedback), total, fp); // Carrega o bloco de estruturas
    }
    
    fclose(fp);
    return total;
}

/* =============================================================
   PERSISTÊNCIA DE DADOS (WRITE)
   ============================================================= */

void guardarUtilizadores(Utilizador users[], int total) {
    FILE *fp = fopen("data/users.dat", "wb"); // Abre para escrita binária (write binary)
    
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel criar o ficheiro de utilizadores.\n");
        return;
    }

    // Grava o inteiro 'total' no início do ficheiro (Header) para orientar a leitura futura
    fwrite(&total, sizeof(int), 1, fp);

    // Grava todo o bloco de memória do array de uma só vez (muito eficiente)
    if (total > 0) {
        fwrite(users, sizeof(Utilizador), total, fp);
    }

    fclose(fp); // Garante que os dados são escritos fisicamente no disco
}

void guardarLivros(Livro books[], int total) {
    FILE *fp = fopen("data/books.dat", "wb");
    
    if (fp == NULL) {
        printf("[Erro] Nao foi possivel criar o ficheiro de livros.\n");
        return;
    }

    fwrite(&total, sizeof(int), 1, fp); // Escreve o número de livros ativos

    if (total > 0) {
        fwrite(books, sizeof(Livro), total, fp);
    }

    fclose(fp);
}

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

    fwrite(&total, sizeof(int), 1, fp); // Fundamental gravar o total primeiro

    if (total > 0) {
        fwrite(feedbacks, sizeof(Feedback), total, fp);
    }

    fclose(fp);
}

/* =============================================================
   CONSULTA A BASE DE DADOS EXTERNA (CSV/TXT)
   ============================================================= */

int procurarISBNnaBaseDados(char *isbnProcurado, char *titulo, char *autor, char *categoria) {
    FILE *file = fopen("data/isbn_database.txt", "r"); // Abre em modo texto para leitura
    if (!file) {
        printf("[Erro] Base de dados ISBN nao encontrada (isbn_database.txt).\n");
        return 0;
    }

    char linha[300]; // Buffer para armazenar cada linha do ficheiro
    char isbnFicheiro[20], t[MAX_STRING], a[MAX_STRING], c[50];

    // fgets lê até encontrar um '\n' ou atingir o tamanho máximo do buffer
    while (fgets(linha, sizeof(linha), file)) {
        // Ignora linhas que não tenham dados suficientes ou que comecem por comentário (#)
        if (strlen(linha) < 10 || linha[0] == '#') continue;

        // sscanf utiliza o formato %[^;] para ler toda a string até encontrar o delimitador ';'
        if (sscanf(linha, "%[^;];%[^;];%[^;];%[^\n]", isbnFicheiro, t, a, c) == 4) {
            
            // Comparação de strings para verificar se é o ISBN que o utilizador procura
            if (strcmp(isbnFicheiro, isbnProcurado) == 0) {
                strcpy(titulo, t); // Se encontrar, preenche os ponteiros de saída com os dados do ficheiro
                strcpy(autor, a);
                strcpy(categoria, c);
                
                fclose(file);
                return 1; // Sucesso na localização
            }
        }
    }

    fclose(file);
    return 0; // O ISBN não existe no catálogo oficial fornecido
}