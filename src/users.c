#include <stdio.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isdigit
#include <stdlib.h>
#include <time.h>  // Necessário para obterDataAtual

#include "utils.h"
#include "interface.h"
#include "files.h"
#include "transactions.h"
#include "structs.h"


void registarUtilizador(Utilizador users[], int *total) {
    printf("\n=== REGISTO DE UTILIZADOR (Digite '0' para cancelar) ===\n");

    int indice = -1;
    int isReativacao = 0;
    char entradaTemp[MAX_STRING];

    // --- 1. EMAIL E VERIFICAÇÃO DE TIPO ---
    do {
        printf("Email Institucional: ");
        lerString(entradaTemp, MAX_STRING);
        if (strcmp(entradaTemp, "0") == 0) return;

        if (!validarFormatoEmailIPCA(entradaTemp)) {
            printf("[Erro] Formato invalido!\n");
            continue; 
        }

        int encontrado = -1;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].email, entradaTemp) == 0) {
                encontrado = k;
                break;
            }
        }

        if (encontrado != -1) {
            if (users[encontrado].estado == CONTA_INATIVA) {
                printf("\n[Info] Conta antiga detetada. Iniciando processo de reativacao...\n");
                indice = encontrado;
                isReativacao = 1;
                break;
            } else {
                printf("[Erro] Este email ja se encontra registado e ativo (ou pendente).\n");
                continue;
            }
        } else {
            if (*total >= MAX_UTILIZADORES) {
                printf("[Erro] Limite de utilizadores atingido.\n");
                return;
            }
            indice = *total;
            isReativacao = 0;
            break;
        }
    } while (1);

    char emailFinal[MAX_STRING];
    strcpy(emailFinal, entradaTemp);

    // --- 2. NOME (Condicional) ---
    if (isReativacao) {
        printf("[Info] Nome mantido: %s\n", users[indice].nome);
    } else {
        do {
            printf("Nome Completo: ");
            lerString(entradaTemp, MAX_STRING);
            if (strcmp(entradaTemp, "0") == 0) return;
            if (stringNaoVazia(entradaTemp) && validarApenasLetras(entradaTemp, MAX_STRING)) {
                strcpy(users[indice].nome, entradaTemp);
                break;
            }
            printf("[Erro] Nome invalido.\n");
        } while (1);
    }

    // --- 3. TELEMOVEL (Opção de manter ou atualizar na reativação) ---
    int atualizarTelemovel = 1;
    if (isReativacao) {
        printf("\nTelemovel atual: %s\n", users[indice].telemovel);
        printf("1. Manter numero atual\n2. Atualizar numero\n0. CANCELAR REGISTO\n");
        
        // Alterado o mínimo para 0 para permitir o cancelamento
        int opTel = lerInteiro("Opcao: ", 0, 2);
        
        if (opTel == 0) {
            printf("[Info] Reativacao cancelada.\n");
            return; // <--- Sai da função aqui
        }
        if (opTel == 1) atualizarTelemovel = 0;
    }

    if (atualizarTelemovel) {
        do {
            printf("Telemovel (9 digitos): ");
            lerString(entradaTemp, MAX_STRING);
            if (strcmp(entradaTemp, "0") == 0) return;

            if (strlen(entradaTemp) == 9 && validarApenasNumeros(entradaTemp)) {
                int existe = 0;
                for (int k = 0; k < *total; k++) {
                    if (k != indice && strcmp(users[k].telemovel, entradaTemp) == 0) { 
                        existe = 1; break; 
                    }
                }
                if (!existe) { strcpy(users[indice].telemovel, entradaTemp); break; }
                printf("[Erro] Telemovel ja registado.\n");
            } else printf("[Erro] Invalido.\n");
        } while (1);
    }

    // --- 4. DATA NASCIMENTO (Condicional) ---
    if (isReativacao) {
        printf("[Info] Data de Nascimento mantida: %s\n", users[indice].dataNascimento);
    } else {
        do {
            printf("Data de Nascimento (DD/MM/AAAA): ");
            lerString(users[*total].dataNascimento, 11);
            
            if (!validarData(users[*total].dataNascimento)) {
                printf("[Erro] Data invalida ou formato incorreto. Use DD/MM/AAAA.\n");
            }
        } while (!validarData(users[*total].dataNascimento));
    }

    // --- 5. PASSWORD --- (Sempre pedimos uma nova por segurança)
    do {
        printf("Defina uma Password (min. 4 caracteres): ");
        lerString(entradaTemp, 50);
        if (strcmp(entradaTemp, "0") == 0) return;
        if (strlen(entradaTemp) >= 4) { strcpy(users[indice].password, entradaTemp); break; }
        printf("[Erro] Password muito curta.\n");
    } while (1);

    // --- 6. FINALIZAR ---
    strcpy(users[indice].email, emailFinal);

    if (isReativacao) {
        users[indice].estado = CONTA_PENDENTE_REATIVACAO;
        char detalhes[100];
        sprintf(detalhes, "Reativacao solicitada: %s", users[indice].email);
        registarLog(users[indice].id, "REGISTO_REATIVACAO", detalhes);
        printf("\n[Sucesso] Pedido de reativacao enviado para o Administrador.\n");
    } else {
        users[indice].id = *total + 1; 
        users[indice].estado = CONTA_PENDENTE_APROVACAO;
        registarLog(users[indice].id, "REGISTO_NOVO", "Novo registo efetuado.");
        (*total)++; 
        printf("\n[Sucesso] Registo enviado para aprovacao.\n");
    }
    
    guardarUtilizadores(users, *total); 
}

int loginUtilizador(Utilizador users[], int totalUsers) {
    char email[MAX_STRING], pass[50];
    int indice = -1;

    printf("\n=== LOGIN (0 para cancelar) ===\n");
    printf("Email: "); 
    lerString(email, MAX_STRING);
    
    if (strcmp(email, "0") == 0) return -5;

    // PASSO 1: O email existe?
    for (int i = 0; i < totalUsers; i++) {
        if (strcmp(users[i].email, email) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("\n[Erro] Email nao encontrado.\n");
        esperarEnter();
        return -1;
    }

    // PASSO 2: Validar Estados
    if (users[indice].estado == CONTA_PENDENTE_APROVACAO || 
        users[indice].estado == CONTA_PENDENTE_REATIVACAO) {
        
        printf("\n[Acesso Negado] A sua conta aguarda validacao do Administrador.\n");
        esperarEnter();
        return -4;
    } 
    // Corrigido: Apenas entra aqui se for especificamente INATIVA
    else if (users[indice].estado == CONTA_INATIVA) {
        printf("\n[Acesso Negado] Esta conta foi eliminada ou desativada.\n");
        printf("Para recuperar o acesso, deve fazer um novo Registo com este email.\n");
        esperarEnter();
        return -3;
    }

    // PASSO 3: Password
    printf("Password: "); 
    lerString(pass, 50);
    if (strcmp(pass, "0") == 0) return -5;

    if (strcmp(users[indice].password, pass) == 0) {
        limparEcra();
        
        // --- REGISTO DE LOG: SUCESSO ---
        registarLog(users[indice].id, "LOGIN", "Sessao iniciada com sucesso.");

        if (strcmp(users[indice].email, "admin@ipca.pt") == 0) {
                printf("\nBem-vindo Administrador (IPCA)!\n"); 
        }
        else {
            if (strstr(users[indice].email, "@alunos.ipca.pt") != NULL)
                printf("\nBem-vindo Aluno(a), %s!\n", users[indice].nome);
            else
                printf("\nBem-vindo Docente, %s!\n", users[indice].nome);
        }

        return users[indice].id; 
        
    } else {
        // --- REGISTO DE LOG: FALHA (Opcional, mas muito bom para nota final) ---
        char detalheFalha[100];
        sprintf(detalheFalha, "Tentativa de login falhada para o email: %s", email);
        registarLog(0, "LOGIN_FAIL", detalheFalha);

        printf("\n[Erro] Password incorreta.\n");
        esperarEnter();
        return -2;
    }
}

void recuperarPassword(Utilizador users[], int total) {
    char emailInput[MAX_STRING];
    char telInput[15];
    
    printf("\n=== RECUPERACAO DE CONTA (0 para cancelar) ===\n");
    
    // 1. Pedir Email
    printf("Introduza o seu Email: ");
    lerString(emailInput, MAX_STRING);
    if (strcmp(emailInput, "0") == 0) return;

    // 2. Procurar o utilizador
    int indiceEncontrado = -1;
    for (int i = 0; i < total; i++) {
        if (strcmp(users[i].email, emailInput) == 0) {
            indiceEncontrado = i;
            break;
        }
    }

    if (indiceEncontrado == -1) {
        printf("[Erro] Email nao encontrado no sistema.\n");
        // REMOVIDO: esperarEnter(); - Deixa a pausa para o menu principal
        return;
    }

    // --- NOVA VALIDAÇÃO: Só permite se a conta estiver ATIVA ---
    if (users[indiceEncontrado].estado != CONTA_ATIVA) {
        printf("[Erro] So e possivel recuperar passwords de contas ATIVAS.\n");
        printf("A sua conta encontra-se em estado: %s\n", 
               (users[indiceEncontrado].estado == CONTA_INATIVA) ? "Inativa" : "Pendente de Aprovacao");
        return;
    }

    // 3. Validação de Telemóvel (Identidade)
    printf("Confirme o seu Telemovel (9 digitos): ");
    lerString(telInput, 15);
    if (strcmp(telInput, "0") == 0) return;

    if (strcmp(users[indiceEncontrado].telemovel, telInput) == 0) {
        printf("\n[Identidade Confirmada]\n");
        
        char novaPass[50];
        char confirmPass[50]; 
        int passwordAceite = 0; 

        // 4. Ciclo para definir nova password
        do {
            printf("\nNova Password (min 4 chars): ");
            lerString(novaPass, 50);

            if (strcmp(novaPass, "0") == 0) {
                printf("Operacao cancelada.\n");
                return;
            }

            if (!stringNaoVazia(novaPass)) {
                printf("[Erro] A password nao pode estar vazia.\n");
                continue; 
            }

            if (strlen(novaPass) < 4) {
                printf("[Erro] A password deve ter pelo menos 4 caracteres.\n");
                continue;
            }

            if (strcmp(novaPass, users[indiceEncontrado].password) == 0) {
                printf("[Erro] A nova password nao pode ser igual a atual.\n");
                continue;
            }

            printf("Confirme a Nova Password: ");
            lerString(confirmPass, 50);

            if (strcmp(novaPass, confirmPass) != 0) {
                printf("[Erro] As passwords nao coincidem. Tente novamente.\n");
            } else {
                passwordAceite = 1;
            }

        } while (!passwordAceite);

        // 5. Aplicar Alterações
        strcpy(users[indiceEncontrado].password, novaPass);
        
        // --- LOG DE SEGURANÇA ---
        char detalhes[100];
        sprintf(detalhes, "Password recuperada via confirmacao de telemovel.");
        registarLog(users[indiceEncontrado].id, "PASS_RECOVERY", detalhes);

        printf("\n[Sucesso] A sua password foi recuperada e alterada.\n");

    } else {
        printf("[Erro] O telemovel nao corresponde ao registo deste email.\n");
        // REMOVIDO: esperarEnter();
    }
    
    // Deixamos este esperarEnter final apenas para o caso de sucesso 
    // ou se quiseres que o utilizador veja o resultado antes do menu limpar.
    // Se o menu principal já tiver um, podes remover este também.
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

void garantirAdminPadrao(Utilizador users[], int *totalUsers) {
    if (*totalUsers == 0) {
        users[0].id = 1;
        
        // Define os dados de login
        strcpy(users[0].nome, "IPCA");
        strcpy(users[0].email, "admin@ipca.pt");
        strcpy(users[0].password, "admin"); // Password simples para testes
        strcpy(users[0].dataNascimento, "01/01/2000");
        strcpy(users[0].telemovel, "253000000");

        users[0].estado = CONTA_ATIVA;

        *totalUsers = 1; // Agora temos 1 utilizador
        
        printf("\n[Sistema] Base de dados vazia. Admin criado (User: admin@ipca.pt / Pass: admin)\n");
        
        guardarUtilizadores(users, *totalUsers); 
    }
}