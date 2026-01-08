#include <stdio.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isdigit
#include <stdlib.h>
#include <time.h>  // Necessário para obterDataAtual
#include "users.h"
#include "utils.h"
#include "files.h"
#include "interface.h"

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

/**
 * @brief Regista um novo utilizador no sistema.
 * @param users Array de estruturas Utilizador (base de dados em memória).
 * @param total Apontador para o contador total de utilizadores.
 */
void registarUtilizador(Utilizador users[], int *total) {
    if (*total >= MAX_USERS) {
        printf("[Erro] Limite de utilizadores atingido!\n");
        return;
    }

    int i = *total;
    printf("\n=== REGISTO DE UTILIZADOR ===\n");

    // --- 1. NOME (Não aceita números) ---
    int nomeValido = 0;
    do {
        printf("Nome Completo: ");
        lerString(users[i].nome, MAX_STRING);
        
        if (!stringNaoVazia(users[i].nome)) {
            printf("[Erro] O nome nao pode estar vazio.\n");
        } 
        else if (!validarFormatoNome(users[i].nome)) {
            printf("[Erro] O nome nao pode conter numeros.\n");
        } 
        else {
            nomeValido = 1;
        }
    } while (!nomeValido);

    // --- EMAIL IPCA ---
    int emailValido = 0;
    do {
        printf("Email Institucional (IPCA): ");
        lerString(users[i].email, MAX_STRING);
        
        // 1. Validação de Formato Rigorosa
        if (!validarFormatoEmailIPCA(users[i].email)) {
            printf("[Erro] Formato invalido!\n");
            printf("- Alunos: aXXXXX@alunos.ipca.pt\n");
            printf("- Docentes: XXXXX@ipca.pt\n");
            continue;
        }

        // B. Validação de Duplicado
        int duplicado = 0;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].email, users[i].email) == 0) {
                duplicado = 1;
                break;
            }
        }

        if (duplicado) {
            printf("[Erro] Este email ja esta registado.\n");
        } else {
            emailValido = 1;
        }

    } while (!emailValido);

    // --- 3. TELEMOVEL (9 Dígitos) ---
    int telemovelValido = 0;
    do {
        printf("Telemovel (9 digitos): ");
        lerString(users[i].telemovel, sizeof(users[i].telemovel));

        if (strlen(users[i].telemovel) != 9) {
            printf("[Erro] O numero deve ter exatamente 9 digitos.\n");
            continue; 
        }

        int apenasNumeros = 1;
        for (int k = 0; k < 9; k++) {
            if (!isdigit(users[i].telemovel[k])) {
                apenasNumeros = 0;
                break;
            }
        }

        if (!apenasNumeros) {
            printf("[Erro] Insira apenas numeros.\n");
            continue; 
        }

        int existe = 0;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].telemovel, users[i].telemovel) == 0) {
                existe = 1;
                break;
            }
        }

        if (existe) {
            printf("[Erro] Este telemovel ja existe.\n");
        } else {
            telemovelValido = 1; 
        }

    } while (!telemovelValido); 

    // --- 4. DATA DE NASCIMENTO ---
    int idadeValida = 0;
    do {
        printf("Data Nascimento (DD/MM/AAAA): ");
        lerString(users[i].dataNascimento, 11); 

        int idade = calcularIdade(users[i].dataNascimento);

        if (idade == -1) {
            printf("[Erro] Formato invalido. Use DD/MM/AAAA.\n");
        } 
        else if (idade < 18) {
            printf("[Erro] Apenas maiores de 18 anos. (Idade: %d)\n", idade);
            return; // Sai da função, não deixa registar
        } 
        else {
            idadeValida = 1;
        }

    } while (!idadeValida);

    // --- 5. PASSWORD ---
    do {
        printf("Password: ");
        lerString(users[i].password, 50);

        if (!stringNaoVazia(users[i].password)) {
            printf("[Erro] A password nao pode estar vazia.\n");
        }
    } while (!stringNaoVazia(users[i].password));

    // --- CONFIGURAÇÃO FINAL DO ESTADO ---
    users[i].id = i + 1;
    
    // VERIFICAÇÃO NA WHITELIST
    if (strcmp(users[i].email, "admin@ipca.pt") == 0) {
        users[i].ativo = ATIVO; // Admin é sempre ativo
    }
    else if (emailExisteNaWhitelist(users[i].email)) {
        users[i].ativo = ATIVO; // Está na lista, entra direto
        printf("[Sistema] Email reconhecido na base de dados. Conta ATIVA.\n");
    } 
    else {
        users[i].ativo = PENDENTE; // Não está na lista, fica à espera
        printf("[Aviso] O seu email nao consta na base de dados.\n");
        printf("A sua conta ficou PENDENTE de validacao pelo Administrador.\n");
    }

    (*total)++;
}


    // ... dentro do if (passwords coincidem) ...



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

    // Percorrer a base de dados
    for (int i = 0; i < total; i++) {
        
        // Passo 1: Encontramos o Email?
        if (strcmp(users[i].email, email) == 0) {
            
            // Passo 2: A password coincide?
            if (strcmp(users[i].password, pass) == 0) {

                if (users[i].ativo == ATIVO) {
                    return i; // Login OK
                } 
                
                // Passo 3: A conta está ativa?
                if (users[i].ativo == ATIVO) {
                    return i; // SUCESSO! Retorna a posição do user
                } else if (users[i].ativo == PENDENTE) {
                    printf("\n[Acesso Negado] A sua conta aguarda validacao do Administrador.\n");
                    return -4; // Erro: Conta pendente
                } else {
                    return -3; // Erro: Conta eliminada
                }
            } else {
                return -2; // Erro: Encontrou email, mas password errada
            }
        }
    }

    // Se o ciclo acabou e não entrou no primeiro if, o email não existe
    return -1; // Erro: Email não encontrado
}

/**
 * @brief Valida o formato do email segundo as regras do IPCA.
 * @param email Email a ser validado.
 * @return int
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
        // O utilizador tem de ser apenas números (ex: 24610)
        for (int i = 0; i < tamanhoUser; i++) {
            if (!isdigit(email[i])) return 0;
        }
        return 1; // Válido
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
    printf("Opcao: ");
    
    // Assumindo que tens o lerInteiro() disponível
    // Se não tiveres o import do utils.h aqui, confirma se está no topo
    opcao = lerInteiro(); 

    if (opcao == 1) {
        printf("Novo Nome: ");
        lerString(user->nome, MAX_STRING); // Usamos -> porque user é um ponteiro
        printf("[Sucesso] Nome atualizado.\n");
    } 
    else if (opcao == 2) {
        printf("Nova Password: ");
        lerString(user->password, 50);
        printf("[Sucesso] Password atualizada.\n");
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