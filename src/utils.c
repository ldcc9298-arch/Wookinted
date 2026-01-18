#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <time.h>  

#include "files.h"
#include "utils.h"
#include "structs.h"
#include "books.h"

/* =============================================================
   GESTÃO DE ENTRADA (INPUT) E TERMINAL
   ============================================================= */

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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

int lerInteiro(char *mensagem, int min, int max) {
    int valor;
    do {
        printf("%s", mensagem);
        if (scanf("%d", &valor) != 1) {
            limparBuffer(); 
            printf("[Erro] Insira um numero valido.\n");
            continue;
        }
        limparBuffer(); 
        if (valor < min || valor > max) {
            printf("[Erro] Valor deve estar entre %d e %d.\n", min, max);
        }
    } while (valor < min || valor > max);
    return valor;
}

float lerFloat(char* mensagem, float min, float max) {
    float valor;
    int res;
    do {
        printf("%s", mensagem);
        res = scanf("%f", &valor);
        limparBuffer(); 
        if (res != 1 || valor < min || valor > max) {
            printf("[Erro] Por favor, introduza um valor entre %.1f e %.1f.\n", min, max);
        }
    } while (res != 1 || valor < min || valor > max);
    return valor;
}

void limparEcra() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void esperarEnter() {
    printf("\n[Pressione ENTER para continuar...]");
    getchar();      
}

/* =============================================================
   VALIDAÇÕES DE DADOS (SANREAMENTO)
   ============================================================= */

int stringNaoVazia(char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) return 1;
    }
    return 0; 
}

int validarApenasLetras(char *str, int tamanho) {
    (void)tamanho; 
    if (str == NULL || strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit((unsigned char)str[i])) return 0;
        if (ispunct(str[i]) && str[i] != '-') return 0;
    }
    return 1;
}

int validarApenasNumeros(const char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int validarISBN(const char *isbn) {
    char limpo[20];
    int j = 0;
    for (int i = 0; isbn[i] != '\0'; i++) {
        if (isdigit(isbn[i]) || isbn[i] == 'X' || isbn[i] == 'x') {
            limpo[j++] = isbn[i];
        }
    }
    limpo[j] = '\0';
    int tam = strlen(limpo);

    if (tam == 10) {
        int soma = 0;
        for (int i = 0; i < 9; i++) soma += (limpo[i] - '0') * (10 - i);
        char ultimo = toupper(limpo[9]);
        if (ultimo == 'X') soma += 10;
        else soma += (ultimo - '0');
        return (soma % 11 == 0);
    }
    if (tam == 13) {
        int soma = 0;
        for (int i = 0; i < 13; i++) {
            int digito = limpo[i] - '0';
            soma += (i % 2 == 0) ? digito : digito * 3;
        }
        return (soma % 10 == 0);
    }
    return 0; 
}

int validarTelemovelPortugues(char *tel) {
    if (strlen(tel) != 9) return 0;
    if (!validarApenasNumeros(tel)) return 0;
    if (strncmp(tel, "91", 2) == 0 || strncmp(tel, "92", 2) == 0 ||
        strncmp(tel, "93", 2) == 0 || strncmp(tel, "96", 2) == 0) {
        return 1;
    }
    return 0;
}

int validarAlfanumerico(char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum((unsigned char)str[i]) && !isspace((unsigned char)str[i])) return 0;
    }
    return 1;
}

/* =============================================================
   MANIPULAÇÃO DE DATAS E TEMPO (time.h)
   ============================================================= */

int calcularIdade(char *dataNasc) {
    int dia, mes, ano;
    if (sscanf(dataNasc, "%d/%d/%d", &dia, &mes, &ano) != 3) return -1;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int anoAt = tm.tm_year + 1900, mesAt = tm.tm_mon + 1, diaAt = tm.tm_mday;
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano > anoAt) return -1;
    int idade = anoAt - ano;
    if (mesAt < mes || (mesAt == mes && diaAt < dia)) idade--;
    return idade;
}

int obterDataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return ((tm.tm_year + 1900) * 10000) + ((tm.tm_mon + 1) * 100) + tm.tm_mday;
}

void formatarData(int dataInt, char *buffer) {
    if (dataInt == 0) { strcpy(buffer, "--/--/----"); return; }
    sprintf(buffer, "%02d/%02d/%04d", dataInt % 100, (dataInt % 10000) / 100, dataInt / 10000);
}

int somarDias(int dataInicio, int dias) {
    if (dataInicio == 0) return 0;
    struct tm tm = {0};
    tm.tm_year = (dataInicio / 10000) - 1900;
    tm.tm_mon  = ((dataInicio % 10000) / 100) - 1;
    tm.tm_mday = (dataInicio % 100) + dias;
    mktime(&tm);
    return ((tm.tm_year + 1900) * 10000) + ((tm.tm_mon + 1) * 100) + tm.tm_mday;
}

int validarData(char *data) {
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;
    int d = atoi(data), m = atoi(data + 3), a = atoi(data + 6);
    if (a < 1920 || a > 2026 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) return 0;
    if (m == 2) {
        int biss = (a % 4 == 0 && (a % 100 != 0 || a % 400 == 0));
        if (d > (biss ? 29 : 28)) return 0;
    }
    return 1;
}

/* =============================================================
   OUTROS UTILITÁRIOS E AUDITORIA
   ============================================================= */

void registarLog(int idUser, char *acao, char *detalhes) {
    FILE *f = fopen("data/log_sistema.txt", "a");
    if (!f) return;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d] [ID:%3d] [%-20s] %s\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, idUser, acao, detalhes);
    fclose(f);
}

int existeISBN(Livro livros[], int totalLivros, char *isbn) {
    for (int i = 0; i < totalLivros; i++) {
        if (livros[i].eliminado == 0 && strcmp(livros[i].isbn, isbn) == 0) return 1;
    }
    return 0; 
}

void paraMinusculas(const char *origem, char *destino) {
    int i = 0;
    while (origem[i] != '\0') {
        destino[i] = tolower((unsigned char)origem[i]);
        i++;
    }
    destino[i] = '\0';
}