#include <stdio.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isdigit
#include <stdlib.h>
#include <time.h>  // Necessário para obterDataAtual

#include "utils.h"
//#include "files.h"
//#include "transactions.h"
#include "structs.h"


void registarUtilizador(Utilizador users[], int *total, Operacao loans[], int totalLoans) {
    
    // 1. Validação de Limites (Só é critico se for conta nova, mas validamos já)
    if (*total >= MAX_USERS) {
        // Nota: Se for reativação, isto não seria problema, mas por segurança bloqueamos.
        printf("[Erro] Limite de utilizadores atingido!\n");
        return;
    }

    printf("\n=== REGISTO DE UTILIZADOR (Digite '0' para cancelar) ===\n");

    int indice = -1;
    int isReativacao = 0;
    char entradaTemp[MAX_STRING];

    // --- 1. EMAIL ---
    do {
        printf("Email Institucional: ");
        lerString(entradaTemp, MAX_STRING);
        if (strcmp(entradaTemp, "0") == 0) return; // CANCELAR

        if (!validarFormatoEmailIPCA(entradaTemp)) {
            printf("[Erro] Formato invalido! (Alunos: a123@alunos.ipca.pt | Docentes: nome@ipca.pt)\n");
            continue; 
        }

        // Verificar Existência
        int encontrado = -1;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].email, entradaTemp) == 0) {
                encontrado = k;
                break;
            }
        }

        if (encontrado != -1) {
            if (users[encontrado].estado == CONTA_INATIVA) {
                printf("\n[Aviso] Conta inativa detetada. Procedendo a reativacao...\n");
                indice = encontrado;
                isReativacao = 1;
                break;
            } else {
                printf("[Erro] Este email ja se encontra ativo ou pendente.\n");
                continue;
            }
        } else {
            indice = *total;
            isReativacao = 0;
            break;
        }
    } while (1);

    // Guardar email temporário
    char emailFinal[MAX_STRING];
    strcpy(emailFinal, entradaTemp);

    // --- 2. NOME ---
    do {
        printf("Nome Completo: ");
        lerString(entradaTemp, MAX_STRING);
        if (strcmp(entradaTemp, "0") == 0) return; // CANCELAR
        
        if (!stringNaoVazia(entradaTemp) || !validarApenasLetras(entradaTemp, MAX_STRING)) {
            printf("[Erro] Nome invalido (nao pode conter numeros ou estar vazio).\n");
            continue;
        }
        strcpy(users[indice].nome, entradaTemp);
        break;
    } while (1);

    // --- 3. TELEMOVEL ---
    do {
        printf("Telemovel (9 digitos): ");
        lerString(entradaTemp, MAX_STRING);
        if (strcmp(entradaTemp, "0") == 0) return; // CANCELAR

        if (strlen(entradaTemp) != 9) { printf("[Erro] Deve ter 9 digitos.\n"); continue; }
        
        int soNumeros = 1;
        for(int k=0; k<9; k++) if(!isdigit(entradaTemp[k])) soNumeros = 0;
        if(!soNumeros) { printf("[Erro] Apenas numeros.\n"); continue; }

        int existe = 0;
        for (int k = 0; k < *total; k++) {
            if (k != indice && strcmp(users[k].telemovel, entradaTemp) == 0) { existe = 1; break; }
        }
        if (existe) printf("[Erro] Telemovel ja registado.\n");
        else { strcpy(users[indice].telemovel, entradaTemp); break; }
    } while (1);

    // --- 4. DATA NASCIMENTO ---
    do {
        printf("Data Nascimento (DD/MM/AAAA): ");
        lerString(entradaTemp, 11);
        if (strcmp(entradaTemp, "0") == 0) return; // CANCELAR

        int idade = calcularIdade(entradaTemp);
        if (idade == -1) printf("[Erro] Formato invalido.\n");
        else if (idade < 18) {
            printf("[Erro] Registo apenas para maiores de 18 anos.\n");
            return;
        } else { strcpy(users[indice].dataNascimento, entradaTemp); break; }
    } while (1);

    // --- 5. PASSWORD ---
    do {
        printf("Password: ");
        lerString(entradaTemp, 50);
        if (strcmp(entradaTemp, "0") == 0) return; // CANCELAR
        if (stringNaoVazia(entradaTemp)) {
            strcpy(users[indice].password, entradaTemp);
            break;
        }
    } while (1);

    // Aplicar o email guardado no início
    strcpy(users[indice].email, emailFinal);

    // --- 6. ESTADO FINAL ---
    if (isReativacao) {
        if (verificarHistoricoUtilizador(users[indice].id, operacoes, totalOperacoes)) {
            users[indice].estado = CONTA_PENDENTE_REATIVACAO;
            printf("\n[Info] Conta aguarda validacao do Administrador (tem historico).\n");
        } else {
            users[indice].estado = CONTA_PENDENTE_APROVACAO;
            printf("\n[Info] Reativacao pendente de aprovacao.\n");
        }
    } else {
        users[indice].id = indice + 1;
        if (strcmp(users[indice].email, "admin@ipca.pt") == 0) {
            users[indice].estado = CONTA_ATIVA;
        } else {
            users[indice].estado = CONTA_PENDENTE_APROVACAO;
            printf("\n[Info] Registo efetuado com sucesso. Aguarde aprovacao.\n");
        }
        (*total)++;
    }
}

int loginUtilizador(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes) {
    char email[MAX_STRING], pass[50];
    int indice = -1;

    printf("\n=== LOGIN (0 para cancelar) ===\n");
    printf("Email: "); 
    lerString(email, MAX_STRING);
    
    // Se o utilizador digitar 0, cancela imediatamente
    if (strcmp(email, "0") == 0) return -5;

    // PASSO 1: O email existe?
    for (int i = 0; i < totalUsers; i++) { // Corrigido para totalUsers
        if (strcmp(users[i].email, email) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("\n[Erro] Email nao encontrado.\n");
        return -1;
    }

    // PASSO 2: A conta está em que estado?
    if (users[indice].estado == CONTA_PENDENTE_APROVACAO || users[indice].estado == CONTA_PENDENTE_REATIVACAO) {
        printf("\n[Acesso Negado] A sua conta aguarda validacao do Administrador.\n");
        return -4;
    } 
    else if (users[indice].estado == CONTA_INATIVA) {
        printf("\n[Aviso] Esta conta foi eliminada anteriormente.\n");
        printf("Por favor, use o menu 'Registar' para a reativar.\n");
        return -3;
    }

    // PASSO 3: Password (só pedimos se a conta existir e estiver ativa)
    printf("Password: "); 
    lerString(pass, 50);
    if (strcmp(pass, "0") == 0) return -5;

    if (strcmp(users[indice].password, pass) == 0) {
        // SAUDAÇÃO E NOTIFICAÇÕES (Ignorado se for Admin)
        if (strcmp(users[indice].email, "admin@ipca.pt") != 0) {
            
            if (strstr(users[indice].email, "@alunos.ipca.pt") != NULL)
                printf("\nBem-vindo Aluno %s!\n", users[indice].nome);
            else
                printf("\nBem-vindo Docente %s!\n", users[indice].nome);

            // Chamada da função de notificações
            verificarNotificacoes(operacoes, totalOperacoes, users[indice].id);
            
            // Pausa para o utilizador ler a saudação e os alertas
            esperarEnter(); 
        }

        return indice; // Retorna o índice para o main/menuModoVisitante
    } else {
        printf("\n[Erro] Password incorreta.\n");
        return -2;
    }
}

void recuperarPassword(Utilizador users[], int total) {
    char emailInput[MAX_STRING];
    char telInput[15];
    
    printf("\n=== RECUPERACAO DE CONTA (0 para cancelar) ===\n");
    printf("Introduza o seu Email: ");
    lerString(emailInput, MAX_STRING);
    if (strcmp(emailInput, "0") == 0) return;

    // 1. Procurar o utilizador
    int indiceEncontrado = -1;
    for (int i = 0; i < total; i++) {
        if (strcmp(users[i].email, emailInput) == 0) {
            indiceEncontrado = i;
            break;
        }
    }

    if (indiceEncontrado == -1) {
        printf("[Erro] Email nao encontrado no sistema.\n");
        return;
    }

    // 2. Validação de Telemóvel
    printf("Confirme o seu Telemovel (9 digitos): ");
    lerString(telInput, 15);
    if (strcmp(telInput, "0") == 0) return;

    if (strcmp(users[indiceEncontrado].telemovel, telInput) == 0) {
        printf("\n[Identidade Confirmada]\n");
        
        // 3. Nova Password com validação de "não vazia"
        char novaPass[50];
        do {
            printf("Nova Password: ");
            lerString(novaPass, 50);
            if (strcmp(novaPass, "0") == 0) return;
        } while (!stringNaoVazia(novaPass));

        strcpy(users[indiceEncontrado].password, novaPass);
        printf("[Sucesso] A sua password foi alterada.\n");
        
    } else {
        printf("[Erro] O telemovel nao corresponde ao registo deste email.\n");
    }
    esperarEnter();
}

/**
 * @brief Mostra os dados do perfil do utilizador.
 * @param user Utilizador cujo perfil será mostrado.
 */
void mostrarPerfil(Utilizador user) {
    printf("\n=== O MEU PERFIL ===\n");
    printf("ID: %d\n", user.id);
    printf("Nome: %s\n", user.nome);
    printf("Email: %s\n", user.email);
    printf("Telemovel: %s\n", user.telemovel);
    printf("Data Nascimento: %s\n", user.dataNascimento);
    printf("Estado: %s\n", (user.estado == CONTA_ATIVA ? "Ativo" : "Inativo"));
    printf("====================\n");
}

/**
 * @brief Edita os dados do perfil do utilizador.
 * @param user Ponteiro para o utilizador a ser editado.
 */
void editarPerfil(Utilizador *user) {
    int opcao;
    printf("\n=== EDITAR PERFIL ===\n");
    printf("1. Alterar Nome\n");
    printf("2. Alterar Password\n");
    printf("0. Cancelar\n");
    
    // Assumindo que tens o lerInteiro() disponível
    // Se não tiveres o import do utils.h aqui, confirma se está no topo
    opcao = lerInteiro("Opcao: ", 0, 2);

    if (opcao == 1) {
        printf("Novo Nome: ");
        lerString(user->nome, MAX_STRING); // Usamos -> porque user é um ponteiro
        printf("[Sucesso] Nome atualizado.\n");
    } 
    else if (opcao == 2) {
        printf("Nova Password: ");
        lerString(user->password, 50);
        printf("[Sucesso] Password atualizada.\n");
    } else {
        printf("[Cancelado] Nenhuma alteracao efetuada.\n");
    }
}

/**
 * @brief Elimina (desativa) a conta do utilizador.
 * @param user Ponteiro para o utilizador a ser eliminado.
 * @return 1 se a conta foi eliminada, 0 se cancelado.
 */
int eliminarConta(Utilizador *user) {
    char confirmacao[10];
    printf("\n!!! ZONA DE PERIGO !!!\n");
    printf("Tem a certeza que deseja eliminar a sua conta?\n");
    printf("Esta acao e irreversivel e perdera acesso aos seus livros.\n");
    printf("Escreva 'ELIMINAR' para confirmar: ");
    
    lerString(confirmacao, 10);
    
    if (strcmp(confirmacao, "ELIMINAR") == 0) {
        user->estado = CONTA_INATIVA; // Soft Delete (Muda estado para 0)
        printf("[Conta Eliminada] Lamentamos ve-lo partir.\n");
        return 1; // Confirma que eliminou
    } else {
        printf("[Cancelado] A conta mantem-se ativa.\n");
        return 0; // Não eliminou
    }
}