#include <stdio.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isdigit
#include <stdlib.h>
#include <time.h>  // Necessário para obterDataAtual
#include "utils.h"
#include "files.h"
#include "transactions.h"
#include "struct.h"


void registarUtilizador(Utilizador users[], int *total, Operacao loans[], int totalLoans) {
    
    // 1. Validação de Limites (Só é critico se for conta nova, mas validamos já)
    if (*total >= MAX_USERS) {
        // Nota: Se for reativação, isto não seria problema, mas por segurança bloqueamos.
        printf("[Erro] Limite de utilizadores atingido!\n");
        return;
    }

    printf("\n=== REGISTO DE UTILIZADOR (IPCA) ===\n");

    // Variáveis de Controlo
    int indice = -1;      // Onde vamos guardar os dados?
    int isReativacao = 0; // É conta nova ou velha?
    char emailTemp[MAX_STRING];

    // --- 1. EMAIL (PRIMEIRO PASSO) ---
    int emailValido = 0;
    do {
        printf("Email Institucional (IPCA): ");
        lerString(emailTemp, MAX_STRING);
        
        // A. Validação de Formato
        if (!validarFormatoEmail(emailTemp)) {
            printf("[Erro] Formato invalido!\n");
            printf("- Alunos: aXXXXX@alunos.ipca.pt\n");
            printf("- Docentes: [Nome]@ipca.pt\n");
            continue; 
        }

        // B. Verificar Existência na BD
        int encontrado = -1;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].email, emailTemp) == 0) {
                encontrado = k;
                break;
            }
        }

        if (encontrado != -1) {
            // Se encontrou, verificamos o estado
            if (users[encontrado].ativo == INATIVO) {
                // SUCESSO: É uma reativação!
                printf("\n[Aviso] Detetamos uma conta antiga inativa associada a este email.\n");
                printf("Os seus dados serao atualizados.\n");
                indice = encontrado;
                isReativacao = 1;
                emailValido = 1;
            } else {
                // ERRO: Email já ativo
                printf("[Erro] Este email ja esta registado e ativo/pendente.\n");
            }
        } else {
            // SUCESSO: É um registo novo!
            indice = *total; // Usa a próxima posição livre
            isReativacao = 0;
            emailValido = 1;
        }

    } while (!emailValido);

    // Copiar o email validado para a struct definitiva
    strcpy(users[indice].email, emailTemp);


    // --- 2. NOME ---
    int nomeValido = 0;
    do {
        printf("Nome Completo: ");
        lerString(users[indice].nome, MAX_STRING);
        
        if (!stringNaoVazia(users[indice].nome)) {
            printf("[Erro] O nome nao pode estar vazio.\n");
        } 
        else if (!validarApenasLetras(users[indice].nome, MAX_STRING)) {
            printf("[Erro] O nome nao pode conter numeros.\n");
        } 
        else {
            nomeValido = 1;
        }
    } while (!nomeValido);


    // --- 3. TELEMOVEL ---
    int telemovelValido = 0;
    do {
        printf("Telemovel (9 digitos): ");
        lerString(users[indice].telemovel, sizeof(users[indice].telemovel));

        // A. Tamanho
        if (strlen(users[indice].telemovel) != 9) {
            printf("[Erro] 9 digitos.\n"); continue; 
        }

        // B. Apenas Números
        int apenasNumeros = 1;
        for (int k = 0; k < 9; k++) {
            if (!isdigit(users[indice].telemovel[k])) {
                apenasNumeros = 0; break;
            }
        }
        if (!apenasNumeros) { printf("[Erro] Apenas numeros.\n"); continue; }

        // C. Duplicados (Cuidado com reativação!)
        int existe = 0;
        for (int k = 0; k < *total; k++) {
            // Só é erro se o número existir NUMA OUTRA conta (k != indice)
            if (k != indice && strcmp(users[k].telemovel, users[indice].telemovel) == 0) {
                existe = 1; break;
            }
        }

        if (existe) printf("[Erro] Telemovel ja associado a outra conta.\n");
        else telemovelValido = 1; 

    } while (!telemovelValido); 


    // --- 4. DATA NASCIMENTO ---
    int idadeValida = 0;
    do {
        printf("Data Nascimento (DD/MM/AAAA): ");
        lerString(users[indice].dataNascimento, 11); 

        int idade = calcularIdade(users[indice].dataNascimento);

        if (idade == -1) printf("[Erro] Formato invalido.\n");
        else if (idade < 18) {
            printf("[Erro] Apenas maiores de 18 anos.\n");
            return; // Sai da função
        } 
        else idadeValida = 1;

    } while (!idadeValida);


    // --- 5. PASSWORD ---
    do {
        printf("Password: ");
        lerString(users[indice].password, 50);
    } while (!stringNaoVazia(users[indice].password));


    // --- 6. DEFINIÇÃO DE ESTADO E FUSÃO ---

    if (isReativacao) {
        // É REATIVAÇÃO: Mantém o ID antigo
        // users[indice].id já existe, não mexemos.
        
        // Verificar Fusão de Histórico
        if (verificarHistoricoUtilizador(users[indice].id, loans, totalLoans)) {
            users[indice].ativo = PENDENTE_REATIVACAO;
            printf("\n[Info] Detetamos historico antigo nesta conta.\n");
            printf("A conta ficou PENDENTE para reativacao.\n"); // Admin precisa aprovar. Faz fusão do histórico antigo.
        } 
        else {
            // Reativação sem histórico: Trata como novo para efeitos de whitelist
            if (emailExisteNaWhitelist(users[indice].email)) {
                users[indice].ativo = ATIVO;
                printf("[Sucesso] Conta reativada automaticamente.\n");
            } else {
                users[indice].ativo = PENDENTE_APROVACAO; 
                printf("[Info] Conta reativada. Aguarde validacao.\n");
            }
        }
    } 
    else {
        // É NOVO REGISTO
        users[indice].id = indice + 1; // Gera novo ID

        // Verificar Whitelist
        if (strcmp(users[indice].email, "admin@ipca.pt") == 0) {
            users[indice].ativo = ATIVO;
        }
        else if (emailExisteNaWhitelist(users[indice].email)) { // Função no files.c
            users[indice].ativo = ATIVO;
            printf("[Sucesso] Conta criada e ativa!\n");
        } 
        else {
            users[indice].ativo = PENDENTE_APROVACAO;
            printf("[Info] Registo efetuado. Aguarde validacao do Admin.\n");
        }

        (*total)++; // Só incrementa se for novo!
    }

    printf("Registo concluido para %s (ID: %d).\n", users[indice].nome, users[indice].id);
}

/**
 * @brief Realiza o login de um utilizador.
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 * @return Índice do utilizador no array se sucesso, ou código de erro negativo.
 */
int loginUtilizador(Utilizador users[], int total) {
    char email[MAX_STRING], pass[50];
    
    printf("\n=== LOGIN ===\n");
    printf("Email: "); 
    lerString(email, MAX_STRING);
    
    printf("Password: "); 
    lerString(pass, 50);

    for (int i = 0; i < total; i++) {
        // Passo 1: Email existe?
        if (strcmp(users[i].email, email) == 0) {
            
            // Passo 2: Password coincide?
            if (strcmp(users[i].password, pass) == 0) {
                
                // Passo 3: Verificar Estado da Conta
                if (users[i].ativo == ATIVO) {
                    return i; // SUCESSO
                } 
                else if (users[i].ativo == PENDENTE_APROVACAO || users[i].ativo == PENDENTE_REATIVACAO) {
                    printf("\n[Acesso Negado] A sua conta aguarda validacao do Administrador.\n");
                    return -4; 
                } 
                else { 
                    // Estado INATIVO (Eliminado)
                    printf("\n[Aviso] Esta conta foi eliminada anteriormente.\n");
                    printf("Por favor, va ao menu 'Registar' para reativar a sua conta.\n");
                    return -3; 
                }
            } else {
                printf("\n[Erro] Password incorreta.\n");
                return -2; 
            }
        }
    }
    return -1; // Email não encontrado
}

/**
 * @brief Valida o formato do email institucional do IPCA.
 * @param email A string do email a validar.
 * @return int Retorna 1 se o formato for válido, 0 caso contrário.
 */
int validarFormatoEmailIPCA(char *email) {
    // Regra Base: Admin
    if (strcmp(email, "admin@ipca.pt") == 0) return 1;

    char *arroba = strchr(email, '@');
    if (arroba == NULL) return 0;

    // Separar utilizador e domínio
    int tamanhoUser = arroba - email;
    char dominio[50];
    strcpy(dominio, arroba + 1);

    // --- CASO 1: ALUNOS (@alunos.ipca.pt) ---
    if (strcmp(dominio, "alunos.ipca.pt") == 0) {
        // Tem de começar por 'a' ou 'A'
        if (email[0] != 'a' && email[0] != 'A') return 0;
        
        // O resto do utilizador tem de ser números
        for (int i = 1; i < tamanhoUser; i++) {
            if (!isdigit(email[i])) return 0;
        }
        return 1; // Válido
    }

    // --- CASO 2: DOCENTES/INSTITUIÇÃO (@ipca.pt) ---
    else if (strcmp(dominio, "ipca.pt") == 0) {
        // Deve ser composto por letras ou pontos (ex: ldccastro, jose.silva)
        for (int i = 0; i < tamanhoUser; i++) {
            // Se encontrarmos um número, REJEITAMOS
            if (isdigit((unsigned char)email[i])) {
                return 0; 
            }
        }
        return 1; // Válido (não tem números)
    }

    return 0; // Domínio desconhecido
}

/**
 * @brief Recupera a password de um utilizador.
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 */
void recuperarPassword(Utilizador users[], int total) {
    char emailInput[MAX_STRING];
    char telInput[15];
    
    printf("\n=== RECUPERACAO DE CONTA ===\n");
    printf("Introduza o seu Email: ");
    lerString(emailInput, MAX_STRING);

    // 1. Procurar o utilizador pelo Email
    int indiceEncontrado = -1;
    
    for (int i = 0; i < total; i++) {
        if (strcmp(users[i].email, emailInput) == 0) {
            indiceEncontrado = i;
            break; // Parar assim que encontrar
        }
    }

    // Se não encontrou o email, aborta por segurança
    // (Em sistemas reais, às vezes não se diz "email não existe" para não revelar dados,
    // mas num projeto académico a clareza ajuda).
    if (indiceEncontrado == -1) {
        printf("[Erro] Email nao encontrado no sistema.\n");
        return;
    }

    // 2. Validação de Segurança (Simulação de 2FA)
    printf("Para seguranca, confirme o seu Telemovel: ");
    lerString(telInput, 15);

    // Compara o telemóvel inserido com o que está gravado na struct
    if (strcmp(users[indiceEncontrado].telemovel, telInput) == 0) {
        
        // 3. Segurança OK -> Pedir nova password
        printf("\n[Identidade Confirmada]\n");
        printf("Nova Password: ");
        lerString(users[indiceEncontrado].password, 50);
        
        printf("[Sucesso] A sua password foi alterada.\n");
        
    } else {
        printf("[Erro] O telemovel nao corresponde ao registo deste email.\n");
    }
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
    printf("Estado: %s\n", (user.ativo == ATIVO ? "Ativo" : "Inativo"));
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
        user->ativo = INATIVO; // Soft Delete (Muda estado para 0)
        printf("[Conta Eliminada] Lamentamos ve-lo partir.\n");
        return 1; // Confirma que eliminou
    } else {
        printf("[Cancelado] A conta mantem-se ativa.\n");
        return 0; // Não eliminou
    }
}