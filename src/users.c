#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>

#include "utils.h"
#include "interface.h"
#include "files.h"
#include "transactions.h"
#include "structs.h"

/* =============================================================
   SEGURANÇA E VALIDAÇÃO DE ACESSO
   ============================================================= */

int validarFormatoEmailIPCA(char *email) {
    // Exceção direta para permitir o login do administrador padrão
    if (strcmp(email, "admin@ipca.pt") == 0) return 1;

    // Localiza o caractere separador @ na string
    char *arroba = strchr(email, '@');
    if (arroba == NULL) return 0; // Se não existir @, o email é inválido

    // Calcula a distância entre o início da string e o @ para obter o tamanho do nome de utilizador
    int tamanhoUser = arroba - email;
    // Define o ponteiro para o início da string de domínio (caractere após o @)
    char *dominio = arroba + 1;

    // Verificação para o domínio de Alunos
    if (strcmp(dominio, "alunos.ipca.pt") == 0) {
        // O email de aluno deve obrigatoriamente começar com 'a' ou 'A'
        if (email[0] != 'a' && email[0] != 'A') return 0;
        // Percorre o restante do nome do utilizador para garantir que são apenas dígitos
        for (int i = 1; i < tamanhoUser; i++) {
            if (!isdigit((unsigned char)email[i])) return 0;
        }
        return 1;
    }
    // Verificação para o domínio de Docentes/Staff
    else if (strcmp(dominio, "ipca.pt") == 0) {
        // Nomes de docentes não podem conter números no identificador
        for (int i = 0; i < tamanhoUser; i++) {
            if (isdigit((unsigned char)email[i])) return 0;
        }
        return 1;
    }

    return 0; // Rejeita qualquer outro domínio não institucional
}

int autenticarUtilizador(Utilizador users[], int totalUsers, char *email, char *pass) {
    int indice = -1;

    // Percorre o array à procura de um email que coincida com o introduzido
    for (int i = 0; i < totalUsers; i++) {
        if (strcmp(users[i].email, email) == 0) {
            indice = i; // Guarda o índice onde o utilizador foi localizado
            break;
        }
    }

    // Se o índice não foi alterado, o email não existe na base de dados
    if (indice == -1) return -1; 

    // Bloqueia o acesso se a conta estiver pendente de aprovação inicial ou reativação
    if (users[indice].estado == CONTA_PENDENTE_APROVACAO || 
        users[indice].estado == CONTA_PENDENTE_REATIVACAO) {
        return -4; 
    } 
    
    // Bloqueia o acesso se a conta tiver sido desativada (eliminada logicamente)
    if (users[indice].estado == CONTA_INATIVA) {
        return -3; 
    }

    // Compara a password introduzida com a password armazenada na struct
    if (strcmp(users[indice].password, pass) == 0) {
        return users[indice].id; // Login bem-sucedido: retorna o ID único do utilizador
    }

    return -2; // Erro: password incorreta
}

int validarIdentidadeRecuperacao(Utilizador users[], int total, char *email, char *telemovel) {
    int indice = -1;
    // Procura o utilizador pelo email fornecido
    for (int i = 0; i < total; i++) {
        if (strcmp(users[i].email, email) == 0) {
            indice = i;
            break;
        }
    }

    // Verificações de segurança em cascata para garantir a identidade
    if (indice == -1) return -1; // Utilizador não existe
    if (users[indice].estado != CONTA_ATIVA) return -2; // Apenas contas ativas podem recuperar pass
    if (strcmp(users[indice].telemovel, telemovel) != 0) return -3; // O telemóvel não coincide com o registo

    return indice; // Sucesso: identidade confirmada
}

/* =============================================================
   GESTÃO DE DADOS E ESTADOS
   ============================================================= */

int verificarStatusEmail(Utilizador users[], int total, char *email, int *index) {
    for (int k = 0; k < total; k++) {
        if (strcmp(users[k].email, email) == 0) {
            *index = k; // Retorna o índice por referência para uso externo
            // Se a conta existe mas está inativa, permite o fluxo de reativação
            if (users[k].estado == CONTA_INATIVA) return 1; 
            // Se a conta já existe e está ativa ou pendente, impede novo registo
            return 2; 
        }
    }
    return 0; // Email totalmente novo no sistema
}

int telemovelEmUso(Utilizador users[], int total, char *telemovel, int indexAtual) {
    for (int k = 0; k < total; k++) {
        // Verifica se o telemóvel existe noutro utilizador que não seja o próprio (indexAtual)
        if (k != indexAtual && strcmp(users[k].telemovel, telemovel) == 0) {
            return 1; // Duplicidade encontrada
        }
    }
    return 0; // Número disponível
}

void garantirAdminPadrao(Utilizador users[], int *totalUsers) {
    // Se o contador de utilizadores for zero, inicializa a conta mestra
    if (*totalUsers == 0) {
        users[0].id = 1; // O Administrador e Instituição ocupam sempre o ID 1
        strcpy(users[0].nome, "IPCA");
        strcpy(users[0].email, "admin@ipca.pt");
        strcpy(users[0].password, "admin"); 
        strcpy(users[0].dataNascimento, "01/01/2000");
        strcpy(users[0].telemovel, "253000000");
        users[0].estado = CONTA_ATIVA;

        *totalUsers = 1; // Atualiza o total de utilizadores na memória
        printf("\n[Sistema] Base de dados vazia. Admin criado (User: admin@ipca.pt / Pass: admin)\n");
        guardarUtilizadores(users, *totalUsers); // Persiste a criação no ficheiro
    }
}

void atualizarNomeUtilizador(Utilizador *user, char *novoNome) {
    // Validação básica de ponteiros e comprimento mínimo do nome
    if (user != NULL && novoNome != NULL && strlen(novoNome) >= 3) {
        strncpy(user->nome, novoNome, MAX_STRING - 1);
        user->nome[MAX_STRING - 1] = '\0'; // Garante o fecho da string
    }
}

void atualizarPasswordUtilizador(Utilizador *user, char *novaPass) {
    if (user != NULL && novaPass != NULL && strlen(novaPass) >= 4) {
        strncpy(user->password, novaPass, 49);
        user->password[49] = '\0';
    }
}

void desativarContaUtilizador(Utilizador *user) {
    if (user != NULL) {
        user->estado = CONTA_INATIVA; // Aplicação do estado de Soft Delete
    }
}

/* =============================================================
   CÁLCULOS E RANKINGS (REPUTAÇÃO E ATIVIDADE)
   ============================================================= */

float calcularMediaUtilizador(Feedback feedbacks[], int totalFeedbacks, int idUtilizador, int *quantidade) {
    float soma = 0.0f; 
    int contador = 0;

    for (int i = 0; i < totalFeedbacks; i++) {
        // Soma a nota se o alvo da avaliação (idAvaliado) for o utilizador em questão
        if (feedbacks[i].idAvaliado == idUtilizador) {
            soma += (float)feedbacks[i].nota;
            contador++;
        }
    }

    *quantidade = contador; // Retorna por ponteiro o total de votos recebidos

    // Proteção contra divisão por zero; se não houver votos, a média é zero
    if (contador > 0) return soma / (float)contador;

    return 0.0f;
}

void calcularRankingReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, UserStats ranking[]) {
    // 1. Preenchimento inicial do array de estatísticas com as médias calculadas
    for(int i = 0; i < totalUsers; i++) {
        ranking[i].indexUser = i;
        ranking[i].media = calcularMediaUtilizador(feedbacks, totalFeedbacks, users[i].id, &ranking[i].qtd);
    }

    // 2. Ordenação Bubble Sort descendente por média de avaliação
    for(int i = 0; i < totalUsers - 1; i++) {
        for(int j = 0; j < totalUsers - i - 1; j++) {
            // Critério: Maior média primeiro; se iguais, maior quantidade de avaliações desempata
            if(ranking[j].media < ranking[j+1].media || 
              (ranking[j].media == ranking[j+1].media && ranking[j].qtd < ranking[j+1].qtd)) {
                UserStats temp = ranking[j]; 
                ranking[j] = ranking[j+1]; 
                ranking[j+1] = temp;
            }
        }
    }
}

void calcularAtividadeUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes, UserActivityRanking ranking[]) {
    for (int i = 0; i < totalUsers; i++) {
        ranking[i].indexUser = i;
        // O Admin (ID 1) é marcado com -1 para ser excluído do ranking de atividade pública
        ranking[i].qtd = (users[i].id == 1) ? -1 : 0; 

        if (ranking[i].qtd == -1) continue;

        // Itera sobre o histórico de operações para contar movimentos válidos (Aceites ou Concluídos)
        for (int k = 0; k < totalOperacoes; k++) {
            if (operacoes[k].estado == ESTADO_OP_ACEITE || 
                operacoes[k].estado == ESTADO_OP_CONCLUIDO || 
                operacoes[k].estado == ESTADO_OP_DEVOLUCAO_PENDENTE) {
                
                // Incrementa se o utilizador for o requerente ou o dono do livro na transação
                if (operacoes[k].idRequerente == users[i].id || operacoes[k].idProprietario == users[i].id) {
                    ranking[i].qtd++;
                }
            }
        }
    }

    // Ordenação Bubble Sort para colocar os utilizadores mais ativos no topo
    for (int i = 0; i < totalUsers - 1; i++) {
        for (int j = 0; j < totalUsers - i - 1; j++) {
            if (ranking[j].qtd < ranking[j+1].qtd) {
                UserActivityRanking temp = ranking[j];
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }
}

int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes) {
    for (int i = 0; i < totalOperacoes; i++) {
        // Verifica se existe qualquer evidência de transação ligada a este ID
        if (operacoes[i].idRequerente == idUser || operacoes[i].idProprietario == idUser) {
            return 1; // Registos encontrados
        }
    }
    return 0; // Histórico limpo
}