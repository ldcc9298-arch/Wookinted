#include <stdio.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isdigit
#include <stdlib.h>
#include <time.h>  // Necessário para obterDataAtual
#include "users.h"
#include "utils.h"
#include "files.h"

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
 * @param users Array de utilizadores.
 * @param total Ponteiro para o número total de utilizadores.
 */
void registarUtilizador(Utilizador users[], int *total) {
    // 1. Validação de Limites
    if (*total >= MAX_USERS) {
        printf("[Erro] Limite de utilizadores atingido!\n");
        return;
    }

    int i = *total; // Próxima posição livre
    printf("\n=== REGISTO DE UTILIZADOR ===\n");

    // 2. Recolha de Dados
    printf("Nome Completo: ");
    lerString(users[i].nome, MAX_STRING);

    printf("Email: ");
    lerString(users[i].email, MAX_STRING);
    
    // Verificar email unico
    for (int k = 0; k < *total; k++) {
        if (strcmp(users[k].email, users[i].email) == 0) {
            printf("[Erro] Este email ja esta registado.\n");
            return; 
        }
    }

    // 4. Recolha e validação do telemóvel
    int telemovelValido = 0;
    do {
        printf("Telemovel (9 digitos): ");
        lerString(users[i].telemovel, sizeof(users[i].telemovel));

        // A. VALIDAR TAMANHO
        // strlen conta quantos caracteres estão na string
        if (strlen(users[i].telemovel) != 9) {
            printf("[Erro] O numero deve ter exatamente 9 digitos.\n");
            continue; // Volta ao início do do-while
        }

        // B. VALIDAR SE SÃO APENAS NÚMEROS
        int apenasNumeros = 1;
        for (int k = 0; k < 9; k++) {
            if (!isdigit(users[i].telemovel[k])) {
                apenasNumeros = 0;
                break;
            }
        }

        if (!apenasNumeros) {
            printf("[Erro] Insira apenas numeros, sem letras ou espacos.\n");
            continue; // Volta ao início
        }

        // C. VALIDAR SE JÁ EXISTE (Duplicado)
        int existe = 0;
        for (int k = 0; k < *total; k++) {
            if (strcmp(users[k].telemovel, users[i].telemovel) == 0) {
                existe = 1;
                break;
            }
        }

        if (existe) {
            printf("[Erro] Este telemovel ja esta registado no sistema.\n");
            // Aqui não fazemos 'return', apenas avisamos e o loop repete
        } else {
            // Se passou em todos os testes (Tamanho, Digitos e Unicidade)
            telemovelValido = 1; 
        }

    } while (!telemovelValido); // Repete enquanto não for válido

    // 5. Validação da Data de Nascimento (+18)
    int idadeValida = 0;
    do {
        printf("Data Nascimento (DD/MM/AAAA): ");
        lerString(users[i].dataNascimento, 11); // Tamanho exato 11

        int idade = calcularIdade(users[i].dataNascimento);

        if (idade == -1) {
            printf("[Erro] Formato invalido. Use DD/MM/AAAA (ex: 15/05/2000)\n");
        } 
        else if (idade < 18) {
            printf("[Erro] Apenas maiores de 18 anos podem registar-se. (Idade calculada: %d)\n", idade);
            return; 
        } 
        else {
            idadeValida = 1;
        }

    } while (!idadeValida);

    printf("Password: ");
    lerString(users[i].password, 50);

    // 4. Configuração Final
    users[i].id = i + 1;    // ID sequencial
    users[i].ativo = ATIVO; // Conta nasce ativa

    (*total)++; // Atualiza o contador global
    printf("[Sucesso] Conta criada com ID %d.\n", users[i].id);
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

    // Percorrer a base de dados
    for (int i = 0; i < total; i++) {
        
        // Passo 1: Encontramos o Email?
        if (strcmp(users[i].email, email) == 0) {
            
            // Passo 2: A password coincide?
            if (strcmp(users[i].password, pass) == 0) {
                
                // Passo 3: A conta está ativa?
                if (users[i].ativo == ATIVO) {
                    return i; // SUCESSO! Retorna a posição do user
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

/**
 * @brief Gerencia o perfil do utilizador logado.
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 * @param idLogado Ponteiro para o ID do utilizador logado (pode ser alterado se eliminar conta).
 */
void gerirPerfil(Utilizador users[], int total, int *idLogado) {
    int subOpcao;
    int id = *idLogado;

    do {
        limparEcra();
        printf("\n--- GESTAO DE PERFIL ---\n");
        printf("1. Ver Meus Dados\n");
        printf("2. Editar Dados\n");
        printf("3. Eliminar Conta\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        
        subOpcao = lerInteiro();

        switch (subOpcao)
        {
            case 1:
                mostrarPerfil(users[id]);
                esperarEnter();
                break;

            case 2:
                editarPerfil(&users[id]);
                guardarUtilizadores(users, total);
                esperarEnter();
                break;

            case 3:
                if (eliminarConta(&users[id])) {
                    guardarUtilizadores(users, total);
                    id = -1; // Logout
                    subOpcao = 0; // Sair do submenu
                    printf("Sessao terminada apos eliminacao de conta.\n");
                }
                esperarEnter();
                break;

            case 0:
                // Não faz nada, só sai do loop
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();                   
        }

    } while (subOpcao != 0);
}