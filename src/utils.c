#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // Necessário para isspace
#include <time.h>  // Necessário para calcularIdade
#include "utils.h"
#include "structs.h"
#include "books.h"

/**
 * @brief Limpa o buffer de entrada para evitar lixo.
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Lê uma string do stdin de forma segura.
 * @param texto Ponteiro para o buffer onde a string será armazenada.
 * @param tamanho Tamanho máximo do buffer.
 */
void lerString(char *texto, int tamanho) {
    if (fgets(texto, tamanho, stdin) != NULL) {
        size_t len = strlen(texto);
        if (len > 0 && texto[len-1] == '\n') {
            texto[len-1] = '\0';
        } else {
            limparBuffer();
        }
    }
}

/**
 * @brief Lê um número inteiro do stdin com validação.
 * @return int O número inteiro lido.
 */
int lerInteiro(char *mensagem, int min, int max) {
    int valor;
    do {
        printf("%s", mensagem);
        // O scanf deve ter validação para não entrar em loop infinito com letras
        if (scanf("%d", &valor) != 1) {
            limparBuffer(); // Limpa o "lixo" se não for número
            printf("[Erro] Insira um numero valido.\n");
            continue;
        }
        limparBuffer(); // Limpa o \n que sobra
        
        if (valor < min || valor > max) {
            printf("[Erro] Valor deve estar entre %d e %d.\n", min, max);
        }
    } while (valor < min || valor > max);
    return valor;
}

/**
 * @brief Limpa o ecrã/terminal.
 */
void limparEcra() {
    // Verifica se estamos em Windows (_WIN32) ou Mac/Linux
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief Pausa a execução até o utilizador pressionar ENTER.
 */
void esperarEnter() {
    printf("\n[Pressione ENTER para continuar...]");
    limparBuffer(); // Garante que não há lixo a atrapalhar
    getchar();      // Espera efetivamente pelo Enter
}

/**
 * @brief Verifica se uma string não está vazia (contém pelo menos um caractere não espaço).
 * @param str A string a verificar.
 * @return 1 se não estiver vazia, 0 caso contrário.
 */
int stringNaoVazia(char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    
    // Verifica se existe pelo menos um caractere que não seja espaço
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 1; // Encontrou um caractere válido
        }
    }
    return 0; // Só tem espaços
}

/**
 * @brief Calcula a idade com base na data de nascimento.
 * @param dataNasc String no formato "DD/MM/AAAA".
 * @return Idade em anos ou -1 se o formato for inválido.
 */
int calcularIdade(char *dataNasc) {
    int dia, mes, ano;
    
    // 1. Tentar ler 3 inteiros da string "DD/MM/AAAA"
    // O sscanf retira os numeros do texto. Retorna 3 se conseguir ler os 3.
    if (sscanf(dataNasc, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        return -1; // Formato errado
    }

    // 2. Obter a data atual do sistema
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    int anoAtual = tm.tm_year + 1900; // time.h conta anos desde 1900
    int mesAtual = tm.tm_mon + 1;     // time.h conta meses de 0 a 11
    int diaAtual = tm.tm_mday;

    // 3. Validação básica de limites (opcional mas recomendada)
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano > anoAtual) return -1;

    // 4. Cálculo da idade
    int idade = anoAtual - ano;

    // Se ainda não fez anos este ano, retira 1 à idade
    if (mesAtual < mes || (mesAtual == mes && diaAtual < dia)) {
        idade--;
    }

    return idade;
}

// Retorna 1 se o nome for válido (sem números), 0 se inválido
int validarApenasLetras(char *str, int tamanho) {
    (void)tamanho; // Evita warning de variável não usada

    if (str == NULL || strlen(str) == 0) return 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        // Se encontrar um dígito (0-9), falha imediatamente
        if (isdigit((unsigned char)str[i])) {
            return 0;
        }
        // Permitir apenas letras, espaços e alguns sinais de pontuação comuns (ex: hífen, apóstrofo)
        if (ispunct(str[i]) && str[i] != '-') {
            return 0;
        }
    }
    return 1;
}

// Retorna 1 se o email parecer válido (tem @ e . e não tem espaços)
int validarFormatoEmail(char *str) {
    if (str == NULL || strlen(str) < 5) return 0; // a@b.c = 5 chars min

    // 1. Não pode ter espaços
    if (strchr(str, ' ') != NULL) return 0;

    // 2. Tem de ter um '@'
    char *arroba = strchr(str, '@');
    if (arroba == NULL) return 0;

    // 3. Tem de ter um '.' DEPOIS do '@'
    // strchr procura a partir do ponteiro do arroba
    char *ponto = strchr(arroba, '.'); 
    if (ponto == NULL) return 0;
    
    // 4. Não pode ser o último caracter (ex: "joao@ipca.")
    if (ponto == str + strlen(str) - 1) return 0;

    return 1;
}

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

/**
 * @brief Relatório dos 5 utilizadores com mais transações.
 */
void relatorioTopTransacoes(Utilizador users[], int totalUsers, Operacao loans[], int totalLoans) {
    // Cria array temporário para contagem
    typedef struct { int idUser; int count; } Rank;
    Rank ranking[MAX_USERS];

    // Inicializar
    for(int i=0; i<totalUsers; i++) {
        ranking[i].idUser = users[i].id;
        ranking[i].count = 0;
    }

    // Contar transações (User é origem ou destino)
    for(int i=0; i<totalLoans; i++) {
        // Encontra o índice do user no array ranking (assumindo id sequencial users[id-1])
        // Se os IDs não forem sequenciais, terias de fazer um loop de busca.
        // Assumindo sequencial para simplificar:
        int u1 = loans[i].userId - 1; 
        int u2 = loans[i].userIdEmprestimo - 1;
        if(u1 >= 0 && u1 < totalUsers) ranking[u1].count++;
        if(u2 >= 0 && u2 < totalUsers && u2 != 0) ranking[u2].count++; // Ignora ID 0 (Instituição) se quiseres
    }

    // Bubble Sort simples (Descendente)
    for(int i=0; i<totalUsers-1; i++) {
        for(int j=0; j<totalUsers-i-1; j++) {
            if(ranking[j].count < ranking[j+1].count) {
                Rank temp = ranking[j]; ranking[j] = ranking[j+1]; ranking[j+1] = temp;
            }
        }
    }

    printf("\n--- TOP 5 UTILIZADORES ---\n");
    for(int i=0; i<5 && i<totalUsers; i++) {
        if(ranking[i].count == 0) break; // Não mostra users com 0
        int uid = ranking[i].idUser;
        // Busca nome direto no array original (uid-1)
        printf("%d. %s (ID: %d) - %d Transacoes\n", i+1, users[uid-1].nome, uid, ranking[i].count);
    }
}

/**
 * @brief Relatório dos 5 livros mais requisitados.
 */
void relatorioTopLivros(Livro books[], int totalBooks) {
    // Vamos criar um array de ponteiros para não duplicar dados pesados, apenas ordenar referências
    Livro *ptrs[MAX_BOOKS];
    for(int i=0; i<totalBooks; i++) ptrs[i] = &books[i];

    // Sort por numRequisicoes
    for(int i=0; i<totalBooks-1; i++) {
        for(int j=0; j<totalBooks-i-1; j++) {
            if(ptrs[j]->numRequisicoes < ptrs[j+1]->numRequisicoes) {
                Livro *temp = ptrs[j]; ptrs[j] = ptrs[j+1]; ptrs[j+1] = temp;
            }
        }
    }

    printf("\n--- TOP 5 LIVROS MAIS PROCURADOS ---\n");
    for(int i=0; i<5 && i<totalBooks; i++) {
        if(ptrs[i]->numRequisicoes == 0) break;
            printf("%d. '%s' (%s) - %d Pedidos\n", 
                i+1, 
                ptrs[i]->titulo, 
                obterNomeCategoria(ptrs[i]->categoria),
                ptrs[i]->numRequisicoes);    
    }
}

/**
 * @brief Relatório dos 5 utilizadores com melhor reputação (média de feedback).
 */
void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks) {
    typedef struct { int id; float media; int qtd; } Rep;
    Rep r[MAX_USERS];

    // Calcular médias
    for(int i=0; i<totalUsers; i++) {
        r[i].id = users[i].id;
        r[i].qtd = 0;
        float soma = 0;
        
        for(int k=0; k<totalFeedbacks; k++) {
            if(feedbacks[k].avaliadoId == users[i].id) {
                soma += feedbacks[k].nota;
                r[i].qtd++;
            }
        }
        r[i].media = (r[i].qtd > 0) ? soma / r[i].qtd : 0.0;
    }

    // Sort
    for(int i=0; i<totalUsers-1; i++) {
        for(int j=0; j<totalUsers-i-1; j++) {
            if(r[j].media < r[j+1].media) {
                Rep temp = r[j]; r[j] = r[j+1]; r[j+1] = temp;
            }
        }
    }

    printf("\n--- TOP 5 REPUTACAO (Min. 5 Avaliacoes) ---\n");
    int count = 0;
    for(int i=0; i<totalUsers; i++) {
        if(r[i].qtd >= 5) { // Filtro Mínimo 5
            printf("%d. %s - Classificacao: %.1f (Total: %d)\n", 
                   count+1, users[r[i].id-1].nome, r[i].media, r[i].qtd);
            count++;
        }
        if(count >= 5) break;
    }
    if(count == 0) printf("Nenhum utilizador cumpre o criterio (min 5 avaliacoes).\n");
}

/**
 * @brief Verifica se um utilizador tem histórico de empréstimos ou trocas.
 * @param idUser ID do utilizador a verificar.
 * @param loans Array de empréstimos.
 * @param totalLoans Número total de empréstimos.
 * @return 1 se tiver histórico, 0 caso contrário.
 */
int verificarHistoricoUtilizador(int idUser, Operacao loans[], int totalLoans) {
    for (int i = 0; i < totalLoans; i++) {
        // Verifica se foi quem pediu OU se foi quem emprestou
        if (loans[i].userId == idUser || loans[i].userIdEmprestimo == idUser) {
            return 1; // Tem histórico!
        }
    }
    return 0; // Não tem histórico nenhum
}

/**
 * @brief Valida um ISBN-10 ou ISBN-13.
 * @param isbn String do ISBN (pode conter hífens).
 * @return 1 se válido, 0 se inválido.
 */
int validarISBN(const char *isbn) {
    int len = strlen(isbn);
    int numeros[13]; // Para guardar apenas os dígitos
    int count = 0;

    // 1. Limpar (Remover hífens e validar se são números)
    for (int i = 0; i < len; i++) {
        if (isdigit(isbn[i])) {
            if (count < 13) {
                numeros[count] = isbn[i] - '0';
                count++;
            } else {
                return 0; // Demasiados números
            }
        } 
        // Lógica para aceitar 'X' ou 'x' apenas no ISBN-10 (último dígito)
        else if ((isbn[i] == 'X' || isbn[i] == 'x') && count == 9) {
             numeros[count] = 10; // X vale 10
             count++;
        }
        else if (isbn[i] != '-' && isbn[i] != ' ') {
            return 0; 
        }
    }

    // 2. Validação Matemática
    
    // CASO A: ISBN-13 (Padrão atual, ex: 978-3-16-148410-0)
    if (count == 13) {
        int soma = 0;
        for (int i = 0; i < 12; i++) {
            if (i % 2 == 0) soma += numeros[i] * 1;
            else soma += numeros[i] * 3;
        }
        int check = 10 - (soma % 10);
        if (check == 10) check = 0;

        return (check == numeros[12]);
    }

    // CASO B: ISBN-10 (Antigo, apenas verificamos o tamanho para simplificar)
    if (count == 10) {
        return 1; // Aceitamos ISBN-10 antigo como válido se tiver 10 digitos
    }

    return 0; // Tamanho incorreto (nem 10 nem 13)
}
