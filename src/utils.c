#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // Necessário para isspace
#include <time.h>  // Necessário para calcularIdade

#include "utils.h"
#include "structs.h"
#include "books.h"

// =============================================================
// CATEGORIAS DE LIVROS
// =============================================================

const char* obterNomeCategoria(CategoriaLivro cat, char* manualDesc) {
    switch (cat) {
        case CAT_FICCAO: return "Ficcao";
        case CAT_NAO_FICCAO: return "Nao Ficcao";
        case CAT_CIENCIA: return "Ciencia";
        case CAT_HISTORIA: return "Historia";
        case CAT_BIOGRAFIA: return "Biografia";
        case CAT_TECNOLOGIA: return "Tecnologia";
        case CAT_OUTRO: return manualDesc; // Retorna o texto manual se for Outro
        default: return "Desconhecido";
    }
}

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

/**
 * @brief Imprime uma linha formatada de um livro.
 * Usado para evitar repetição de código nas pesquisas.
 */
void imprimirLinhaLivro(Livro *book) {
    char donoStr[20];
    if (book->idProprietario == 0) strcpy(donoStr, "INSTITUTO");
    else sprintf(donoStr, "User %d", book->idProprietario);

    printf("- ID: %d | ISBN: %-13s | Titulo: %-20.20s | Autor: %-15.15s | Cat: %s\n", 
           book->id,
           book->isbn,
           book->titulo, 
           book->autor, 
           obterNomeCategoria(book->categoria, book->categoriaManual));
}

/**
 * @brief Calcula a média de avaliações (notas) recebidas por um utilizador.
 * @param feedbacks Array de feedbacks.
 * @param totalFeedbacks Número total de feedbacks no array.
 * @param idLogado ID do utilizador alvo.
 * @param qtdReviews Ponteiro para armazenar a quantidade de reviews (pode ser NULL).
 * @return Média das notas (float). Retorna 0.0 se não houver avaliações.
 */
float calcularMediaUtilizador(Feedback feedbacks[], int totalFeedbacks, int idLogado, int *qtdReviews) {
    int somaNotas = 0;
    int contador = 0;

    for (int i = 0; i < totalFeedbacks; i++) {
        // Verifica se o feedback é dirigido ao utilizador alvo
        if (feedbacks[i].idAvaliado == idLogado) {
            somaNotas += feedbacks[i].nota;
            contador++;
        }
    }

    // "Devolvemos" o contador para fora através do ponteiro
    if (qtdReviews != NULL) {
        *qtdReviews = contador;
    }

    // Evitar divisão por zero
    if (contador == 0) return 0.0;

    return (float)somaNotas / contador;
}

/**
 * @brief Relatório dos 5 utilizadores com melhor reputação (média de feedback).
 */
void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks) {
    
// Estrutura temporária para ordenação
    typedef struct { 
        int indexUser; // Guardamos a posição no array 'users' para saber o nome depois
        float media; 
        int qtd; 
    } UserStats;

    UserStats stats[MAX_UTILIZADORES]; // Ou usar totalUsers se o compilador permitir VLA

    // 1. Calcular médias usando a função DRY (Don't Repeat Yourself)
    for(int i = 0; i < totalUsers; i++) {
        stats[i].indexUser = i; // Guardamos onde este user está no array original
        
        // A função faz o trabalho sujo de somar e contar
        stats[i].media = calcularMediaUtilizador(feedbacks, totalFeedbacks, users[i].id, &stats[i].qtd);
    }

    // 2. Ordenar (Bubble Sort - Do maior para o menor)
    for(int i = 0; i < totalUsers - 1; i++) {
        for(int j = 0; j < totalUsers - i - 1; j++) {
            if(stats[j].media < stats[j+1].media) {
                UserStats temp = stats[j]; 
                stats[j] = stats[j+1]; 
                stats[j+1] = temp;
            }
        }
    }

    // 3. Apresentar Resultados
    limparEcra();
    printf("\n--- TOP 5 REPUTACAO (Min. 5 Avaliacoes) ---\n");
    printf("%-3s | %-20s | %-10s | %s\n", "N.", "NOME", "MEDIA", "QTD");
    printf("----------------------------------------------------\n");

    int contador = 0;
    for(int i = 0; i < totalUsers; i++) {
        // Filtro: Mínimo 5 avaliações E conta ativa (opcional)
        // Usamos stats[i].indexUser para ir buscar o nome correto com segurança
        int idxReal = stats[i].indexUser;

        if(stats[i].qtd >= 5 && users[idxReal].estado == CONTA_ATIVA) { 
            
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
        printf("\n[Info] Nenhum utilizador cumpre os criterios (min 5 avaliacoes).\n");
    }
    
    printf("----------------------------------------------------\n");
    esperarEnter();
}

void relatorioTopLivros(Livro livros[], int totalLivros) {
    // 1. Array de ponteiros para ordenação (Mantido da tua lógica original)
    Livro *ptrs[MAX_LIVROS];
    for(int i=0; i<totalLivros; i++) ptrs[i] = &livros[i];

    // 2. Ordenar por numRequisicoes (Bubble Sort Descrescente)
    for(int i=0; i<totalLivros-1; i++) {
        for(int j=0; j<totalLivros-i-1; j++) {
            if(ptrs[j]->numRequisicoes < ptrs[j+1]->numRequisicoes) {
                Livro *temp = ptrs[j]; 
                ptrs[j] = ptrs[j+1]; 
                ptrs[j+1] = temp;
            }
        }
    }

    // 3. Apresentar Resultados (Formatação Tabela)
    limparEcra();
    printf("\n--- TOP 5 LIVROS MAIS REQUISITADOS ---\n");
    // Ajustei as larguras: Título (25), Categoria (15)
    printf("%-4s | %-25s | %-15s | %s\n", "POS.", "TITULO", "CATEGORIA", "TOTAL");
    printf("------------------------------------------------------------------\n");

    int mostrados = 0;
    
    for(int i=0; i<totalLivros; i++) {
        // CRITÉRIOS DE PARAGEM:
        // 1. Já mostramos 5?
        // 2. O livro tem 0 requisições? (Como está ordenado, os seguintes também terão 0)
        if(mostrados >= 5) break;
        if(ptrs[i]->numRequisicoes == 0) break;

        // Obter nome da categoria (assumindo que tens esta função auxiliar)
        const char *nomeCat = obterNomeCategoria(ptrs[i]->categoria, ptrs[i]->categoriaManual);
        // Imprimir Linha Formatada
        // %.25s garante que se o título for gigante, corta para não estragar a tabela
        printf("%-4d | %-25.25s | %-15.15s | %d\n", 
               mostrados + 1, 
               ptrs[i]->titulo, 
               nomeCat,
               ptrs[i]->numRequisicoes);    
        
        mostrados++;
    }

    // Mensagem se a lista estiver vazia
    if(mostrados == 0) {
        printf("\n[Info] Ainda nao existem registos de requisicoes de livros.\n");
    }

    printf("------------------------------------------------------------------\n");
    
    // Mantém o enter aqui (e remove do switch no menu, tal como nos outros)
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

/**
 * @brief Valida um ISBN-10 ou ISBN-13.
 * @param isbn String do ISBN (pode conter hífens).
 * @return 1 se válido, 0 se inválido.
 */
int validarISBN(const char *isbn) {
    int len = strlen(isbn);
    int numeros[13]; // Para guardar apenas os dígitos
    int contador = 0;

    // 1. Limpar (Remover hífens e validar se são números)
    for (int i = 0; i < len; i++) {
        if (isdigit(isbn[i])) {
            if (contador < 13) {
                numeros[contador] = isbn[i] - '0';
                contador++;
            } else {
                return 0; // Demasiados números
            }
        } 
        // Lógica para aceitar 'X' ou 'x' apenas no ISBN-10 (último dígito)
        else if ((isbn[i] == 'X' || isbn[i] == 'x') && contador == 9) {
             numeros[contador] = 10; // X vale 10
             contador++;
        }
        // --- AQUI ESTAVA O PROBLEMA ---
        // Agora aceitamos também '\n' e '\r' (Enter) como caracteres a ignorar
        else if (isbn[i] != '-' && isbn[i] != ' ' && isbn[i] != '\n' && isbn[i] != '\r') {
            return 0; // Caracter inválido encontrado
        }
    }

    // 2. Validação Matemática
    
    // CASO A: ISBN-13 (Padrão atual)
    if (contador == 13) {
        int soma = 0;
        for (int i = 0; i < 12; i++) {
            if (i % 2 == 0) soma += numeros[i] * 1;
            else soma += numeros[i] * 3;
        }
        int check = 10 - (soma % 10);
        if (check == 10) check = 0;

        return (check == numeros[12]);
    }

    // CASO B: ISBN-10 (Antigo)
    if (contador == 10) {
        return 1; 
    }

    return 0; // Tamanho incorreto
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
 * @brief Obtém o nome da categoria visual de um livro, considerando categorias manuais.
 * @param b Apontador para o livro.
 * @return Nome da categoria visual (pode ser a manual se for "Outro").
 */
const char* obterNomeVisualCategoria(Livro *b) {
    // 1. Se não for "Outro" (vamos assumir que o ID de Outro é, por exemplo, 5 ou 9 - ajusta ao teu enum)
    // Nota: Estou a assumir que tens uma constante ou #define para OUTRO. Se não, usa o número correspondente.
    if (b->categoria != 7) {
        return obterNomeCategoria(b->categoria, b->categoriaManual); // Retorna a categoria normal
    }
    
    // 2. Se for "Outro", verificamos se tem uma categoria manual definida
    // (AQUI: Podes adicionar && b->status == APROVADO se tiveres esse campo)
    if (strlen(b->categoriaManual) > 0) {
        return b->categoriaManual; // Retorna "Fantasia", "Culinária", etc.
    }

    // 3. Se for Outro e não tiver nada manual, retorna "Outro"
    return "Outro";
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
        printf("\n--- VALIDAR UTILIZADORES PENDENTES ---\n");
        // Alteração 1: Cabeçalho com as colunas pedidas
        printf("%-3s | %-8s | %-30s | %s\n", "N.", "TIPO", "EMAIL", "NOME");
        printf("--------------------------------------------------------------------------------\n");

        int pendentes = 0;
        int visualId = 1;

        // 1. Listagem e Contagem
        for(int i = 0; i < totalUsers; i++) {
            if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
                
                // Alteração 2: Determinar o Tipo baseado no domínio do email
                char tipo[10];
                if (strstr(users[i].email, "alunos.ipca.pt") != NULL) {
                    strcpy(tipo, "Aluno");
                } else {
                    strcpy(tipo, "Docente"); // Assume Docente/Staff se for ipca.pt
                }

                // Alteração 3: Print formatado (N. Visual, Tipo, Email, Nome)
                printf("%-3d | %-8s | %-30.30s | %s\n", 
                       visualId, 
                       tipo, 
                       users[i].email, 
                       users[i].nome);
                       
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

        // 2. Seleção
        printf("\nSelecione o N. para validar (0 para Voltar): ");
        escolha = lerInteiro("", 0, visualId - 1);

        if (escolha != 0) {
            // Lógica de mapear o número visual para o índice real do array
            int contador = 1;
            int idx = -1;
            for(int i = 0; i < totalUsers; i++) {
                if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
                    if(contador == escolha) {
                        idx = i; break;
                    }
                    contador++;
                }
            }

            if(idx != -1) {
                // Aqui mostramos os detalhes antes de confirmar
                printf("\n--- Validar %s ---\n", users[idx].nome);
                printf("Tipo: %s\n", (strstr(users[idx].email, "alunos") ? "Aluno" : "Docente"));
                printf("Email: %s\n", users[idx].email);
                printf("1. Aprovar\n2. Rejeitar\n0. Cancelar\n");
                
                int dec = lerInteiro("Decisao: ", 0, 2);

                if (dec == 1) {
                    users[idx].estado = CONTA_ATIVA;
                    printf("[Sucesso] Conta ativada.\n");
                } else if (dec == 2) {
                    users[idx].estado = CONTA_INATIVA; // Ou podes ter um estado REJEITADO
                    printf("[Sucesso] Registo rejeitado.\n");
                }
                if (dec != 0) esperarEnter();
            }
        }

    } while (escolha != 0);
}

// Agora recebe também 'users' e 'totalUsers'
void adminValidarLivros(Utilizador users[], int totalUsers, Livro livros[], int totalLivros) {
    int escolha;
    do {
        limparEcra();
        printf("\n--- VALIDAR LIVROS (CATEGORIA 'OUTRO') ---\n");
        // Ajustei as larguras para caber a nova coluna
        printf("%-3s | %-25s | %-15s | %s\n", "N.", "TITULO", "CAT. PROPOSTA", "ENVIADO POR");
        printf("--------------------------------------------------------------------------------\n");

        int pendentes = 0;
        int visualId = 1;

        for(int i = 0; i < totalLivros; i++) {
            if (livros[i].estado == LIVRO_PENDENTE_CAT && livros[i].eliminado == 0) {
                
                // --- NOVO: Procurar o nome do dono do livro ---
                char nomeDono[50] = "Desconhecido";
                for(int u = 0; u < totalUsers; u++) {
                    if(users[u].id == livros[i].idProprietario) {
                        // Podes usar users[u].email se preferires algo mais curto/formal
                        strncpy(nomeDono, users[u].nome, 19); 
                        nomeDono[19] = '\0'; // Garante que a string termina
                        break;
                    }
                }
                // ----------------------------------------------

                printf("%-3d | %-25.25s | %-15.15s | %s\n", 
                       visualId, 
                       livros[i].titulo, 
                       livros[i].categoriaManual, 
                       nomeDono); // Mostra quem enviou
                       
                visualId++;
                pendentes++;
            }
        }
        printf("--------------------------------------------------------------------------------\n");

        if (pendentes == 0) {
            printf("[Info] Sem livros pendentes de validacao.\n");
            esperarEnter();
            return;
        }

        printf("\nSelecione o N. para validar (0 para Voltar): ");
        escolha = lerInteiro("", 0, visualId - 1);

        if (escolha != 0) {
            int contador = 1;
            int idx = -1;
            for(int i = 0; i < totalLivros; i++) {
                if (livros[i].estado == LIVRO_PENDENTE_CAT && livros[i].eliminado == 0) {
                    if(contador == escolha) { idx = i; break; }
                    contador++;
                }
            }

            if(idx != -1) {
                // Mostrar mais detalhes na confirmação
                printf("\n--- Detalhes da Validacao ---\n");
                printf("Titulo: %s\n", livros[idx].titulo);
                printf("Autor: %s\n", livros[idx].autor);
                printf("Categoria Proposta: %s\n", livros[idx].categoriaManual);
                
                // Opção de editar a categoria seria interessante aqui, mas vamos manter simples
                printf("\n1. Aprovar (Aceitar Categoria)\n2. Rejeitar (Remover Livro)\n0. Cancelar\n");
                int dec = lerInteiro("Decisao: ", 0, 2);

                if (dec == 1) {
                    livros[idx].estado = LIVRO_DISPONIVEL;
                    // Nota: Futuramente podias copiar a categoriaManual para um enum novo,
                    // mas por agora fica apenas aprovado.
                    printf("[Sucesso] Livro Aprovado e Disponivel no Mercado.\n");
                } else if (dec == 2) {
                    livros[idx].eliminado = 1;
                    livros[idx].estado = LIVRO_INDISPONIVEL;
                    printf("[Sucesso] Livro Removido do sistema.\n");
                }
                if (dec != 0) esperarEnter();
            }
        }
    } while (escolha != 0);
}

int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes) {
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].idRequerente == idUser || operacoes[i].idProprietario == idUser) {
            return 1; // Tem histórico
        }
    }
    return 0; // Limpo
}

void relatorioTopUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes) {
    limparEcra();
    printf("\n=== TOP 5 UTILIZADORES MAIS ATIVOS ===\n");
    printf("(Criterio: Minimo de 5 transacoes realizadas)\n\n");

    // 1. Estrutura temporária para guardar contagens
    // Usamos isto para poder ordenar sem estragar o array 'users' original
    typedef struct {
        int indexUser; // Para saber quem é o user no array original
        int qtd;       // Quantidade de transações
    } UserStats;

    UserStats ranking[totalUsers]; // Array temporário (VLA)

    // 2. Contar Transações para cada Utilizador
    for (int i = 0; i < totalUsers; i++) {
        ranking[i].indexUser = i;
        ranking[i].qtd = 0;

        // Ignorar o Admin/IPCA (geralmente não entra no top)
        if (strcmp(users[i].email, "admin@ipca.pt") == 0) {
            ranking[i].qtd = -1; // Forçamos a ficar no fundo
            continue;
        }

        // Percorrer todas as operações
        for (int k = 0; k < totalOperacoes; k++) {
            // Conta se o user for o Requerente OU o Proprietário (envolvido na troca)
            // E ignoramos operações pendentes/recusadas (apenas transações reais contam?)
            // Se quiseres contar TUDO (mesmo pendentes), remove a verificação do estado.
            if (operacoes[k].estado != ESTADO_OP_PENDENTE && operacoes[k].estado != ESTADO_OP_REJEITADO) {
                
                if (operacoes[k].idRequerente == users[i].id || 
                    operacoes[k].idProprietario == users[i].id) {
                    ranking[i].qtd++;
                }
            }
        }
    }

    // 3. Ordenar (Bubble Sort Simples) - Do Maior para o Menor
    for (int i = 0; i < totalUsers - 1; i++) {
        for (int j = 0; j < totalUsers - i - 1; j++) {
            if (ranking[j].qtd < ranking[j+1].qtd) {
                // Troca de posição
                UserStats temp = ranking[j];
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }

    // 4. Apresentar Resultados
    printf("%-4s | %-25s | %-15s | %s\n", "POS.", "NOME", "TIPO", "TOTAL MOV.");
    printf("------------------------------------------------------------------\n");

    int mostrados = 0;
    int encontrouAlguem = 0;

    for (int i = 0; i < totalUsers; i++) {
        int qtd = ranking[i].qtd;
        int idxReal = ranking[i].indexUser;

        // CRITÉRIO DE PARAGEM:
        // 1. Já mostramos 5 pessoas? Pára.
        // 2. A quantidade é menor que 5? Pára (porque está ordenado, os seguintes serão menores).
        if (mostrados >= 5) break;
        if (qtd < 5) break; 

        // Definir Tipo (Aluno/Docente)
        char tipo[20];
        if (strstr(users[idxReal].email, "@alunos.ipca.pt")) strcpy(tipo, "Aluno");
        else strcpy(tipo, "Docente");

        printf("%-4d | %-25.25s | %-15s | %d\n", 
               mostrados + 1, 
               users[idxReal].nome, 
               tipo, 
               qtd);

        mostrados++;
        encontrouAlguem = 1;
    }

    if (!encontrouAlguem) {
        printf("\n[Info] Nenhum utilizador atingiu ainda o minimo de 5 transacoes.\n");
    }

    printf("------------------------------------------------------------------\n");
    esperarEnter();
}

