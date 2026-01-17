#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // Necessário para isspace
#include <time.h>  // Necessário para calcularIdade

#include "files.h"
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

int validarApenasNumeros(const char *str) {
    while (*str) {
        if (*str < '0' || *str > '9') return 0;
        str++;
    }
    return 1;
}


int validarFormatoEmailIPCA(char *email) {
    if (strcmp(email, "admin@ipca.pt") == 0) return 1;

    char *arroba = strchr(email, '@');
    if (arroba == NULL) return 0;

    int tamanhoUser = arroba - email;
    char *dominio = arroba + 1;

    // --- REGRA ALUNOS ---
    if (strcmp(dominio, "alunos.ipca.pt") == 0) {
        // Tem de começar por 'a' ou 'A'
        if (email[0] != 'a' && email[0] != 'A') return 0;
        // O resto do utilizador tem de ser apenas números
        for (int i = 1; i < tamanhoUser; i++) {
            if (!isdigit((unsigned char)email[i])) return 0;
        }
        return 1;
    }

    // --- REGRA DOCENTES ---
    else if (strcmp(dominio, "ipca.pt") == 0) {
        // Não pode conter números no nome
        for (int i = 0; i < tamanhoUser; i++) {
            if (isdigit((unsigned char)email[i])) return 0;
        }
        return 1;
    }

    return 0; // Outros domínios são rejeitados
}



/**
 * @brief Gera o próximo ID único para um novo livro.
 * Lógica: Percorre todos os livros e encontra o maior ID existente, retornando esse valor + 1.
 */
int gerarProximoId(Livro livros[], int total) {
    int maxId = 0;
    for (int i = 0; i < total; i++) {
        // Verifica todos, mesmo os "retidos" (soft deleted), para evitar duplicar IDs antigos
        if (livros[i].id > maxId) {
            maxId = livros[i].id;
        }
    }
    return maxId + 1;
}

void imprimirLinhaLivro(Livro *book) {
    char donoStr[20];
    
    // Ajustado para ID 1 (Admin/IPCA)
    if (book->idProprietario == 1) {
        strcpy(donoStr, "IPCA (Admin)");
    } else {
        sprintf(donoStr, "User %d", book->idProprietario);
    }

    printf("- ID: %d | ISBN: %-13s | Titulo: %-20.20s | Autor: %-15.15s | Cat: %-10s | Dono: %s\n", 
           book->id,
           book->isbn,
           book->titulo, 
           book->autor, 
           book->categoria,
           donoStr); // Adicionei o dono no print para ser mais informativo
}


float calcularMediaUtilizador(Feedback feedbacks[], int totalFeedbacks, int idUtilizador, int *quantidade) {
    float soma = 0.0f; // IMPORTANTE: Inicializar sempre a zero
    int contador = 0;

    for (int i = 0; i < totalFeedbacks; i++) {
        // Verifica se o feedback é para o utilizador correto
        if (feedbacks[i].idAvaliado == idUtilizador) {
            soma += (float)feedbacks[i].nota;
            contador++;
        }
    }

    *quantidade = contador;

    if (contador > 0) {
        return soma / (float)contador;
    }

    return 0.0f; // Se não houver avaliações, a média é 0
}

void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks) {
    typedef struct { 
        int indexUser; 
        float media; 
        int qtd; 
    } UserStats;

    // Inicializamos tudo a zero para evitar lixo de memória
    UserStats stats[MAX_UTILIZADORES] = {0}; 

    // 1. Calcular médias
    for(int i = 0; i < totalUsers; i++) {
        stats[i].indexUser = i;
        stats[i].media = calcularMediaUtilizador(feedbacks, totalFeedbacks, users[i].id, &stats[i].qtd);
    }

    // 2. Ordenar (Bubble Sort)
    for(int i = 0; i < totalUsers - 1; i++) {
        for(int j = 0; j < totalUsers - i - 1; j++) {
            // Critério Principal: Média
            // Critério Secundário (Desempate): Quantidade de avaliações
            if(stats[j].media < stats[j+1].media || 
              (stats[j].media == stats[j+1].media && stats[j].qtd < stats[j+1].qtd)) {
                UserStats temp = stats[j]; 
                stats[j] = stats[j+1]; 
                stats[j+1] = temp;
            }
        }
    }

    // 3. Apresentar Resultados
    limparEcra();
    printf("\n=== TOP 5 REPUTACAO (Min. 5 Avaliacoes) ===\n");
    printf("%-3s | %-20s | %-10s | %s\n", "N.", "NOME", "MEDIA", "AVALIACOES");
    printf("------------------------------------------------------------\n");

    int contador = 0;
    for(int i = 0; i < totalUsers; i++) {
        int idxReal = stats[i].indexUser;

        // Filtros: Min 5 avaliações, Conta Ativa e IGNORAR o Admin (ID 1)
        if(stats[i].qtd >= 5 && users[idxReal].estado == CONTA_ATIVA && users[idxReal].id != 1) { 
            
            printf("%-3d | %-20.20s | %-10.1f | %d\n", 
                   contador + 1, 
                   users[idxReal].nome, 
                   stats[i].media, 
                   stats[i].qtd);
            
            contador++;
        }
        if(contador >= 5) break;
    }

    if(contador == 0) {
        printf("\n[Info] Nenhum utilizador cumpre os criterios (min. 5 avaliacoes).\n");
    }
    
    printf("------------------------------------------------------------\n");
    esperarEnter();
}

void relatorioTopLivros(Livro livros[], int totalLivros) {
    if (totalLivros == 0) {
        printf("\n[Erro] Nao existem livros registados no sistema.\n");
        esperarEnter();
        return;
    }

    // 1. Criar array de ponteiros para não alterar a ordem original do array principal
    Livro *ptrs[MAX_LIVROS];
    for(int i = 0; i < totalLivros; i++) ptrs[i] = &livros[i];

    // 2. Ordenar por numRequisicoes (Bubble Sort Descrescente)
    for(int i = 0; i < totalLivros - 1; i++) {
        for(int j = 0; j < totalLivros - i - 1; j++) {
            if(ptrs[j]->numRequisicoes < ptrs[j+1]->numRequisicoes) {
                Livro *temp = ptrs[j]; 
                ptrs[j] = ptrs[j+1]; 
                ptrs[j+1] = temp;
            }
        }
    }

    // 3. Apresentar Resultados
    limparEcra();
    printf("\n==================== TOP 5 LIVROS MAIS REQUISITADOS ====================\n");
    printf("%-4s | %-30s | %-15s | %s\n", "POS.", "TITULO", "ISBN", "REQUISICOES");
    printf("------------------------------------------------------------------------\n");

    int mostrados = 0;
    
    for(int i = 0; i < totalLivros && mostrados < 5; i++) {
        // Se chegarmos a livros com 0 requisições, paramos (não faz sentido estarem no top)
        if(ptrs[i]->numRequisicoes == 0) break;

        // Print formatado: %-30.30s corta o título se exceder 30 caracteres
        printf("%-4d | %-30.30s | %-15s | %d\n", 
               mostrados + 1, 
               ptrs[i]->titulo, 
               ptrs[i]->isbn, // Troquei categoria por ISBN pois é mais útil para o Admin identificar
               ptrs[i]->numRequisicoes);    
        
        mostrados++;
    }

    if(mostrados == 0) {
        printf("\n[Info] Ainda nao foram realizadas requisicoes no sistema.\n");
    } else {
        printf("------------------------------------------------------------------------\n");
        printf(" Relatorio gerado com base em %d livros.\n", totalLivros);
    }

    esperarEnter();
}

/**
 * @brief Lista o histórico completo de operações do utilizador logado.
 * @param operacoes Array de operações.
 * @param totalOperacoes Total de operações.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param users Array de utilizadores.
 * @param totalUsers Total de utilizadores.
 * @param idLogado ID do utilizador logado.
 */
void listarHistoricoCompleto(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n=== O MEU HISTORICO DE MOVIMENTOS ===\n");
    // Cabeçalho da tabela
    printf("%-11s | %-12s | %-25s | %-20s | %s\n", "DATA", "TIPO", "LIVRO", "INTERVENIENTE", "DETALHES/PRAZO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int encontrou = 0;
    char dataStr[12];
    char dataFimStr[12];

    // Percorre do mais recente para o mais antigo
    for (int i = totalOperacoes - 1; i >= 0; i--) {
        
        // 1. FILTRO: Sou Requerente (Pedi) OU Sou Proprietário (Emprestei/Troquei)
        if (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado) {
            
            // --- A. DADOS DO LIVRO ---
            char titulo[MAX_STRING] = "Livro Removido";
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) {
                    strcpy(titulo, books[b].titulo);
                    break;
                }
            }

            // --- B. IDENTIFICAR O "OUTRO" E A MINHA AÇÃO ---
            int idOutroUser;
            char acaoDesc[50]; // Ex: "Recebi de...", "Cedi a..."

            if (operacoes[i].idRequerente == idLogado) {
                // Eu PEDI o livro -> O outro é o Proprietário
                idOutroUser = operacoes[i].idProprietario;
                strcpy(acaoDesc, "Recebido de: "); 
            } else {
                // Eu EMPRESTEI o livro -> O outro é o Requerente
                idOutroUser = operacoes[i].idRequerente;
                strcpy(acaoDesc, "Cedido a: ");
            }

            // --- C. NOME DO OUTRO UTILIZADOR ---
            char nomeOutro[MAX_STRING] = "Desconhecido";
            if (idOutroUser == 0 || idOutroUser == 1) {
                strcpy(nomeOutro, "IPCA (Instituicao)");
            } else {
                for (int u = 0; u < totalUsers; u++) {
                    if (users[u].id == idOutroUser) {
                        strcpy(nomeOutro, users[u].nome);
                        break;
                    }
                }
            }

            // Combina a ação com o nome (Ex: "Cedido a: Joao")
            char colunaInterveniente[MAX_STRING];
            sprintf(colunaInterveniente, "%s %s", 
                    (operacoes[i].idRequerente == idLogado) ? "De:" : "Para:", // Simplificação visual
                    nomeOutro);

            // --- D. DATAS E PRAZOS ---
            // Data de Início
            formatarData(operacoes[i].dataEmprestimo, dataStr);

            char prazoDesc[50] = "";
            
            if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
                if (operacoes[i].estado == ESTADO_OP_CONCLUIDO || operacoes[i].estado == ESTADO_OP_CONCLUIDO) { // Ajustar conforme teu enum de DEVOLVIDO
                     strcpy(prazoDesc, "Devolvido");
                } else {
                    // Calcular data prevista
                    int dataFimInt = somarDias(operacoes[i].dataEmprestimo, operacoes[i].dias);
                    formatarData(dataFimInt, dataFimStr);
                    sprintf(prazoDesc, "Ate: %s", dataFimStr);
                }
            } 
            else if (operacoes[i].tipo == OP_TIPO_TROCA) {
                strcpy(prazoDesc, "Permanente");
            }
            else if (operacoes[i].tipo == OP_TIPO_DEVOLUCAO) {
                strcpy(prazoDesc, "Concluido");
            }
            else {
                 // Se estiver PENDENTE
                 if(operacoes[i].estado == ESTADO_OP_PENDENTE) strcpy(prazoDesc, "Pendente...");
                 else if(operacoes[i].estado == ESTADO_OP_REJEITADO) strcpy(prazoDesc, "Recusado");
                 else strcpy(prazoDesc, "---");
            }

            // --- E. TIPO DA OPERAÇÃO ---
            char tipoStr[15];
            switch(operacoes[i].tipo) {
                case OP_TIPO_EMPRESTIMO: strcpy(tipoStr, "EMPRESTIMO"); break;
                case OP_TIPO_TROCA:      strcpy(tipoStr, "TROCA"); break;
                case OP_TIPO_DEVOLUCAO:  strcpy(tipoStr, "DEVOLUCAO"); break;
                default:                 strcpy(tipoStr, "OUTRO");
            }

            // --- IMPRESSÃO ---
            printf("%-11s | %-12s | %-25.25s | %-20.20s | %s\n", 
                   dataStr, 
                   tipoStr, 
                   titulo, 
                   colunaInterveniente, 
                   prazoDesc);

            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\t(Sem historico registado)\n");
    }
    printf("----------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

int validarISBN(const char *isbn) {
    char limpo[20];
    int j = 0;

    // 1. Limpar hífenes ou espaços
    for (int i = 0; isbn[i] != '\0'; i++) {
        if (isdigit(isbn[i]) || isbn[i] == 'X' || isbn[i] == 'x') {
            limpo[j++] = isbn[i];
        }
    }
    limpo[j] = '\0';

    int tam = strlen(limpo);

    // 2. Validação ISBN-10
    if (tam == 10) {
        int soma = 0;
        for (int i = 0; i < 9; i++) {
            soma += (limpo[i] - '0') * (10 - i);
        }
        char ultimo = toupper(limpo[9]);
        if (ultimo == 'X') soma += 10;
        else soma += (ultimo - '0');

        return (soma % 11 == 0);
    }

    // 3. Validação ISBN-13 (O padrão atual)
    if (tam == 13) {
        int soma = 0;
        for (int i = 0; i < 13; i++) {
            int digito = limpo[i] - '0';
            if (i % 2 == 0) soma += digito;     // Peso 1
            else soma += digito * 3;            // Peso 3
        }
        return (soma % 10 == 0);
    }

    return 0; // Se não tiver 10 nem 13 dígitos, é inválido
}

/**
 * @brief Verifica se um ISBN já existe na base de dados de livros.
 * @param livros Array de livros.
 * @param totalLivros Número total de livros.
 * @param isbn String do ISBN a verificar.
 * @return 1 se o ISBN já existir, 0 se for único.
 */
int existeISBN(Livro livros[], int totalLivros, char *isbn) {
    for (int i = 0; i < totalLivros; i++) {
        // Verifica se o livro não está eliminado e se o ISBN bate certo
        if (livros[i].eliminado == 0 && strcmp(livros[i].isbn, isbn) == 0) {
            return 1; // Encontrou duplicado
        }
    }
    return 0; // É único
}

/**
 * @brief Converte uma string para minúsculas.
 * @param origem String original.
 * @param destino Buffer onde a string minúscula será armazenada.
 */
void paraMinusculas(const char *origem, char *destino) {
    int i = 0;
    while (origem[i] != '\0') {
        destino[i] = tolower((unsigned char)origem[i]);
        i++;
    }
    destino[i] = '\0'; // Fechar a string
}

/**
 * @brief Obtém a data atual no formato YYYYMMDD.
 * @return Data atual como inteiro.
 */
int obterDataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    int ano = tm.tm_year + 1900;
    int mes = tm.tm_mon + 1;
    int dia = tm.tm_mday;

    return (ano * 10000) + (mes * 100) + dia;
}

/**
 * @brief Formata uma data do formato YYYYMMDD para DD/MM/YYYY.
 * @param dataInt Data no formato inteiro YYYYMMDD.
 * @param buffer Buffer onde a data formatada será armazenada (deve ter pelo menos 11 bytes).
 */
void formatarData(int dataInt, char *buffer) {
    if (dataInt == 0) {
        strcpy(buffer, "--/--/----");
        return;
    }
    
    int ano = dataInt / 10000;
    int mes = (dataInt % 10000) / 100;
    int dia = dataInt % 100;
    
    sprintf(buffer, "%02d/%02d/%04d", dia, mes, ano);
}

/**
 * @brief Soma um número de dias a uma data no formato YYYYMMDD.
 * @param dataInicio Data inicial no formato inteiro YYYYMMDD.
 * @param dias Número de dias a somar.
 * @return Nova data no formato inteiro YYYYMMDD.
 */
int somarDias(int dataInicio, int dias) {
    if (dataInicio == 0) return 0;

    struct tm tm = {0};
    
    // Decompor o inteiro YYYYMMDD
    tm.tm_year = (dataInicio / 10000) - 1900;
    tm.tm_mon  = ((dataInicio % 10000) / 100) - 1;
    tm.tm_mday = dataInicio % 100;
    
    // Adicionar os dias
    tm.tm_mday += dias;
    
    // Normalizar (o mktime corrige 32 de Janeiro para 1 de Fevereiro, etc.)
    mktime(&tm);
    
    // Reconstruir o inteiro YYYYMMDD
    int ano = tm.tm_year + 1900;
    int mes = tm.tm_mon + 1;
    int dia = tm.tm_mday;
    
    return (ano * 10000) + (mes * 100) + dia;
}

void adminValidarUtilizadores(Utilizador users[], int totalUsers) {
    int escolha;
    do {
        limparEcra();
        printf("\n--- VALIDAR PEDIDOS DE ACESSO ---\n");
        printf("%-3s | %-8s | %-30s | %s\n", "N.", "TIPO", "EMAIL", "NOME");
        printf("--------------------------------------------------------------------------------\n");

        int pendentes = 0;
        int visualId = 1;

        for(int i = 0; i < totalUsers; i++) {
            // Utilizando os nomes exatos da tua struct/enum
            if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
                
                char tipo[10];
                if (strstr(users[i].email, "alunos.ipca.pt") != NULL) strcpy(tipo, "Aluno");
                else strcpy(tipo, "Docente");

                printf("%-3d | %-8s | %-30.30s | %s\n", 
                       visualId, tipo, users[i].email, users[i].nome);
                       
                visualId++;
                pendentes++;
            }
        }
        printf("--------------------------------------------------------------------------------\n");

        if (pendentes == 0) {
            printf("[Info] Nao existem utilizadores pendentes.\n");
            esperarEnter(); 
            return;        
        }

        printf("\nSelecione o N. para validar (0 para Voltar): ");
        escolha = lerInteiro("", 0, visualId - 1);

        if (escolha != 0) {
            int contador = 1;
            int idx = -1;
            for(int i = 0; i < totalUsers; i++) {
                if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
                    if(contador == escolha) { idx = i; break; }
                    contador++;
                }
            }

            if(idx != -1) {
                printf("\n--- ACAO: %s ---\n", 
                       (users[idx].estado == CONTA_PENDENTE_REATIVACAO) ? "REATIVAR CONTA" : "NOVO REGISTO");
                printf("Utilizador: %s\n", users[idx].nome);
                printf("\n1. Aprovar\n2. Rejeitar\n0. Cancelar\n");
                
                int dec = lerInteiro("Decisao: ", 0, 2);

                if (dec == 1) {
                    users[idx].estado = CONTA_ATIVA; // Nome da tua struct
                    registarLog(0, "ADMIN_APROVA", users[idx].email);
                    guardarUtilizadores(users, totalUsers); 
                    // Removido o print e o esperarEnter para fluidez
                } else if (dec == 2) {
                    users[idx].estado = CONTA_INATIVA; // Nome da tua struct
                    registarLog(0, "ADMIN_REJEITA", users[idx].email);
                    guardarUtilizadores(users, totalUsers);
                    // Removido o print e o esperarEnter para fluidez
                }
            }
        }
    } while (escolha != 0);
}



void relatorioTopUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes) {
    limparEcra();
    printf("\n=== TOP 5 UTILIZADORES MAIS ATIVOS ===\n");
    printf("(Criterio: Minimo de 5 transacoes concluidas)\n\n");

    typedef struct {
        int indexUser;
        int qtd;
    } UserStats;

    UserStats ranking[MAX_UTILIZADORES]; // Usar a constante para maior segurança

    // 1. Inicializar e Contar
    for (int i = 0; i < totalUsers; i++) {
        ranking[i].indexUser = i;
        ranking[i].qtd = 0;

        // Ignorar o Admin pelo ID (mais seguro que string)
        if (users[i].id == 1) {
            ranking[i].qtd = -1; 
            continue;
        }

        for (int k = 0; k < totalOperacoes; k++) {
            // Contamos apenas transações que chegaram a um estado ativo ou final
            if (operacoes[k].estado == ESTADO_OP_ACEITE || 
                operacoes[k].estado == ESTADO_OP_CONCLUIDO || 
                operacoes[k].estado == ESTADO_OP_DEVOLUCAO_PENDENTE) {
                
                // Se o utilizador é o Requerente OU o Proprietário
                if (operacoes[k].idRequerente == users[i].id || 
                    operacoes[k].idProprietario == users[i].id) {
                    ranking[i].qtd++;
                }
            }
        }
    }

    // 2. Ordenar (Bubble Sort)
    for (int i = 0; i < totalUsers - 1; i++) {
        for (int j = 0; j < totalUsers - i - 1; j++) {
            // Critério 1: Quantidade | Critério 2: Ordem Alfabética (Desempate)
            if (ranking[j].qtd < ranking[j+1].qtd) {
                UserStats temp = ranking[j];
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }

    // 3. Apresentar Resultados
    printf("%-4s | %-25s | %-12s | %s\n", "POS.", "NOME", "TIPO", "MOVIMENTACOES");
    printf("------------------------------------------------------------------\n");

    int mostrados = 0;
    for (int i = 0; i < totalUsers && mostrados < 5; i++) {
        int idxReal = ranking[i].indexUser;
        
        // Mantemos o teu critério de 5 transações
        if (ranking[i].qtd < 5) break;

        char tipo[15];
        if (strstr(users[idxReal].email, "@alunos.ipca.pt")) strcpy(tipo, "Aluno");
        else strcpy(tipo, "Docente");

        printf("%-4d | %-25.25s | %-12s | %d\n", 
               mostrados + 1, 
               users[idxReal].nome, 
               tipo, 
               ranking[i].qtd);

        mostrados++;
    }

    if (mostrados == 0) {
        printf("\n[Info] Nenhum utilizador cumpre o requisito de 5 transacoes.\n");
    }

    printf("------------------------------------------------------------------\n");
    esperarEnter();
}

void registarLog(int idUser, char *acao, char *detalhes) {
    FILE *f = fopen("data/log_sistema.txt", "a");
    if (f == NULL) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Formato scannable: Data | Hora | UserID | Acao | Detalhes
    fprintf(f, "[%04d-%02d-%02d | %02d:%02d] [ID:%d] [%-15s] %s\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, idUser, acao, detalhes);

    fclose(f);
}

int validarData(char *data) {
    // 1. Verificar tamanho (DD/MM/AAAA = 10 caracteres)
    if (strlen(data) != 10) return 0;

    // 2. Verificar se as barras estão nos locais certos
    if (data[2] != '/' || data[5] != '/') return 0;

    // 3. Extrair valores (convertendo chars para inteiros)
    int dia = atoi(data);
    int mes = atoi(data + 3);
    int ano = atoi(data + 6);

    // 4. Validações lógicas básicas
    if (ano < 1920 || ano > 2026) return 0; // 2026 é o ano atual
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;

    // 5. Validação de dias por mês (simplificada)
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) return 0;
    if (mes == 2) {
        // Fevereiro (simplificado para 28 ou 29 se bissexto)
        int bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
        if (dia > (bissexto ? 29 : 28)) return 0;
    }

    return 1; // Data válida!
}

#include <time.h>

int adicionarDias(int dataAtual, int diasParaAdicionar) {
    struct tm data = {0};
    
    // Decompõe YYYYMMDD
    data.tm_year = (dataAtual / 10000) - 1900;
    data.tm_mon = ((dataAtual % 10000) / 100) - 1;
    data.tm_mday = (dataAtual % 100);

    // Adiciona os dias
    data.tm_mday += diasParaAdicionar;

    // Normaliza a data (o C trata de virar o mês/ano se necessário)
    mktime(&data);

    // Retorna no formato YYYYMMDD
    return (data.tm_year + 1900) * 10000 + (data.tm_mon + 1) * 100 + data.tm_mday;
}

float lerFloat(char* mensagem, float min, float max) {
    float valor;
    int res;
    do {
        printf("%s", mensagem);
        res = scanf("%f", &valor);
        limparBuffer(); // Função que já deves ter para limpar o \n

        if (res != 1 || valor < min || valor > max) {
            printf("[Erro] Por favor, introduza um valor entre %.1f e %.1f.\n", min, max);
        }
    } while (res != 1 || valor < min || valor > max);
    
    return valor;
}