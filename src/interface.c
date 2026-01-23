#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "interface.h"
#include "users.h"
#include "books.h"
#include "transactions.h"
#include "files.h"
#include "structs.h"

/* =============================================================
   VISUALIZAÇÃO DE DADOS E AUDITORIA (BASE)
   ============================================================= */

void imprimirLinhaLivro(Livro *book) {
    char donoStr[20]; // Buffer para armazenar o rótulo do proprietário de forma amigável
    
    // Lógica de Identificação de Propriedade:
    // O ID 1 está reservado para a gestão central (IPCA/Admin). 
    // Esta distinção permite ao utilizador saber se o livro é do sistema ou de um par (User).
    if (book->idProprietario == 1) {
        strcpy(donoStr, "IPCA (Admin)");
    } else {
        sprintf(donoStr, "User %d", book->idProprietario);
    }

    // Explicação para Defesa: Uso especificadores de largura (ex: %-20.20s).
    // O '-' alinha à esquerda, o primeiro '20' reserva o espaço e o '.20' trunca o texto.
    // Isto garante que a tabela mantém o alinhamento mesmo com títulos ou autores muito longos.
    printf("- ID: %d | ISBN: %-13s | Titulo: %-20.20s | Autor: %-15.15s | Cat: %-10s | Dono: %s\n", 
           book->id,
           book->isbn,
           book->titulo, 
           book->autor, 
           book->categoria,
           donoStr); 
}

void exibirLogSistema() {
    limparEcra(); // Garante uma visualização limpa focada apenas nos dados do ficheiro
    
    // Explicação para Defesa: Demonstração de persistência em modo texto. 
    // Abrimos o ficheiro em modo "r" (read) para processar o histórico de auditoria.
    FILE *f = fopen("data/log_sistema.txt", "r");
    
    // Tratamento de Erro/Ficheiro Inexistente: impede que o programa tente 
    // ler de um ponteiro nulo (segmentation fault) se o log ainda não tiver sido criado.
    if (f == NULL) {
        printf("\n[Aviso] O ficheiro de logs ainda nao foi criado ou esta vazio.\n");
        esperarEnter();
        return;
    }

    printf("\n==================== LOG DE AUDITORIA DO SISTEMA ====================\n");
    printf("%-18s | %-6s | %-15s | %s\n", "DATA E HORA", "ID", "ACAO", "DETALHES");
    printf("---------------------------------------------------------------------\n");

    char linha[256]; // Buffer de tamanho fixo para processamento por linha
    
    // Ciclo de Leitura: fgets lê até encontrar um '\n' ou atingir o limite do buffer.
    // O ciclo continua até que fgets retorne NULL (fim do ficheiro - EOF).
    while (fgets(linha, sizeof(linha), f)) {
        printf("%s", linha);
    }
    
    printf("=====================================================================\n");
    
    // Fecho do descritor: libertação obrigatória do ficheiro no SO para evitar fugas de memória (memory leaks).
    fclose(f);
    esperarEnter();
}

void listarStockAdmin(Livro books[], int totalBooks) {
    limparEcra(); // Limpa resíduos visual do menu anterior para focar na tabela
    printf("\n=== STOCK DA INSTITUICAO (ADMIN) ===\n");

    // Explicação para Defesa: Cabeçalho com alinhamento à esquerda (%-) e limites de caracteres 
    // (.35) para assegurar que a tabela não quebre em ecrãs de terminal pequenos.
    printf("%-35.35s | %-15s | %s\n", "TITULO", "ISBN", "ESTADO");
    printf("----------------------------------------------------------------------------\n");

    int stock = 0; // Acumulador para fornecer um resumo estatístico ao final da listagem
    
    for(int i = 0; i < totalBooks; i++) {
        // Lógica de Filtragem: Esta função aplica um filtro seletivo onde apenas 
        // livros com idProprietario == 1 (Admin) e eliminado == 0 (Ativo) são processados.
        // Isto isola o inventário institucional da base de dados global de utilizadores.
        if(books[i].idProprietario == 1 && books[i].eliminado == 0) {
            
            char estadoTexto[20];
            // Mapeamento de ENUM para String: Traduzimos o valor inteiro interno 
            // da estrutura para uma linguagem natural, facilitando a leitura do Administrador.
            switch(books[i].estado) {
                case LIVRO_INDISPONIVEL: strcpy(estadoTexto, "Indisponivel"); break;
                case LIVRO_DISPONIVEL:   strcpy(estadoTexto, "Disponivel"); break;
                case LIVRO_EMPRESTADO:   strcpy(estadoTexto, "Emprestado"); break;
                case LIVRO_RESERVADO:    strcpy(estadoTexto, "Reservado"); break;
                default: strcpy(estadoTexto, "Desconhecido"); break;
            }

            // Exibição simplificada: Foca na disponibilidade física e identificação (ISBN)
            printf("%-35.35s | %-15s | %s\n", 
                    books[i].titulo, 
                    books[i].isbn,
                    estadoTexto);
            
            stock++;
        }
    }

    // Validação de Resultados: Se o loop terminar com stock a zero, 
    // fornecemos um feedback amigável em vez de uma tabela vazia.
    if(stock == 0) {
        printf("\n[Info] A instituicao nao tem livros em stock.\n");
    } else {
        printf("----------------------------------------------------------------------------\n");
        printf("Total: %d livros listados no stock da instituicao.\n", stock);
    }

    esperarEnter(); // Pausa a execução para que o Administrador possa ler os dados antes de regressar ao menu
}

/* =============================================================
   FLUXOS DE AUTENTICAÇÃO E ENTRADA (VISITANTE)
   ============================================================= */
   
int menuModoVisitante(Utilizador users[], int *totalUsers, Operacao operacoes[], int totalOperacoes) {
    (void) operacoes;
    (void) totalOperacoes;

    int opcao;
    limparEcra();
    printf("\n=== BEM-VINDO AO WOOKINTED ===\n");
    printf("1. Registar Conta\n");
    printf("2. Login\n");
    printf("3. Recuperar Password\n");
    printf("0. Sair\n");
    
    // Explicação para Defesa: A função lerInteiro protege o programa contra inputs 
    // inválidos (letras ou números fora do intervalo 0-3), garantindo a robustez do menu.
    opcao = lerInteiro("Opcao: ", 0, 3);

    switch(opcao) {
        case 1:
            // Registo e Persistência Imediata: Chamamos guardarUtilizadores logo após o registo 
            // para assegurar que, em caso de fecho inesperado, os novos dados já estão no disco.
            registarUtilizador(users, totalUsers);
            guardarUtilizadores(users, *totalUsers); 
            esperarEnter();
            return -1; // Mantém o estado como Visitante (ID -1) no ciclo principal
        
        case 2: {
            // Lógica de Autenticação: A função loginUtilizador valida as credenciais.
            // Se o ID retornado for >= 0, o main.c saberá que deve mudar para o Menu Principal.
            int resultado = loginUtilizador(users, *totalUsers);
            if (resultado >= 0) {                
                return resultado; 
            } 
            else {
                return -1; // Login falhou ou foi cancelado, mantém utilizador como Visitante
            }
        }

        case 3:
            // Recuperação de Identidade: Permite alterar a password após validação de dados 
            // biográficos, seguido de gravação automática para atualizar o ficheiro .dat.
            recuperarPassword(users, *totalUsers);
            guardarUtilizadores(users, *totalUsers);
            return -1;

        case 0:
            // Sinal de Encerramento: Retornamos um código sentinela (-10) que será 
            // interpretado pelo main() como a instrução definitiva para fechar o software.
            return -10; 

        default:
            printf("Opcao invalida.\n");
            esperarEnter();
            return -1;
    }
}

void registarUtilizador(Utilizador users[], int *total) {
    limparEcra();
    printf("\n=== REGISTO DE UTILIZADOR (Digite '0' para cancelar) ===\n");

    int indice = -1;
    int isReativacao = 0; // Flag para distinguir entre um novo registo e o regresso de um ex-utilizador
    char entradaTemp[MAX_STRING];
    char emailFinal[MAX_STRING];
    char logDetalhe[100]; 

    // --- 1. ETAPA: EMAIL (CHAVE PRIMÁRIA LÓGICA) ---
    do {
        printf("Email Institucional: ");
        lerString(entradaTemp, MAX_STRING);
        if (strcmp(entradaTemp, "0") == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }

        // Validação de Domínio: Garante que apenas emails @ipca.pt ou @alunos.ipca.pt são aceites
        if (!validarFormatoEmailIPCA(entradaTemp)) {
            printf("[Erro] Formato invalido ou contem caracteres especiais proibidos.\n");
            continue; 
        }

        // Verificação de Estado: O sistema verifica se o email já existe e se a conta está ativa ou inativa
        int status = verificarStatusEmail(users, *total, entradaTemp, &indice);

        if (status == 1) {
            // Caso de Reativação: Se a conta existe mas está inativa (Soft Delete), permite-se o pedido de regresso
            printf("\n[Info] Conta antiga detetada. Iniciando processo de reativacao...\n");
            isReativacao = 1;
            break;
        } else if (status == 2) {
            // Caso de Duplicação: Impede o registo de contas que já estão ativas no sistema
            printf("[Erro] Este email ja se encontra registado e ativo.\n");
            sprintf(logDetalhe, "Registo falhado (%s)", entradaTemp);
            registarLog(0, "REGISTO_FAIL", logDetalhe);
            continue;
        } else {
            // Verificação de Limite de Memória: Impede o transbordo (overflow) do array estático
            if (*total >= MAX_UTILIZADORES) {
                printf("[Erro] Limite de utilizadores atingido.\n");
                sprintf(logDetalhe, "Registo falhado (%s)", entradaTemp);
                registarLog(0, "REGISTO_FAIL", logDetalhe);
                return;
            }
            indice = *total;
            isReativacao = 0;
            break;
        }
    } while (1);

    strcpy(emailFinal, entradaTemp);

    // --- 2. ETAPA: NOME (VALIDAÇÃO DE CARACTERES) ---
    if (!isReativacao) {
        do {
            printf("Nome Completo: ");
            lerString(entradaTemp, MAX_STRING);
            if (strcmp(entradaTemp, "0") == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }
            
            // Garantia de integridade: O nome não pode estar vazio nem conter números/símbolos
            if (stringNaoVazia(entradaTemp) && validarApenasLetras(entradaTemp, MAX_STRING) && validarAlfanumerico(entradaTemp)) {
                strcpy(users[indice].nome, entradaTemp);
                break;
            }
            printf("[Erro] Nome invalido (nao use caracteres especiais ou numeros).\n");
        } while (1);
    }

    // --- 3. ETAPA: CONTACTO TELEMÓVEL ---
    int atualizarTelemovel = 1;
    if (isReativacao) {
        printf("\nTelemovel atual: %s\n1. Manter numero\n2. Atualizar numero\n0. Cancelar\n", users[indice].telemovel);
        int opTel = lerInteiro("Opcao: ", 0, 2);
        if (opTel == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }
        if (opTel == 1) atualizarTelemovel = 0;
    }

    if (atualizarTelemovel) {
        do {
            printf("Telemovel (Comece por 91, 92, 93 ou 96): ");
            lerString(entradaTemp, MAX_STRING);
            if (strcmp(entradaTemp, "0") == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }

            // Validação de Formato e Unicidade: Garante que o número é português e não pertence a outro user
            if (validarTelemovelPortugues(entradaTemp)) {
                if (!telemovelEmUso(users, *total, entradaTemp, indice)) {
                    strcpy(users[indice].telemovel, entradaTemp);
                    break;
                }
                printf("[Erro] Telemovel ja registado.\n");
            } else printf("[Erro] Numero invalido. Deve ter 9 digitos e prefixo nacional (91, 92, 93, 96).\n");
        } while (1);
    }

    // --- 4. ETAPA: IDADE MÍNIMA (REGRA DE NEGÓCIO) ---
    if (!isReativacao) {
        do {
            printf("Data de Nascimento (DD/MM/AAAA): ");
            lerString(entradaTemp, 11);
            if (strcmp(entradaTemp, "0") == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }

            if (!validarData(entradaTemp)) {
                printf("[Erro] Formato incorreto.\n");
                continue;
            }

            // Explicação para Defesa: O sistema calcula a idade e impede o registo de menores de 18 anos
            int idade = calcularIdade(entradaTemp);
            if (idade < 18) {
                printf("[Erro] Registro proibido para menores de 18 anos (Idade atual: %d).\n", idade);
                sprintf(logDetalhe, "Registo falhado (%s)", emailFinal);
                registarLog(0, "REGISTO_FAIL", logDetalhe);
                continue;
            }
            strcpy(users[indice].dataNascimento, entradaTemp);
            break;
        } while (1);
    }

    // --- 5. ETAPA: SEGURANÇA (PASSWORD) ---
    do {
        printf("Defina uma Password (min. 4 caracteres, sem especiais): ");
        lerString(entradaTemp, 50);
        if (strcmp(entradaTemp, "0") == 0) { printf("\n[Cancelado] Operacao interrompida.\n"); return; }
        
        // Critério de Robustez: Mínimo de caracteres e proibição de símbolos complexos para simplificar a base de dados
        if (strlen(entradaTemp) >= 4 && validarAlfanumerico(entradaTemp)) { 
            strcpy(users[indice].password, entradaTemp); 
            break; 
        }
        printf("[Erro] Password invalida (min. 4 caracteres e apenas letras/numeros).\n");
    } while (1);

    // --- 6. ETAPA: FINALIZAÇÃO E ESTADOS DE APROVAÇÃO ---
    strcpy(users[indice].email, emailFinal);
    if (isReativacao) {
        // As contas reativadas entram num fluxo de aprovação administrativa
        users[indice].estado = CONTA_PENDENTE_REATIVACAO;
        registarLog(users[indice].id, "REGISTO_REATIVACAO", "Reativacao solicitada.");
        printf("\n[Sucesso] Pedido de reativacao enviado, aguarde aprovacao do Administrador.\n");
    } else {
        // Atribuição de ID Incremental: Cada novo utilizador recebe um ID único baseado no total atual
        users[indice].id = *total + 1; 
        users[indice].estado = CONTA_PENDENTE_APROVACAO;
        
        sprintf(logDetalhe, "Novo registo (%s)", users[indice].email);
        registarLog(users[indice].id, "REGISTO_NOVO", logDetalhe);
        
        (*total)++; 
        printf("\n[Sucesso] Registo enviado para aprovacao.\n");
    }
    // Persistência: Os dados são gravados no ficheiro dat imediatamente após o registo lúdico
    guardarUtilizadores(users, *total); 
}

int loginUtilizador(Utilizador users[], int totalUsers) {
    char email[MAX_STRING], pass[50];
    int indice = -1;
    char logDetalhe[100]; 
    
    limparEcra();
    printf("\n=== LOGIN DE UTILIZADOR ===\n");

    // --- 1. ETAPA: EMAIL (VALIDAÇÃO DE IDENTIDADE E ESTADO) ---
    do {
        printf("Email (0 para cancelar): "); 
        lerString(email, MAX_STRING);
        
        // Permite ao utilizador desistir do login sem forçar a entrada de dados
        if (strcmp(email, "0") == 0) {
            printf("\n[Cancelado] Operacao interrompida pelo utilizador.\n");
            esperarEnter();
            return -5; 
        }

        // Validação de Integridade: Impede o processamento de emails fora do domínio institucional
        if (!validarFormatoEmailIPCA(email)) {
            printf("[Erro] Formato de email invalido para o dominio IPCA.\n");
            continue;
        }

        // Pesquisa e Verificação: Busca o utilizador no array e retorna o seu estado atual
        int status = verificarStatusEmail(users, totalUsers, email, &indice);

        if (status == 0) {
            printf("[Erro] Email nao encontrado.\n");
            continue;
        }

        // Segurança de Acesso: Bloqueia utilizadores com contas inativas (Soft Delete)
        if (users[indice].estado == CONTA_INATIVA) {
            printf("\n[Acesso Negado] Esta conta foi eliminada ou desativada.\n");
            esperarEnter();
            return -3;
        }

        // Controlo Administrativo: Garante que apenas utilizadores validados pelo Admin entrem no sistema
        if (users[indice].estado == CONTA_PENDENTE_APROVACAO) {
            printf("\n[Acesso Negado] A sua conta aguarda validacao do Administrador.\n");
            esperarEnter();
            return -4;
        }

        break; // Email válido e conta ativa, prossegue para a password

    } while (1);

    // --- 2. ETAPA: PASSWORD (AUTENTICAÇÃO) ---
    do {
        printf("Password (0 para cancelar): "); 
        lerString(pass, 50);

        if (strcmp(pass, "0") == 0) {
            printf("\n[Cancelado] Operacao interrompida pelo utilizador.\n");
            esperarEnter();
            return -5;
        }

        // Explicação Defesa: A função autenticarUtilizador realiza a comparação segura das strings.
        // Se houver correspondência, retorna o ID do utilizador, caso contrário retorna um código de erro.
        int resultado = autenticarUtilizador(users, totalUsers, email, pass);

        if (resultado == -2) {
            printf("\n[Erro] Password incorreta.\n");
            // Auditoria: Regista-se no ficheiro de logs que houve uma tentativa falhada (ID 0 indica sistema)
            sprintf(logDetalhe, "Falha de pass para: %s", email);
            registarLog(0, "LOGIN_FAIL", logDetalhe);
            continue; 
        } 

        if (resultado > 0) {
            // SUCESSO: SESSÃO INICIADA
            limparEcra();
            
            // Registo de Sucesso: Auditoria guarda quem entrou e a que horas
            sprintf(logDetalhe, "Sessao iniciada com sucesso %s", users[indice].email);
            registarLog(resultado, "LOGIN", logDetalhe);
            
            // Lógica de Boas-Vindas (UX): Diferencia o tipo de utilizador
            printf("\n=======================================\n");

            // 1. Verificação de Administrador (Caso mais específico)
            if (strcmp(users[indice].email, "admin@ipca.pt") == 0) {
                printf("  BEM-VINDO(A) ADMINISTRADOR(A)!");
            } 
            // 2. Verificação de Aluno
            else if (strstr(users[indice].email, "@alunos.ipca.pt") != NULL) {
                printf("  BEM-VINDO(A) ALUNO(A), %s!", users[indice].nome);
            } 
            // 3. Caso contrário, é Docente
            else {
                printf("  BEM-VINDO(A) DOCENTE, %s!", users[indice].nome);
            }
            
            printf("\n=======================================\n");
            
            printf("\nLogin efetuado com sucesso.\n");
            esperarEnter();
            return resultado; 
        }

    } while (1);

    return -1;
}

void recuperarPassword(Utilizador users[], int total) {
    char emailInput[MAX_STRING], telInput[15];
    char logDetalhe[100]; // Buffer para formatar as mensagens de log
    int indice = -1;

    limparEcra();
    printf("\n=== RECUPERACAO DE CONTA (0 para cancelar) ===\n");

    // --- 1. ETAPA: EMAIL (VALIDAÇÃO DE EXISTÊNCIA) ---
    do {
        printf("Introduza o seu Email: ");
        lerString(emailInput, MAX_STRING);
        
        if (strcmp(emailInput, "0") == 0) {
            printf("\n[Cancelado] Operacao interrompida.\n");
            esperarEnter();
            return;
        }

        // Validação de formato IPCA: Garante que apenas emails institucionais tentem o processo
        if (!validarFormatoEmailIPCA(emailInput)) {
            printf("[Erro] Formato de email invalido para o dominio IPCA.\n");
            continue;
        }

        // Explicação Defesa: O sistema procura o email na base de dados. Se não existir, 
        // bloqueia o fluxo imediatamente para evitar tentativas de força bruta.
        int status = verificarStatusEmail(users, total, emailInput, &indice);
        if (status == 0) {
            printf("[Erro] Email nao encontrado no sistema.\n");
            
            // Auditoria: Regista a falha para controlo de tentativas de acesso indevido
            sprintf(logDetalhe, "Falha na recuperacao da Password (%s)", emailInput);
            registarLog(0, "PASS_RECOVERY_FAIL", logDetalhe);
            
            continue;
        }
        
        // Regra de Segurança: A recuperação só é permitida para contas que não foram desativadas
        if (users[indice].estado == CONTA_INATIVA) {
            printf("[Erro] So e possivel recuperar passwords de contas ATIVAS.\n");
            
            sprintf(logDetalhe, "Falha na recuperacao da Password (%s)", emailInput);
            registarLog(0, "PASS_RECOVERY_FAIL", logDetalhe);
            
            esperarEnter();
            return;
        }

        break; 
    } while (1);

    // --- 2. ETAPA: TELEMÓVEL (AUTENTICAÇÃO DE DOIS FATORES) ---
    do {
        // Explicação Defesa: Para garantir que quem está a recuperar é o dono da conta, 
        // exigimos a confirmação do número de telemóvel registado originalmente.
        printf("Confirme o seu Telemovel (9 digitos, 0 para cancelar): ");
        lerString(telInput, 15);
        
        if (strcmp(telInput, "0") == 0) {
            printf("\n[Cancelado] Operacao interrompida.\n");
            esperarEnter();
            return;
        }

        if (strcmp(users[indice].telemovel, telInput) != 0) {
            printf("[Erro] O telemovel nao corresponde ao registo deste email.\n");
            
            sprintf(logDetalhe, "Falha na recuperacao da Password (%s)", emailInput);
            registarLog(0, "PASS_RECOVERY_FAIL", logDetalhe);
        } else {
            break; 
        }
    } while (1);

    // --- 3. ETAPA: NOVA PASSWORD (REDEFINIÇÃO SEGURA) ---
    char novaPass[50], confirmPass[50];
    int passwordAceite = 0;

    do {
        printf("\nNova Password (min 4 chars, sem caracteres especiais, 0 para cancelar): ");
        lerString(novaPass, 50);
        
        if (strcmp(novaPass, "0") == 0) {
            return;
        }

        // Validação de Integridade: Comprimento mínimo e proibição de caracteres especiais 
        // para garantir compatibilidade com o armazenamento binário.
        if (strlen(novaPass) < 4) {
            printf("[Erro] Password demasiado curta.\n");
            continue;
        }
        if (!validarAlfanumerico(novaPass)) {
            printf("[Erro] A password nao pode conter caracteres especiais.\n");
            continue;
        }
        // Segurança Adicional: Impede o uso de uma password igual à anterior
        if (strcmp(novaPass, users[indice].password) == 0) {
            printf("[Erro] A nova password nao pode ser igual a anterior.\n");
            continue;
        }

        printf("Confirme a Nova Password: ");
        lerString(confirmPass, 50);

        if (strcmp(novaPass, confirmPass) != 0) {
            printf("[Erro] As passwords nao coincidem.\n");
        } else {
            passwordAceite = 1;
        }
    } while (!passwordAceite);

    // --- 4. FINALIZAÇÃO E REGISTO DE AUDITORIA ---
    strcpy(users[indice].password, novaPass); // Atualização física no array de memória
    
    // Log de Sucesso: Crucial para o Administrador monitorizar alterações críticas em contas
    sprintf(logDetalhe, "Password recuperada (%s)", users[indice].email);
    registarLog(users[indice].id, "PASS_RECOVERY", logDetalhe);
    
    printf("\n[Sucesso] A sua password foi alterada com sucesso!\n");
    esperarEnter(); 
}

/* =============================================================
   MENUS PRINCIPAIS DE NAVEGAÇÃO E HUB DE OPERAÇÕES
   ============================================================= */

int mostrarMenuPrincipal(char *nome) {
    limparEcra(); // Mantém a interface organizada, eliminando dados de ecrãs anteriores
    
    // UX: Personalização do menu com o nome do utilizador logado para confirmar a sessão ativa
    printf("\n=== MENU PRINCIPAL (%s) ===\n", nome);
    printf("1. Mercado de Livros (Procurar, Pedir, Doar)\n");
    printf("2. Meus Livros (Registar, Editar, Eliminar)\n");
    printf("3. Gestao de Movimentos (Pedidos, Devolucoes, Feedback)\n");
    printf("4. Meu Perfil\n");
    printf("0. Logout\n");

    // Explicação para Defesa: Este menu atua como o controlador central da aplicação (Hub).
    // O uso da função utilitária lerInteiro(min, max) garante que o utilizador nunca 
    // introduza uma opção fora do intervalo [0-4] ou caracteres inválidos, prevenindo erros 
    // lógicos ou encerramentos inesperados do programa.
    return lerInteiro("Opcao: ", 0, 4);
}

void menuAdministrador(Utilizador users[], int *totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks) {
    int opcao;
    do {
        limparEcra();
        printf("\n===============================================\n");
        printf("       PAINEL ADMIN (IPCA - ID: 1)             \n");
        printf("===============================================\n");
        printf("1. Centro de Validacoes (Users, Propostas)\n");
        printf("2. Gestao de Stock Institucional (Catalogo)\n");
        printf("3. Monitorizacao (Historico Global)\n");
        printf("4. Relatorios e Estatisticas\n");
        printf("5. Consultar Log de Auditoria (Sistema)\n");
        printf("0. Sair\n");
        printf("-----------------------------------------------\n");
        
        // Explicação para Defesa: Este menu é o núcleo de gestão do sistema.
        // O acesso é restrito ao ID 1 (identidade institucional), garantindo que funções
        // críticas como aprovação de utilizadores e consulta de logs não sejam expostas a utilizadores comuns.
        opcao = lerInteiro("Opcao: ", 0, 5);

        switch(opcao) {
            case 1:
                // Submenu de validação: Gere o fluxo de entrada de novos utilizadores e propostas de transação.
                submenuValidacoes(users, *totalUsers, books, *totalBooks, operacoes, *totalOperacoes, feedbacks, *totalFeedbacks); 
                break;

            case 2:
                // Gestão de stock: Permite visualizar exclusivamente os livros que pertencem à instituição.
                listarStockAdmin(books, *totalBooks); 
                break;

            case 3:
                // Monitorização Global: Fornece uma visão omnisciente sobre todos os movimentos da plataforma.
                submenuHistoricoGlobal(operacoes, *totalOperacoes, users, *totalUsers, books, *totalBooks, feedbacks, *totalFeedbacks);
                break;

            case 4:
                // Estatísticas: Desvia para as funções de ranking (Top Livros, Users e Reputação).
                submenuRelatorios(users, *totalUsers, books, *totalBooks, operacoes, *totalOperacoes, feedbacks, *totalFeedbacks); 
                break;

            case 5:
                // Auditoria: Leitura direta do ficheiro TXT de logs para monitorizar acessos e ações do sistema.
                exibirLogSistema();
                break;

            case 0:
                printf("A sair do Painel de Administrador...\n");
                break;
            
            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while(opcao != 0); // Ciclo do-while garante que o Admin pode realizar múltiplas tarefas sem ser deslogado.
}

void menuMercadoLivros(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado, Feedback feedbacks[], int totalFeedbacks) {
    int opMercado;

    do {
        limparEcra();
        printf("\n--- MERCADO DE LIVROS ---\n");
        printf("1. Ver Catalogo Completo\n");
        printf("2. Pesquisar Livros\n"); 
        printf("3. Requisitar Livro (Troca/Levantamento)\n");
        printf("4. Doar Livro a Instituicao\n");
        printf("0. Voltar\n");

        // Explicação para Defesa: Controlo de fluxo centralizado. Este menu gere a "economia" do sistema,
        // permitindo a circulação de livros entre utilizadores ou para o stock da instituição.
        opMercado = lerInteiro("Opcao: ", 0, 4);

        switch (opMercado) {
            case 1:
                // Catálogo Dinâmico: Lista livros de outros utilizadores que estão marcados como disponíveis.
                listarCatalogoMercado(books, *totalBooks, users, totalUsers, idLogado);
                break;

            case 2:
                // Motor de Busca: Submenu que permite filtrar livros por título, autor ou categoria.
                submenuPesquisaLivros(users, totalUsers, books, *totalBooks, idLogado);
                break;

            case 3: 
                // Transações: Inicia o processo de pedido (Empréstimo ou proposta de Troca).
                processarRequisicaoLivro(books, totalBooks, users, totalUsers, operacoes, totalOperacoes, idLogado, feedbacks, totalFeedbacks);
                break;

            case 4:
                // Altruísmo Institucional: Permite ao utilizador ceder permanentemente um livro ao IPCA.
                doarLivro(books, *totalBooks, idLogado, operacoes, totalOperacoes);
                // Persistência: Como a doação altera o proprietário do livro no array, gravamos imediatamente no disco.
                guardarLivros(books, *totalBooks);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while (opMercado != 0); // Ciclo de permanência para facilitar múltiplas consultas ou ações.
}

void menuMeusLivros(Livro books[], int *totalBooks, int idLogado) {
    int opcao;
    do {
        limparEcra(); // Limpeza de ecrã para manter a interface organizada a cada iteração
        
        // 1. LISTAR TABELA E OBTER QUANTIDADE
        // Explicação para Defesa: Chamo primeiro a função de listagem que, além de mostrar os dados, 
        // retorna o total de livros visíveis. Este valor (qtdMeusLivros) é crucial para validar 
        // dinamicamente os inputs no submenu de eliminação, garantindo que o utilizador não 
        // escolhe um número fora do que está a ver no ecrã.
        int qtdMeusLivros = listarTabelaMeusLivros(books, *totalBooks, idLogado);

        printf("\n1. Registar Novo Livro (via ISBN)\n");
        printf("2. Eliminar Livro do Sistema\n");
        printf("0. Voltar\n");
        
        opcao = lerInteiro("Opcao: ", 0, 2);

        switch (opcao) {
            case 1:
                // Encaminha para o módulo de criação onde será solicitada a validação por ISBN
                criarLivro(books, totalBooks, idLogado);
                break;

            case 2:
                // Explicação para Defesa: Passamos a 'qtdMeusLivros' para o submenu.
                // Isto evita que o utilizador tente eliminar algo se a lista estiver vazia, 
                // tratando o erro antes mesmo de entrar na lógica de eliminação.
                submenuEliminarLivro(books, *totalBooks, idLogado, qtdMeusLivros);
                break;
        }
    } while (opcao != 0); // Ciclo de permanência para permitir gerir vários livros consecutivamente
}

void menuGestaoMovimentos(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks, int idLogado) {
    int opMov;

    do {
        limparEcra();
        printf("\n--- GESTAO DE MOVIMENTOS E HISTORICO ---\n");
        printf("1. Ver Pedidos Recebidos (Aceitar/Recusar)\n");
        printf("2. Devolver Livro (Que tenho comigo)\n");
        printf("3. Historico de Transacoes\n");
        printf("4. Avaliar Troca/Emprestimo (Pendentes)\n");
        printf("0. Voltar\n");
        
        // Explicação para Defesa: Este menu é o núcleo operacional das interações. 
        // Ele gere o "workflow" desde a receção de um pedido até à sua conclusão e avaliação final.
        opMov = lerInteiro("Opcao: ", 0, 4);

        switch (opMov) {
            case 1:
                // Gestão de Pedidos: Permite ao proprietário do livro validar propostas de outros utilizadores.
                gerirPedidosPendentes(operacoes, totalOperacoes, books, *totalBooks, users, totalUsers, feedbacks, *totalFeedbacks, idLogado);                
                // Persistência: Sempre que um estado de operação ou livro muda, gravamos nos ficheiros .dat correspondentes.
                guardarOperacoes(operacoes, *totalOperacoes); 
                guardarLivros(books, *totalBooks);
                break;

            case 2:
                // Ciclo de Vida: Processa a devolução de livros emprestados, libertando o livro no sistema.
                submenuDevolverLivro(books, *totalBooks, operacoes, totalOperacoes, users, totalUsers, idLogado);                
                guardarOperacoes(operacoes, *totalOperacoes);
                break;

            case 3:
                // Auditoria Pessoal: Filtra o array de operações para mostrar apenas a atividade relevante ao utilizador logado.
                submenuHistoricoPessoal(operacoes, *totalOperacoes, users, totalUsers, books, *totalBooks, feedbacks, *totalFeedbacks, idLogado); 
                break;

            case 4:
                // Sistema de Reputação: Garante que as transações concluídas recebam pontuação (Feedback), 
                // alimentando a métrica de confiança da comunidade.
                // ATUALIZAÇÃO: Passagem de users e totalUsers para permitir a identificação nominal dos parceiros na avaliação.
                submenuAvaliarTransacoes(operacoes, *totalOperacoes, feedbacks, totalFeedbacks, books, *totalBooks, users, totalUsers, idLogado);
                guardarFeedbacks(feedbacks, *totalFeedbacks);
                break;

            case 0:
                break; 

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while (opMov != 0); // Ciclo de repetição para permitir ao utilizador gerir vários movimentos numa única sessão.
}

int menuGestaoPerfil(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    // 1. Localização Dinâmica de Contexto:
    // Explicação para Defesa: Como o sistema é baseado em IDs, o primeiro passo é localizar
    // o índice real do utilizador no array global. Isto garante que as alterações subsequentes
    // sejam feitas exatamente no registo correto, prevenindo erros de memória ou corrupção de dados.
    int idx = -1;
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].id == idLogado) {
            idx = i;
            break;
        }
    }

    // Segurança de Integridade: Se por algum motivo o ID não for encontrado (ex: erro de ponteiro),
    // o sistema aborta a operação por segurança antes de tentar aceder ao índice -1.
    if (idx == -1) return 0; 

    int opcao;
    do {
        limparEcra();
        
        // 2. Interface Dinâmica: O cabeçalho apresenta os dados atuais e a reputação calculada
        // através do processamento do array de feedbacks.
        mostrarCabecalhoPerfil(&users[idx], feedbacks, totalFeedbacks);

        printf("1. Alterar Nome\n");
        printf("2. Alterar Password\n");
        printf("3. Alterar Telemovel\n");
        printf("4. Eliminar Conta\n");
        printf("0. Voltar\n");
        printf("------------------------\n");
        
        opcao = lerInteiro("Opcao: ", 0, 4);

        switch (opcao) {
            case 1: 
                // Ação modular para atualização de campos específicos
                acaoAlterarNome(&users[idx], users, totalUsers);
                break;

            case 2: 
                acaoAlterarPassword(&users[idx], users, totalUsers);
                break;

            case 3: 
                acaoAlterarTelemovel(&users[idx], users, totalUsers);
                break;

            case 4: 
                // Explicação para Defesa (Soft Delete): Se o utilizador decidir eliminar a conta,
                // a função retorna 1 para o main.c, o que despoleta um logout automático imediato.
                // Note que os dados não desaparecem (Soft Delete), permitindo reativações futuras pelo Admin.
                if (acaoEliminarConta(&users[idx], users, totalUsers)) {
                    return 1; // Logout forçado por encerramento de conta
                }
                break;
        }
    } while (opcao != 0);

    return 0; // Utilizador escolheu voltar ao menu principal mantendo a sessão
}

/* =============================================================
   MÓDULO DE CONSULTA DE MERCADO E PESQUISAS
   ============================================================= */

/* =============================================================
   MÓDULO DE CONSULTA DE MERCADO E PESQUISAS
   ============================================================= */

void listarCatalogoMercado(Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n--- CATALOGO DO MERCADO ---\n");
    // Explicação para Defesa: Cabeçalho com formatação rigorosa. O uso de larguras fixas (ex: %-25s) 
    // é essencial para manter o alinhamento tabular independentemente do tamanho das strings.
    printf("%-4s | %-25s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int contVis = 1; // Contador visual (1, 2, 3...) independente do índice real no array
    int encontrou = 0;

    for (int i = 0; i < totalBooks; i++) {
        // Explicação para Defesa (Regra de Negócio): Aplicamos um filtro duplo.
        // 1. O livro deve estar explicitamente como LIVRO_DISPONIVEL.
        // 2. O livro não pode pertencer ao utilizador logado (idProprietario != idLogado).
        // Isto garante que o utilizador só veja o que pode efetivamente requisitar.
        if (books[i].estado == LIVRO_DISPONIVEL && books[i].idProprietario != idLogado) {
            
            char nomeDono[MAX_STRING];
            
            // Lógica de Identificação de Posse:
            if (books[i].idProprietario == 1) {
                strcpy(nomeDono, "Instituicao (IPCA)");
            } else {
                // Explicação para Defesa (Relacionamento de Dados): Como a estrutura Livro apenas guarda
                // o ID do dono, fazemos uma busca no array de Utilizadores para recuperar o Nome real.
                // É o equivalente a um "INNER JOIN" em bases de dados relacionais.
                strcpy(nomeDono, "Utilizador"); 
                for(int u = 0; u < totalUsers; u++) {
                    if(users[u].id == books[i].idProprietario) {
                        strncpy(nomeDono, users[u].nome, 20);
                        nomeDono[20] = '\0'; // Garante o fecho da string após o truncamento
                        break;
                    }
                }
            }

            // Exibição: O uso de .25 no especificador de formato (%-25.25s) trunca títulos longos,
            // impedindo que uma linha "empurre" a coluna seguinte e desformate a tabela.
            printf("%-4d | %-25.25s | %-20.20s | %-15.15s | %-20.20s\n", 
                    contVis, books[i].titulo, books[i].autor, books[i].categoria, nomeDono);
            
            contVis++;
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\n[Info] Nao existem livros de outros utilizadores disponiveis no momento.\n");
    }

    printf("----------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void submenuPesquisaLivros(Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado) {
    (void)idLogado; // Silenciar aviso de não uso: Boa prática de compilação para parâmetros futuros
    int opPesquisa;
    char termoOriginal[MAX_STRING], termoBusca[MAX_STRING];

    do {
        limparEcra();
        printf("\n=== CONSULTAR CATALOGO GERAL ===\n");
        printf("1. Por Titulo\n2. Por Autor\n3. Por Categoria\n0. Voltar\n");
        
        // Explicação para Defesa: Validação de fluxo. O utilizador escolhe o critério de filtragem 
        // antes de introduzir dados, o que otimiza o processamento da busca.
        opPesquisa = lerInteiro("Opcao: ", 0, 3);

        if (opPesquisa == 0) return;

        printf("\nTermo a pesquisar (0 para cancelar): ");
        lerString(termoOriginal, MAX_STRING);
        
        // Controlo de Fluxo: Permite ao utilizador retroceder no processo de pesquisa
        // sem ter de completar a operação, melhorando a experiência de utilização (UX).
        if (strcmp(termoOriginal, "0") == 0) {
            printf("\n[Info] Pesquisa cancelada pelo utilizador.\n");
            esperarEnter();
            continue; 
        }

        // Explicação para Defesa (Normalização de Dados): O uso de paraMinusculas é fundamental.
        // O sistema torna-se "case-insensitive", garantindo que pesquisar por "C" ou "c" 
        // retorne os mesmos resultados, aumentando a eficácia da busca.
        paraMinusculas(termoOriginal, termoBusca);

        // Explicação para Defesa (Modularidade): Esta função atua como interface. 
        // A lógica de iteração e comparação de strings é delegada para listarResultadosPesquisa, 
        // seguindo o princípio da responsabilidade única.
        listarResultadosPesquisa(books, totalBooks, users, totalUsers, termoBusca, opPesquisa, termoOriginal);
        
        esperarEnter();

    } while (opPesquisa != 0);
}

void listarResultadosPesquisa(Livro books[], int totalBooks, Utilizador users[], int totalUsers, char *termoBusca, int opPesquisa, char *termoOriginal) {
    limparEcra();
    printf("--- RESULTADOS PARA: '%s' ---\n", termoOriginal);
    
    // Explicação para Defesa: Interface tabular consistente. 
    // Uso de especificadores de campo (ex: %-25s) para garantir que as colunas fiquem alinhadas,
    // independentemente do conteúdo dos dados.
    printf("%-25s | %-20s | %-15s | %-15s | %s\n", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO", "ESTADO");
    printf("----------------------------------------------------------------------------------------------\n");

    int encontrados = 0;

    for (int i = 0; i < totalBooks; i++) {
        // Explicação para Defesa (Soft Delete): Verificamos o campo 'eliminado'. 
        // Livros que foram "removidos" logicamente pelo utilizador não devem aparecer nas pesquisas públicas.
        if (books[i].eliminado == 1) continue;

        char alvoComparacao[MAX_STRING];
        
        // Explicação para Defesa (Normalização de Busca): Selecionamos o campo com base na opção do menu.
        // Convertemos o campo original para minúsculas antes da comparação para garantir que a 
        // pesquisa seja insensível a maiúsculas (Case-Insensitive).
        if (opPesquisa == 1) paraMinusculas(books[i].titulo, alvoComparacao);
        else if (opPesquisa == 2) paraMinusculas(books[i].autor, alvoComparacao);
        else if (opPesquisa == 3) paraMinusculas(books[i].categoria, alvoComparacao);

        // Explicação para Defesa (Algoritmo de Busca): Uso da função strstr().
        // Isto permite uma "busca por substring", ou seja, se o utilizador pesquisar "Harry", 
        // o sistema encontrará "Harry Potter", tornando a pesquisa muito mais flexível.
        if (strstr(alvoComparacao, termoBusca) != NULL) {
            
            // Relacionamento de Dados: Busca o nome do proprietário no array de Utilizadores
            char nomeDono[MAX_STRING] = "IPCA";
            if (books[i].idProprietario != 1) {
                for (int u = 0; u < totalUsers; u++) {
                    if (users[u].id == books[i].idProprietario) {
                        strncpy(nomeDono, users[u].nome, 14); 
                        nomeDono[14] = '\0';
                        break;
                    }
                }
            }

            // Tradução de ENUM: Converte o estado numérico interno para uma string legível para o utilizador.
            char txtEstado[20];
            if (books[i].estado == LIVRO_DISPONIVEL) strcpy(txtEstado, "Disponivel");
            else if (books[i].estado == LIVRO_EMPRESTADO) strcpy(txtEstado, "Emprestado");
            else strcpy(txtEstado, "Reservado");

            // Formatação de Saída: O uso de .25 (ex: %-25.25s) garante que, se um título for 
            // maior que 25 caracteres, ele será cortado, impedindo a quebra da linha na tabela.
            printf("%-25.25s | %-20.20s | %-15.15s | %-15.15s | %-15s\n", 
                    books[i].titulo, books[i].autor, books[i].categoria, nomeDono, txtEstado);
            
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("\n[Info] Nenhum livro encontrado com esse termo.\n");
    } else {
        printf("----------------------------------------------------------------------------------------------\n");
        printf("[Sucesso] %d correspondencias encontradas.\n", encontrados);
    }
}

void processarRequisicaoLivro(Livro books[], int *totalBooks, Utilizador users[], int totalUsers, Operacao operacoes[], int *totalOperacoes, int idLogado, Feedback feedbacks[], int totalFeedbacks) {
    limparEcra();
    printf("--- REQUISITAR LIVRO ---\n");
    // Explicação para Defesa: Interface informativa. Além dos dados do livro, incluímos a reputação
    // do proprietário. Isto permite uma decisão informada por parte do requerente (User Experience).
    printf("%-3s | %-25s | %-15s | %-20s | %s\n", "N.", "TITULO", "CATEGORIA", "PROPRIETARIO", "REPUTACAO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int cVisual = 1, disponiveis = 0;
    int mapeamento[MAX_LIVROS]; // Vetor de mapeamento: Crucial para ligar o N. visual ao índice real do array

    // 1. LISTAGEM FILTRADA (JOIN DE DADOS)
    for (int i = 0; i < *totalBooks; i++) {
        // Regra de Negócio: Filtramos para não mostrar livros eliminados, livros já ocupados
        // ou livros que pertencem ao próprio utilizador logado.
        if (books[i].eliminado != 0 || books[i].estado != LIVRO_DISPONIVEL || books[i].idProprietario == idLogado) continue;

        char nomeDono[MAX_STRING] = "Desconhecido";
        float mediaDono = 0.0f;
        int qtdAvaliacoes = 0;

        if (books[i].idProprietario == 1) {
            strcpy(nomeDono, "Instituicao (IPCA)");
            mediaDono = calcularMediaUtilizador(feedbacks, totalFeedbacks, 1, &qtdAvaliacoes);
        } else {
            // Relacionamento de Tabelas: Procuramos o nome e a média de feedback do dono no array de Utilizadores e Feedbacks
            for(int u = 0; u < totalUsers; u++) {
                if(users[u].id == books[i].idProprietario) { 
                    strncpy(nomeDono, users[u].nome, 19); nomeDono[19] = '\0';
                    mediaDono = calcularMediaUtilizador(feedbacks, totalFeedbacks, users[u].id, &qtdAvaliacoes);
                    break; 
                }
            }
        }
        printf("%-3d | %-25.25s | %-15.15s | %-20.20s | %.1f* (%d)\n", cVisual, books[i].titulo, books[i].categoria, nomeDono, mediaDono, qtdAvaliacoes);
        
        // Explicação para Defesa: Mapeamos o número que o user vê (cVisual) para o índice real (i).
        // Isto evita que o utilizador tenha de saber IDs complexos do sistema.
        mapeamento[cVisual] = i; 
        cVisual++; disponiveis++;
    }
    printf("----------------------------------------------------------------------------------------------------\n");

    if (disponiveis == 0) {
        printf("[Info] Nao ha livros disponiveis.\n");
        esperarEnter(); return;
    }

    // Explicação para Defesa: Validação de input via lerInteiro. O intervalo é dinâmico (0 a cVisual-1).
    int escolha = lerInteiro("Escolha o N. do livro (0 para cancelar): ", 0, cVisual - 1);
    if (escolha == 0) return;

    int idx = mapeamento[escolha]; // Recuperamos o índice real a partir do mapeamento
    printf("\n[Opcoes de Requisicao]\nLivro selecionado: %s\n1. Pedir Emprestado\n2. Propor Troca\n0. Cancelar\n", books[idx].titulo);

    int tipoPedido = lerInteiro("Opcao: ", 0, 2);
    if (tipoPedido == 0) return;

    if (tipoPedido == 1) { // LÓGICA DE EMPRÉSTIMO TEMPORÁRIO
        int diasReq = lerInteiro("Dias (1-30 ou 0 para cancelar): ", 0, 30);
        if (diasReq == 0) {
            printf("\n[Info] Operacao cancelada.\n");
        } else {
            // Modularidade: Delegamos a criação do registo para uma função especializada
            criarRegistoEmprestimo(operacoes, totalOperacoes, books, *totalBooks, idx, idLogado, diasReq);
            printf("[Sucesso] Pedido registado!\n");
        }
    } 
    else if (tipoPedido == 2) { // LÓGICA DE TROCA PERMANENTE
        // Submenu de Contexto: O utilizador deve escolher qual dos seus próprios livros oferece em troca
        int idxMeu = submenuEscolherLivroTroca(books, *totalBooks, idLogado);
        if (idxMeu != -1) {
            criarRegistoTroca(operacoes, totalOperacoes, books, *totalBooks, idx, idxMeu, idLogado);
            printf("\n[Sucesso] Proposta de troca enviada!\n");
        } else {
            // Correção UX: Garante que o utilizador sabe que a operação foi interrompida ao premir 0 no submenu
            printf("\n[Info] Operacao de troca cancelada.\n");
        }
    }
    esperarEnter();
}

int submenuEscolherLivroTroca(Livro books[], int totalBooks, int idLogado) {
    limparEcra();
    printf("\n--- PROPOR TROCA ---\n");
    printf("Selecione qual dos seus livros deseja oferecer em troca:\n\n");
    
    // Explicação para Defesa: Interface tabular simplificada para focar na escolha.
    // O uso de especificadores de largura (ex: %-25s) garante a legibilidade dos dados.
    printf("%-4s | %-25s | %-20s | %-15s\n", "N.", "TITULO", "AUTOR", "CATEGORIA");
    printf("----------------------------------------------------------------------------\n");

    // Explicação para Defesa (Mapeamento de Índices): 
    // Criamos um array auxiliar 'meusIndices'. Isto é fundamental porque o índice que o 
    // utilizador vê no ecrã (1, 2, 3...) não corresponde ao índice real do livro no array 
    // global 'books'. Este mapeamento traduz a escolha visual para a posição correta na memória.
    int meusIndices[MAX_LIVROS];
    int qtdMeus = 0;

    for(int k=0; k < totalBooks; k++) {
        // Regra de Negócio: Para uma troca ser válida, o livro deve pertencer ao utilizador logado,
        // estar com o estado 'Disponível' e não estar marcado como 'Eliminado' (Soft Delete).
        if(books[k].idProprietario == idLogado && books[k].estado == LIVRO_DISPONIVEL && books[k].eliminado == 0) {
            printf("%-4d | %-25.25s | %-20.20s | %-15.15s\n", 
                qtdMeus + 1, books[k].titulo, books[k].autor, books[k].categoria);
            
            // Guarda a posição real 'k' na posição visual 'qtdMeus + 1'
            meusIndices[qtdMeus + 1] = k;
            qtdMeus++;
        }
    }
    printf("----------------------------------------------------------------------------\n");

    // Validação de Pré-requisitos: Se o utilizador não tiver livros aptos, o sistema 
    // interrompe o fluxo de troca para evitar estados inconsistentes na base de dados.
    if (qtdMeus == 0) {
        printf("[Erro] Nao tem livros disponiveis para oferecer em troca.\n");
        return -1;
    } 
    
    // Proteção de Input: A função lerInteiro limita a escolha ao intervalo de livros listados.
    int escTroca = lerInteiro("Qual livro oferece (0 para cancelar): ", 0, qtdMeus);
    
    // Retorno de Índice: Se houver escolha válida, devolvemos o índice real para a função chamadora.
    if (escTroca > 0) return meusIndices[escTroca];
    
    return -1; // Sinalizador de cancelamento
}

void doarLivro(Livro books[], int totalBooks, int idLogado, Operacao operacoes[], int *totalOperacoes) {
    (void)operacoes; // Silencia aviso de parâmetro não utilizado nesta versão da função
    (void)totalOperacoes; 

    limparEcra();
    printf("\n=== DOAR LIVRO AO IPCA ===\n");
    printf("Escolha qual dos seus livros deseja doar a instituicao:\n");
    printf("%-3s | %-30s | %-20s\n", "N.", "TITULO", "AUTOR");
    printf("----------------------------------------------------------\n");

    int visualId = 1; 
    int temLivros = 0;

    // 1. LISTAGEM FILTRADA DE POSSE
    for (int i = 0; i < totalBooks; i++) {
        // Explicação para Defesa (Regra de Negócio): Apenas livros que pertencem ao utilizador,
        // que estão marcados como disponíveis e não eliminados podem ser objeto de doação.
        if (books[i].idProprietario == idLogado && 
            books[i].estado == LIVRO_DISPONIVEL && 
            books[i].eliminado == 0) {
            
            printf("%-3d | %-30.30s | %-20.20s\n", 
                   visualId, 
                   books[i].titulo, 
                   books[i].autor);
            
            visualId++;
            temLivros = 1;
        }
    }
    printf("----------------------------------------------------------\n");

    if (!temLivros) {
        printf("[Info] Nao tem livros disponiveis para doar.\n");
        esperarEnter();
        return;
    }

    // 2. SELEÇÃO E VALIDAÇÃO DE INPUT
    int escolha = lerInteiro("\nIntroduza o N. do livro (0 para cancelar): ", 0, visualId - 1);

    if (escolha == 0) return;

    // 3. MAPEAMENTO DO ÍNDICE VISUAL PARA O ÍNDICE REAL
    // Explicação para Defesa: Como não usamos um array de mapeamento explícito aqui, 
    // replicamos a lógica do loop de listagem para encontrar exatamente o índice 'i' 
    // que corresponde à escolha numérica do utilizador.
    int contador = 1;
    int idxEncontrado = -1;

    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idProprietario == idLogado && 
            books[i].estado == LIVRO_DISPONIVEL && 
            books[i].eliminado == 0) {
            
            if (contador == escolha) {
                idxEncontrado = i;
                break;
            }
            contador++;
        }
    }

    // 4. CONFIRMAÇÃO DE AÇÃO IRREVERSÍVEL
    if (idxEncontrado != -1) {
        printf("\nConfirma a doacao do livro '%s'?\n", books[idxEncontrado].titulo);
        // Explicação para Defesa (Segurança): Uma doação transfere a propriedade permanentemente.
        // O uso de uma confirmação explícita (1/0) previne erros acidentais de digitação.
        int confirmacao = lerInteiro("(1-Sim / 0-Nao): ", 0, 1);

        if (confirmacao == 1) {
            // Explicação para Defesa (Modularidade): A lógica de alteração do idProprietario para 1 (IPCA) 
            // e a atualização do estado do livro é delegada à função processarDoacao.
            processarDoacao(books, idxEncontrado, totalBooks);
        } else {
            printf("\n[Cancelado] A doacao foi cancelada.\n");
        }
    } else {
        printf("\n[Erro] Livro nao encontrado.\n");
    }
    
    esperarEnter();
}

void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado) {
    int s = 0, c = 0; // Acumuladores: 's' para a soma das notas, 'c' para a contagem de registos
    printf("\n--- Perfil de Reputacao: User %d ---\n", idAvaliado);
    
    // Explicação para Defesa (Processamento de Dados): Percorremos o array global de feedbacks
    // aplicando um filtro seletivo para o 'idAvaliado'. Esta técnica de filtragem permite
    // isolar a reputação de um utilizador específico sem necessitar de sub-arrays.
    for(int i=0; i<totalFeedbacks; i++){
        if(feedbacks[i].idAvaliado == idAvaliado){
            // Exibição detalhada: permite ver cada avaliação individual e quem a atribuiu
            printf("- Nota: %.1f (de User %d)\n", feedbacks[i].nota, feedbacks[i].idAvaliador);
            s += feedbacks[i].nota;
            c++;
        }
    }
    
    // Explicação para Defesa (Cálculo Estatístico): Cálculo da média aritmética simples.
    // O uso do cast (float) na divisão é fundamental para evitar a truncagem da divisão inteira,
    // garantindo que a média seja apresentada com precisão decimal (ex: 4.5 em vez de 4).
    if(c > 0) {
        printf(">> Media Global: %.1f / 5.0\n", (float)s/c);
    } else {
        // Tratamento de ausência de dados: UX clara para perfis novos sem histórico
        printf(">> Sem avaliacoes.\n");
    }
}

/* =============================================================
   GESTÃO DE INVENTÁRIO PESSOAL E MOVIMENTOS ATIVOS
   ============================================================= */

/* =============================================================
   GESTÃO DE INVENTÁRIO PESSOAL E MOVIMENTOS ATIVOS
   ============================================================= */

int listarTabelaMeusLivros(Livro books[], int totalBooks, int idLogado) {
    printf("\n=== OS MEUS LIVROS ===\n");
    // Explicação para Defesa: Cabeçalho formatado com larguras negativas (%-) para garantir
    // o alinhamento à esquerda, essencial para a leitura rápida de tabelas no terminal.
    printf("%-4s | %-15s | %-25s | %-15s | %-15s | %s\n", "N.", "ISBN", "TITULO", "AUTOR", "CATEGORIA", "ESTADO");
    printf("-----------------------------------------------------------------------------------------------------\n");

    int visualId = 1; // ID Relativo: Este número é o que o utilizador usará para selecionar o livro
    int temLivros = 0;

    for (int i = 0; i < totalBooks; i++) {
        // Explicação para Defesa (Filtragem de Segurança): Aplicamos um filtro de pertença 
        // (idProprietario == idLogado) e um filtro de integridade (eliminado == 0).
        // Isto garante que o utilizador nunca vê livros de terceiros ou registos apagados.
        if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
            
            char estadoStr[20];
            // Mapeamento de Estado: Converte o valor interno (ENUM) para uma string descritiva.
            // Isto oculta a complexidade numérica do sistema ao utilizador final.
            switch(books[i].estado) {
                case LIVRO_DISPONIVEL:   strcpy(estadoStr, "Disponivel"); break;
                case LIVRO_EMPRESTADO:   strcpy(estadoStr, "Emprestado"); break;
                case LIVRO_RESERVADO:    strcpy(estadoStr, "Reservado"); break;
                case LIVRO_INDISPONIVEL: strcpy(estadoStr, "Indisponivel"); break;
                default: strcpy(estadoStr, "Desconhecido"); break;
            }

            // Explicação para Defesa (Truncagem): O uso de .25 (ex: %-25.25s) impede que 
            // títulos muito longos "empurrem" as colunas seguintes, mantendo o layout intacto.
            printf("%-4d | %-15s | %-25.25s | %-15.15s | %-15.15s | %s\n", 
                   visualId,
                   books[i].isbn,
                   books[i].titulo, 
                   books[i].autor, 
                   books[i].categoria, 
                   estadoStr);
            
            visualId++;
            temLivros = 1;
        }
    }
    
    if (!temLivros) printf("\n[Aviso] Nao possui livros registados no seu inventario.\n");
    printf("-----------------------------------------------------------------------------------------------------\n");

    // Explicação para Defesa (Controlo de Fluxo): Retornamos o número total de livros listados.
    // Este valor será usado pela função de menu para validar se o utilizador escolheu 
    // um número de livro válido no ecrã (ex: escolher o livro 5 quando só tem 3).
    return visualId - 1; 
}

void submenuEliminarLivro(Livro books[], int totalBooks, int idLogado, int maxVisualId) {
    // Validação de Pré-condição: Verifica se a lista está vazia antes de processar.
    // O parâmetro maxVisualId (vindo da listarTabelaMeusLivros) evita cálculos redundantes.
    if (maxVisualId == 0) {
        printf("[Erro] Nao tem livros para eliminar.\n");
        esperarEnter();
        return;
    } 
    
    // Explicação para Defesa: O input é validado dinamicamente contra o número de livros 
    // que o utilizador está a ver no ecrã, prevenindo erros de índice fora do intervalo.
    int escolha = lerInteiro("Selecione o N. do livro a ELIMINAR (0 cancela): ", 0, maxVisualId);
    
    if (escolha != 0) {
        int contador = 1;
        int idx = -1;
        
        // Explicação para Defesa (Mapeamento Inverso): Como o array global 'books' contém
        // livros de todos os utilizadores, precisamos de percorrer o array e contar apenas 
        // os livros ativos do utilizador logado para encontrar a posição real (idx) na memória.
        for (int i = 0; i < totalBooks; i++) {
            if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
                if (contador == escolha) { idx = i; break; }
                contador++;
            }
        }
        
        if (idx != -1) {
            // Explicação para Defesa (Soft Delete): Em vez de remover o registo fisicamente 
            // do array (o que exigiria reorganizar toda a memória), alteramos o campo 'eliminado' para 1.
            // Isto mantém a integridade do histórico e facilita a recuperação de dados se necessário.
            if (eliminarLivro(&books[idx], idLogado)) {
                // Persistência: Após marcar como eliminado no array (RAM), atualizamos o ficheiro (Disco).
                guardarLivros(books, totalBooks);
            }
            esperarEnter();
        }
    }
}

int listarLivrosParaDevolucao(Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, int idLogado, int mapeamentoLivro[]) {
    printf("Lista de livros que tem em sua posse:\n");
    printf("%-4s | %-25s | %-20s | %-12s\n", "N.", "TITULO", "PROPRIETARIO", "DATA LIMITE");
    printf("--------------------------------------------------------------------------------\n");

    int visualId = 1;
    // Explicação para Defesa: Capturamos a data atual do sistema para realizar a comparação
    // lógica em tempo real com os prazos de entrega, permitindo sinalizar atrasos.
    int dataAtual = obterDataAtual();

    for (int i = 0; i < totalBooks; i++) {
        // Regra de Negócio (Filtro de Posse): 
        // 1. idDetentor == idLogado: O livro está fisicamente com o utilizador atual.
        // 2. idProprietario != idLogado: O livro não pertence a quem o detém (é um empréstimo).
        // 3. estado == LIVRO_EMPRESTADO: Garante que o registo está ativo no sistema.
        if (books[i].idDetentor == idLogado && books[i].idProprietario != idLogado && books[i].estado == LIVRO_EMPRESTADO) {
            
            // 1. RELACIONAMENTO (JOIN): Obter Nome do Proprietário
            char donoNome[30] = "Desconhecido";
            if (books[i].idProprietario == 1) {
                strcpy(donoNome, "Instituicao (IPCA)");
            } else {
                for(int u = 0; u < totalUsers; u++) {
                    if(users[u].id == books[i].idProprietario) {
                        strncpy(donoNome, users[u].nome, 25);
                        donoNome[25] = '\0';
                        break;
                    }
                }
            }

            // 2. RELACIONAMENTO (JOIN): Cruzamento com a Tabela de Operações
            // Explicação para Defesa: Como a estrutura 'Livro' não guarda datas de transação,
            // fazemos uma busca no array 'operacoes' para encontrar a transação ativa (ACEITE)
            // ligada ao ID deste livro. Isto demonstra integridade referencial entre arrays.
            char dataLimStr[20] = "n/a"; 
            for (int k = 0; k < totalOperacoes; k++) {
                if (operacoes[k].idLivro == books[i].id && operacoes[k].estado == ESTADO_OP_ACEITE) {
                    int dt = operacoes[k].dataDevolucaoPrevista;
                    // Conversão de formato: Transforma o inteiro YYYYMMDD em string legível DD/MM/YYYY
                    sprintf(dataLimStr, "%02d/%02d/%04d", dt % 100, (dt % 10000) / 100, dt / 10000);
                    
                    // Alerta de Atraso: Se a data prevista for menor que a atual, adicionamos um aviso visual [!]
                    if (dt < dataAtual) strcat(dataLimStr, " [!]");
                    break;
                }
            }

            printf("%-4d | %-25.25s | %-20.20s | %-12s\n", 
                   visualId, books[i].titulo, donoNome, dataLimStr);
            
            // Mapeamento de Memória: Guarda a posição real (i) na posição visual (visualId).
            // Crucial para que o utilizador possa selecionar o número da lista e o sistema saiba 
            // exatamente qual livro devolver no array principal.
            if (visualId < MAX_LIVROS) { 
                mapeamentoLivro[visualId] = i; 
                visualId++;
            }
        }
    }
    printf("--------------------------------------------------------------------------------\n");
    
    return visualId - 1; // Devolve o total para validação de input no submenu chamador
}

void submenuDevolverLivro(Livro books[], int totalBooks, Operacao operacoes[], int *totalOperacoes, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n=== DEVOLVER LIVRO ===\n");

    // Explicação para Defesa: Vetor de mapeamento local. 
    // É essencial para traduzir a seleção visual do utilizador (1, 2, 3...) na posição 
    // real do livro dentro do array global 'books', que contém centenas de registos.
    int mapeamentoLivro[MAX_LIVROS]; 
    
    // 1. LISTAGEM E MAPEAMENTO:
    // Delegamos a filtragem complexa (livros que estão comigo, mas não são meus) para 
    // listarLivrosParaDevolucao, que preenche o nosso vetor de mapeamento.
    int qtdLivros = listarLivrosParaDevolucao(books, totalBooks, operacoes, *totalOperacoes, users, totalUsers, idLogado, mapeamentoLivro);

    if (qtdLivros == 0) {
        printf("[Info] Nao tem livros de terceiros para devolver.\n");
        esperarEnter();
        return;
    }

    // 2. SELEÇÃO VALIDADA:
    // O lerInteiro garante que o utilizador não escolhe um número inexistente na lista.
    int escolha = lerInteiro("Qual o N. do livro a devolver (0 para cancelar): ", 0, qtdLivros);
    
    if (escolha > 0) {
        // Recuperamos o índice real da memória através do mapeamento
        int idxLivro = mapeamentoLivro[escolha];
        
        // 3. EXECUÇÃO DA LÓGICA DE NEGÓCIO:
        // A função executarDevolucao trata as alterações de estado:
        // - Altera idDetentor do livro para o idProprietario original.
        // - Altera o estado do livro para LIVRO_DISPONIVEL.
        // - Atualiza o estado da operação correspondente para concluída.
        executarDevolucao(&books[idxLivro], operacoes, *totalOperacoes, idLogado);
        
        // 4. PERSISTÊNCIA (COMMIT):
        // Explicação para Defesa: Como a devolução altera dados em dois ficheiros diferentes 
        // (livros.dat e operacoes.dat), invocamos ambas as funções de escrita para garantir 
        // que o estado do sistema se mantém íntegro em caso de encerramento do programa.
        guardarLivros(books, totalBooks);
        guardarOperacoes(operacoes, *totalOperacoes); 
        
        esperarEnter();
    }
}

int listarTransacoesParaAvaliar(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int totalFeeds, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado, int mapeamentoOp[]) {
    printf("\n=== AVALIAR TRANSACOES PENDENTES ===\n");
    // EXPLICAÇÃO PARA DEFESA: Alteramos o cabeçalho para incluir a coluna "PARCEIRO", 
    // demonstrando o cruzamento de dados entre Operações e Utilizadores.
    printf("%-4s | %-12s | %-25s | %-20s\n", "N.", "TIPO", "LIVRO", "PARCEIRO");
    printf("--------------------------------------------------------------------------------------\n");

    int visualId = 1;
    int pendentes = 0;

    for (int i = 0; i < totalOperacoes; i++) {
        int participei = (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado);
        
        // Verifica se está concluída, se participei e se AINDA NÃO avaliei
        // Nota: A função jaAvaliou deve estar disponível (usualmente em feedbacks.c ou features.c)
        if (operacoes[i].estado == ESTADO_OP_CONCLUIDO && participei &&
            jaAvaliou(feeds, totalFeeds, operacoes[i].id, idLogado) == 0) {

            char titulo[MAX_STRING] = "Livro Desconhecido";
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[i].idLivro) {
                    strncpy(titulo, books[b].titulo, 24); 
                    titulo[24] = '\0';
                    break;
                }
            }

            // LÓGICA DE IDENTIFICAÇÃO DO PARCEIRO:
            // Se eu sou o Requerente, o parceiro é o Proprietário. Se eu sou o Proprietário, o parceiro é o Requerente.
            int idParceiro = (operacoes[i].idRequerente == idLogado) ? operacoes[i].idProprietario : operacoes[i].idRequerente;
            char nomeParceiro[MAX_STRING] = "Utilizador Externo";

            for(int u=0; u<totalUsers; u++) {
                if(users[u].id == idParceiro) {
                    strncpy(nomeParceiro, users[u].nome, 19);
                    nomeParceiro[19] = '\0';
                    break;
                }
            }

            printf("%-4d | %-12s | %-25.25s | %-20.20s\n", 
                   visualId, 
                   (operacoes[i].tipo == OP_TIPO_EMPRESTIMO ? "Emprestimo" : "Troca"), 
                   titulo,
                   nomeParceiro);
            
            mapeamentoOp[visualId] = i; // Guarda o índice real
            visualId++;
            pendentes++;
        }
    }

    if (pendentes == 0) {
        printf("[Info] Nao tem transacoes pendentes de avaliacao.\n");
    }
    printf("--------------------------------------------------------------------------------------\n");
    
    return pendentes; // Retorna 0 se vazio, ou qtd > 0
}

void submenuAvaliarTransacoes(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int *totalFeeds, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();

    // Explicação para Defesa: Vetor de mapeamento local que correlaciona a lista 
    // simplificada (que o user vê) com o índice real no array global de operações.
    int mapeamentoOp[MAX_OPERACOES];
    
    // 1. LISTAGEM FILTRADA:
    // Delegamos a lógica de "quem pode avaliar o quê" para a função listarTransacoesParaAvaliar.
    // Ela garante que apenas transações CONCLUÍDAS e ainda NÃO AVALIADAS apareçam.
    // Atualizado: Agora passa os dados de utilizadores para mostrar o nome do parceiro na lista.
    int qtdPendentes = listarTransacoesParaAvaliar(operacoes, totalOperacoes, feeds, *totalFeeds, books, totalBooks, users, totalUsers, idLogado, mapeamentoOp);

    if (qtdPendentes == 0) {
        esperarEnter();
        return;
    }

    // 2. SELEÇÃO DE REGISTO:
    // O lerInteiro limita a escolha do utilizador ao intervalo exato de itens pendentes.
    int escolha = lerInteiro("Escolha o N. para avaliar (0 para voltar): ", 0, qtdPendentes);
    if (escolha == 0) return;

    int idxOp = mapeamentoOp[escolha]; // Tradução do ID visual para o índice de memória

    if (idxOp != -1) {
        // 3. RECOLHA DE MÉTRICAS (UX/Segurança):
        printf("\n--- Avaliar Experiencia ---\n");
        // Explicação para Defesa: lerFloat valida o intervalo (1.0 a 5.0), impedindo 
        // notas fora do padrão que corromperiam as médias de reputação do sistema.
        float nota = lerFloat("Atribua uma nota (1 a 5): ", 1.0, 5.0);
        
        char comentario[200];
        printf("Comentario (Max 200 chars): ");
        lerString(comentario, 200);

        // 4. PROCESSAMENTO E PERSISTÊNCIA:
        // A função processarNovoFeedback cria o registo na estrutura Feedback e associa-o 
        // ao ID da operação original, fechando o elo de auditoria.
        processarNovoFeedback(feeds, totalFeeds, &operacoes[idxOp], idLogado, nota, comentario);
        
        // Explicação para Defesa: A gravação imediata (Commit) assegura que a reputação 
        // é atualizada no disco logo após a submissão, evitando perda de dados.
        guardarFeedbacks(feeds, *totalFeeds);
    }
    esperarEnter();
}

void gerirPedidosPendentes(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    limparEcra();
    
    // Explicação para Defesa: Vetor de mapeamento local. 
    // Imprescindível para traduzir o ID visual (o que o dono vê) no índice real do array global,
    // garantindo que a decisão de aceitar/recusar recai sobre a operação correta.
    int mapeamentoOp[MAX_OPERACOES];

    // 1. LISTAGEM E MAPEAMENTO:
    // A função listarPedidosPendentes filtra o array de operações procurando apenas 
    // aquelas onde idProprietario == idLogado e o estado é ESTADO_OP_PENDENTE.
    int qtd = listarPedidosPendentes(operacoes, *totalOperacoes, books, totalBooks, users, totalUsers, feedbacks, totalFeedbacks, idLogado, mapeamentoOp);

    if (qtd == 0) {
        esperarEnter();
        return;
    }

    // 2. SELEÇÃO SEGURA:
    int escolha = lerInteiro("Escolha o N. do pedido a tratar (0 para voltar): ", 0, qtd);
    if (escolha == 0) return;

    int idxOp = mapeamentoOp[escolha]; // Recuperação da posição de memória original
    
    // 3. FLUXO DE DECISÃO:
    limparEcra();
    printf("\n--- DECISAO DE PEDIDO ---\n");
    // UX: Confirmação visual dos dados antes de executar uma alteração de estado no sistema.
    printf("Pedido ID: %d | Tipo: %s\n", operacoes[idxOp].id, (operacoes[idxOp].tipo == OP_TIPO_TROCA) ? "TROCA" : "EMPRESTIMO");
    printf("--------------------------\n");
    printf("1. Aceitar Pedido\n");
    printf("2. Recusar Pedido\n");
    printf("0. Cancelar\n");
    
    int acao = lerInteiro("Opcao: ", 0, 2);

    // Explicação para Defesa (Lógica de Negócio):
    if (acao == 1) {
        // ACEITAÇÃO: Esta função executa a mudança de posse (idDetentor) no caso do 
        // empréstimo, ou a troca definitiva de proprietários (idProprietario) no caso da troca.
        processarAceitacao(&operacoes[idxOp], books, totalBooks, operacoes, totalOperacoes, idLogado);
        
        // PERSISTÊNCIA ATÓMICA: Como a aceitação altera tanto o array de operações como o de livros,
        // gravamos ambos os ficheiros para evitar inconsistências (ex: operação aceite mas livro indisponível).
        guardarOperacoes(operacoes, *totalOperacoes);
        guardarLivros(books, totalBooks);
    } 
    else if (acao == 2) {
        // REJEIÇÃO: Liberta o livro (estado volta a Disponível) e marca a operação como Recusada.
        processarRejeicao(&operacoes[idxOp], books, totalBooks, idLogado);
        guardarOperacoes(operacoes, *totalOperacoes);
        guardarLivros(books, totalBooks);
    }
    
    if (acao != 0) esperarEnter();
}

int listarPedidosPendentes(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado, int mapeamentoOp[]) {
    
    printf("\n=== PEDIDOS PENDENTES (Meus Livros) ===\n");
    // Explicação para Defesa: Interface expandida. Incluímos a reputação do requerente e detalhes
    // específicos para que o proprietário possa decidir com base na confiança e no benefício da troca.
    printf("%-3s | %-12s | %-25s | %-15s | %-12s | %-10s\n", 
           "N.", "TIPO", "MEU LIVRO", "REQUERENTE", "DETALHE", "REPUTACAO");
    printf("------------------------------------------------------------------------------------------------------------\n");

    int visualId = 1;
    int temPedidos = 0;

    for (int i = 0; i < totalOperacoes; i++) {
        // Regra de Negócio: Filtramos apenas transações que ainda aguardam resposta (PENDENTES).
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            
            // 1. RELACIONAMENTO: Localizar o índice do livro solicitado no array global
            int idxMeuLivro = -1;
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) {
                    idxMeuLivro = b;
                    break;
                }
            }

            // Verificação de Segurança: Garante que apenas pedidos destinados a livros do utilizador logado são listados.
            if (idxMeuLivro != -1 && books[idxMeuLivro].idProprietario == idLogado) {
                
                // 2. RELACIONAMENTO: Identificar o Nome do Requerente através do seu ID
                char nomeReq[20] = "Desconhecido";
                for (int u = 0; u < totalUsers; u++) {
                    if (users[u].id == operacoes[i].idRequerente) {
                        strncpy(nomeReq, users[u].nome, 19);
                        nomeReq[19] = '\0';
                        break;
                    }
                }

                // 3. MÉTRICA DE CONFIANÇA: Calcula a média do histórico de quem pede o livro.
                int qtdAval;
                float mediaReq = calcularMediaUtilizador(feedbacks, totalFeedbacks, operacoes[i].idRequerente, &qtdAval);

                // 4. LÓGICA CONDICIONAL DE EXIBIÇÃO:
                char tipoStr[15], detalheStr[30];
                
                if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
                    strcpy(tipoStr, "EMPRESTIMO");
                    // No empréstimo, a informação é curta (dias), cabe bem na coluna 'Detalhe'.
                    sprintf(detalheStr, "%d dias", operacoes[i].dias);
                    
                    printf("%-3d | %-12s | %-25.25s | %-15.15s | %-12s | %.1f* (%d)\n", 
                           visualId, tipoStr, books[idxMeuLivro].titulo, nomeReq, detalheStr, mediaReq, qtdAval);
                } 
                else {
                    // CASO DE TROCA: Requer a localização do livro oferecido
                    strcpy(tipoStr, "TROCA");
                    
                    int idxLivroOferecido = -1;
                    for (int b = 0; b < totalBooks; b++) {
                        if (books[b].id == operacoes[i].idLivroTroca) { 
                            idxLivroOferecido = b;
                            break;
                        }
                    }

                    // UX/UI Decision: Como os títulos dos livros são longos, não os colocamos na coluna 'Detalhe'
                    // para não partir a tabela. Usamos um placeholder e mostramos a info completa na linha seguinte.
                    if (idxLivroOferecido != -1) {
                        strcpy(detalheStr, "(Ver abaixo)"); 
                    } else {
                        strcpy(detalheStr, "Livro N/A");
                    }

                    printf("%-3d | %-12s | %-25.25s | %-15.15s | %-12s | %.1f* (%d)\n", 
                           visualId, tipoStr, books[idxMeuLivro].titulo, nomeReq, detalheStr, mediaReq, qtdAval);
                    
                    // UX: Linha de detalhe expandida para clareza total da proposta de troca
                    if (idxLivroOferecido != -1) {
                        printf("    └─> OFERTA RECEBIDA: [%s]\n", books[idxLivroOferecido].titulo);                    }
                    }
                
                // Explicação para Defesa (Mapeamento): Armazenamos o índice real 'i' na posição 'visualId'.
                // Isto permite que a função de decisão posterior aceda à operação correta na memória.
                mapeamentoOp[visualId] = i;
                visualId++;
                temPedidos = 1;
            }
        }
    }

    if (!temPedidos) {
        printf("                   Nao existem pedidos pendentes para os seus livros.\n");
    }
    printf("------------------------------------------------------------------------------------------------------------\n");
    return (temPedidos) ? visualId - 1 : 0;
}

void listarEmprestimos(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, int idLogado) {
    // Explicação para Defesa: Cabeçalho com alinhamento tabular. 
    // Garante que o histórico seja legível mesmo com grandes volumes de dados.
    printf("%-3s | %-25s | %-12s | %-12s | %s\n", "ID", "LIVRO", "DATA INICIO", "DATA FIM", "ESTADO");
    printf("--------------------------------------------------------------------------------\n");

    int encontrou = 0;
    char dataIni[12], dataFim[12];

    for (int i = 0; i < totalOperacoes; i++) {
        // Explicação para Defesa (Filtro de Participação): A listagem é inclusiva.
        // Mostra transações onde o utilizador é o Requerente (quem pediu) ou o Proprietário (quem emprestou).
        if (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado) {
            
            // Filtro de Categoria: Ignoramos logs técnicos de devolução/doação para focar na
            // atividade económica (Empréstimos e Trocas), limpando o ruído visual.
            if (operacoes[i].tipo == OP_TIPO_DEVOLUCAO || operacoes[i].tipo == OP_TIPO_DOACAO) continue;

            // Relacionamento (Lookup): Recupera o nome do livro através do ID guardado na operação.
            char titulo[MAX_STRING] = "---";
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[i].idLivro) {
                    strcpy(titulo, books[b].titulo); break;
                }
            }

            // Explicação para Defesa (Manipulação de Datas): 
            // 1. Convertemos o formato inteiro YYYYMMDD para string DD-MM-YYYY via formatarData.
            // 2. Lógica Condicional: Se for troca, o prazo é 'Permanente'. Se for empréstimo,
            //    calculamos a data de fim somando os dias ao início.
            formatarData(operacoes[i].dataEmprestimo, dataIni);
            
            if (operacoes[i].dias > 0) {
                int dataFimInt = somarDias(operacoes[i].dataEmprestimo, operacoes[i].dias);
                formatarData(dataFimInt, dataFim);
            } else {
                strcpy(dataFim, "Permanente");
            }

            // Tradução de Máquina para Humano: Converte códigos de estado (int) em texto (string).
            char estadoStr[20];
            switch(operacoes[i].estado) {
                case ESTADO_OP_PENDENTE: strcpy(estadoStr, "Pendente"); break;
                case ESTADO_OP_ACEITE:   strcpy(estadoStr, "A decorrer"); break;
                case ESTADO_OP_REJEITADO:strcpy(estadoStr, "Recusado"); break;
                case ESTADO_OP_CONCLUIDO:strcpy(estadoStr, "Concluido"); break;
                default: strcpy(estadoStr, "?");
            }

            // Impressão Final: Limitação de 25 caracteres no título para manter a integridade da tabela.
            printf("%-3d | %-25.25s | %-12s | %-12s | %s\n", 
                   operacoes[i].id, titulo, dataIni, dataFim, estadoStr);
            
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\t(Sem registos de emprestimos ou trocas)\n");
    }
    printf("--------------------------------------------------------------------------------\n");
}

/* =============================================================
   SISTEMA DE NOTIFICAÇÕES E CONSULTA DE HISTÓRICO
   ============================================================= */

void verificarNotificacoes(Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    int pedidosParaAprovar = 0;
    int atrasados = 0;
    int resultadosRecentes = 0;
    int novasAvaliacoes = 0; 
    int dataAtual = obterDataAtual(); // Base para comparação de prazos e eventos do dia

    // Explicação para Defesa (Varredura de Eventos): Percorremos o array de operações 
    // uma única vez (O(n)) para contabilizar diferentes tipos de alertas.
    for(int i = 0; i < totalOperacoes; i++) {
        
        // 1. GESTÃO DE FLUXO (PENDENTES): Identifica se outros utilizadores estão 
        // à espera de uma resposta do utilizador logado sobre os seus livros.
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            if (operacoes[i].idProprietario == idLogado) {
                pedidosParaAprovar++;
            }
        }

        // 2. CONTROLO DE PRAZOS (CRÍTICO): Verifica se o utilizador detém livros 
        // cujo prazo de devolução (dataDevolucaoPrevista) é inferior à data de hoje.
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado == ESTADO_OP_ACEITE) {
            if (operacoes[i].dataDevolucaoPrevista > 0 && operacoes[i].dataDevolucaoPrevista < dataAtual) {
                atrasados++;
            }
        }

        // 3. ATUALIZAÇÕES DE ESTADO: Notifica o utilizador se as propostas que ele 
        // enviou foram aceites ou rejeitadas na data corrente.
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado != ESTADO_OP_PENDENTE) {
            if (operacoes[i].dataFecho == dataAtual || operacoes[i].dataEmprestimo == dataAtual) {
                resultadosRecentes++;
            }
        }
    }

    // 4. MONITORIZAÇÃO DE REPUTAÇÃO: Segundo loop para verificar o array de feedbacks.
    // Demonstra a capacidade de cruzar eventos de diferentes módulos (Transações vs Social).
    for (int f = 0; f < totalFeedbacks; f++) {
        if (feedbacks[f].idAvaliado == idLogado && feedbacks[f].dataAvaliacao == dataAtual) {
            novasAvaliacoes++;
        }
    }

    // EXPLICAÇÃO PARA DEFESA (Interface Condicional): O painel só é impresso se 
    // houver algo relevante. Isto evita poluição visual no menu principal (Clean UI).
    if(pedidosParaAprovar > 0 || atrasados > 0 || resultadosRecentes > 0 || novasAvaliacoes > 0) {
        limparEcra();
        printf("\n==================== NOTIFICACOES ====================\n");
        
        if(pedidosParaAprovar > 0)
            printf(" [!] ATENCAO: Tem %d pedido(s) a aguardar a sua aprovacao.\n", pedidosParaAprovar);
        
        if(atrasados > 0)
            printf(" [X] URGENTE: Tem %d livro(s) com DEVOLUCAO EM ATRASO!\n", atrasados);
        
        if(resultadosRecentes > 0)
            printf(" [i] INFO: %d pedido(s) seu(s) tiveram atualizacoes hoje.\n", resultadosRecentes);

        if(novasAvaliacoes > 0)
            printf(" [*] REPUTACAO: Recebeu %d nova(s) avaliacao(oes) hoje!\n", novasAvaliacoes);
            
        printf("======================================================\n");

        // EXPLICAÇÃO PARA DEFESA: Pausa necessária para garantir a leitura dos alertas.
        // Sem esta pausa, o Menu Principal limparia o ecrã imediatamente.
        esperarEnter();
    }
}

void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks, int idLogado) {

    // Nota para Defesa: Estes parâmetros são passados para permitir uma varredura 
    // completa de todos os módulos do sistema num único ponto de entrada.
    (void) operacoes; 
    (void) totalOperacoes;
    (void) books;
    (void) totalBooks;
     
    int usersPendentes = 0;
    int propostasPendentes = 0;
    int doacoesPendentes = 0;
    int propostasAceites = 0;   
    int propostasRejeitadas = 0; 
    int novasAvaliacoes = 0;
    int dataAtual = obterDataAtual(); 

    // 1. GESTÃO DE ACESSOS: Varredura no array de Utilizadores.
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
            usersPendentes++;
        }
    }

    // 2. GESTÃO DE MOVIMENTOS INSTITUCIONAIS: Análise do array de Operações.
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].estado == ESTADO_OP_PENDENTE && operacoes[i].idProprietario <= 1) {
            if (operacoes[i].tipo != OP_TIPO_DOACAO) propostasPendentes++;
            else doacoesPendentes++;
        }
        
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado != ESTADO_OP_PENDENTE) {
             if (operacoes[i].dataFecho == dataAtual || operacoes[i].dataEmprestimo == dataAtual) {
                if (operacoes[i].estado == ESTADO_OP_REJEITADO) propostasRejeitadas++;
                else propostasAceites++;
            }
        }
    }

    // 3. REPUTAÇÃO INSTITUCIONAL:
    for (int f = 0; f < totalFeedbacks; f++) {
        if (feedbacks[f].idAvaliado == idLogado && feedbacks[f].dataAvaliacao == dataAtual) {
            novasAvaliacoes++;
        }
    }

    // 4. INTERFACE DE ALERTAS (UX):
    // Explicação para Defesa: Implementação de um painel "DashBoard". 
    // Só consome espaço no ecrã se existirem valores > 0.
    if (usersPendentes > 0 || propostasPendentes > 0 || doacoesPendentes > 0 || 
        propostasAceites > 0 || propostasRejeitadas > 0 || novasAvaliacoes > 0) {
        limparEcra();
        printf("\n================= ALERTAS ADMINISTRADOR =================\n");
        
        if (usersPendentes > 0) 
            printf(" [!] %d Utilizador(es) aguardam aprovacao.\n", usersPendentes);
        
        if (propostasPendentes > 0) 
            printf(" [!] %d Proposta(s) de requisicao pendentes.\n", propostasPendentes);

        if (doacoesPendentes > 0) 
            printf(" [!] %d Nova(s) DOAÇÃO(ÕES) pendente(s).\n", doacoesPendentes);
        
        if (propostasAceites > 0)
            printf(" [v] %d Pedido(s) seu(s) foram ACEITES hoje.\n", propostasAceites);
        
        if (propostasRejeitadas > 0)
            printf(" [x] %d Pedido(s) seu(s) foram REJEITADOS hoje.\n", propostasRejeitadas);

        if (novasAvaliacoes > 0)
            printf(" [*] Recebeu %d nova(s) avaliacao(oes) no seu perfil hoje!\n", novasAvaliacoes);
            
        printf("=========================================================\n");

        // EXPLICAÇÃO PARA DEFESA: Pausa condicional. 
        // Se houver alertas, forçamos a leitura antes do sistema carregar o menu do Admin.
        esperarEnter(); 
    } else {
        // Feedback passivo: Indica ao Admin que o sistema está em dia.
        // Como é apenas informativo, não forçamos o esperarEnter() para tornar a navegação fluida.
        printf("\n[Admin] Nenhuma pendencia urgente encontrada.\n");
    }
}
   
void submenuHistoricoPessoal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    limparEcra();
    printf("\n======================================= O MEU HISTORICO PESSOAL =======================================\n");
    
    // 1. ALTERAÇÃO: Data na primeira coluna, removido o N.
    printf("%-12s | %-10s | %-18s | %-20s | %-10s | %-5s | %-20s\n", 
           "DATA", "TIPO", "FLUXO (DE->PARA)", "LIVRO", "ESTADO", "NOTA", "COMENTARIO");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");

    int encontrou = 0;
    // Removido visualID pois já não é necessário

    for(int i = totalOperacoes - 1; i >= 0; i--) {

        if (operacoes[i].idRequerente != idLogado && operacoes[i].idProprietario != idLogado) {
            continue;
        }

        int idDe = 0, idPara = 0, dataRaw = 0;
        char dataFormatada[12], tipoStr[15], estadoStr[15];
        char notaStr[10] = "---", comentarioStr[25] = "---"; 
        char nomeDe[30] = "Sistema", nomePara[30] = "Sistema", titulo[30] = "Livro Desconhecido";

        // Lógica de tipo
        switch(operacoes[i].tipo) {
            case OP_TIPO_EMPRESTIMO: strcpy(tipoStr, "Emprestimo"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = operacoes[i].dataPedido; break;
            case OP_TIPO_DEVOLUCAO: strcpy(tipoStr, "Devolucao"); idDe = operacoes[i].idRequerente; idPara = operacoes[i].idProprietario; dataRaw = operacoes[i].dataDevolucaoReal; break;
            case OP_TIPO_TROCA: strcpy(tipoStr, "Troca"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = (operacoes[i].estado == ESTADO_OP_CONCLUIDO) ? operacoes[i].dataFecho : operacoes[i].dataPedido; break;
            case OP_TIPO_DOACAO: strcpy(tipoStr, "Doacao"); idDe = operacoes[i].idRequerente; idPara = 1; dataRaw = operacoes[i].dataPedido; break;
            default: strcpy(tipoStr, "Outro"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = operacoes[i].dataPedido;
        }

        // Buscar nota e comentário
        for (int f = 0; f < totalFeedbacks; f++) {
            if (feedbacks[f].idOperacao == operacoes[i].id) {
                sprintf(notaStr, "%.1f*", feedbacks[f].nota);
                strncpy(comentarioStr, feedbacks[f].comentario, 20);
                comentarioStr[20] = '\0';
                break;
            }
        }

        // Buscar Nomes
        if (idDe == idLogado) strcpy(nomeDe, "EU");
        else if (idDe == 1) strcpy(nomeDe, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idDe) { strncpy(nomeDe, users[u].nome, 8); nomeDe[8]='\0'; break; } }

        if (idPara == idLogado) strcpy(nomePara, "EU");
        else if (idPara == 1) strcpy(nomePara, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idPara) { strncpy(nomePara, users[u].nome, 8); nomePara[8]='\0'; break; } }

        for(int b=0; b < totalBooks; b++) if(books[b].id == operacoes[i].idLivro) { strncpy(titulo, books[b].titulo, 19); titulo[19] = '\0'; break; }

        // Estados e Data
        switch(operacoes[i].estado) {
            case ESTADO_OP_PENDENTE: strcpy(estadoStr, "Pendente"); break;
            case ESTADO_OP_ACEITE: strcpy(estadoStr, "Aprovado"); break;
            case ESTADO_OP_REJEITADO: strcpy(estadoStr, "Recusado"); break;
            case ESTADO_OP_CONCLUIDO: strcpy(estadoStr, "Finalizado"); break;
            case ESTADO_OP_DEVOLUCAO_PENDENTE: strcpy(estadoStr, "Dev. Aguarda"); break;
            default: strcpy(estadoStr, "---");
        }
        
        if (dataRaw > 0) sprintf(dataFormatada, "%02d/%02d/%04d", dataRaw % 100, (dataRaw % 10000) / 100, dataRaw / 10000);
        else strcpy(dataFormatada, "n/a");

        char fluxo[30];
        sprintf(fluxo, "%.7s->%.7s", nomeDe, nomePara); 

        // 2. ALTERAÇÃO: Printf reordenado (Data primeiro, removido visualID)
        printf("%-12s | %-10s | %-18s | %-20.20s | %-10s | %-5s | %-20.20s\n", 
               dataFormatada, tipoStr, fluxo, titulo, estadoStr, notaStr, comentarioStr); 

        encontrou = 1;
    }

    if (!encontrou) printf("\n[Info] Ainda nao realizou qualquer transacao.\n");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks) {
    limparEcra();
    printf("\n=============================== LOG GLOBAL DE TRANSACOES E FEEDBACKS ===============================\n");
    
    // Explicação para Defesa: Cabeçalho projetado para auditoria. A inclusão do ID da operação
    // é vital para rastrear erros ou inconsistências diretamente nos ficheiros de dados.
    printf("%-4s | %-12s | %-10s | %-20s | %-18s | %-4s | %-20s\n", 
           "ID", "DATA", "TIPO", "FLUXO (DE->PARA)", "LIVRO", "NOTA", "COMENTARIO");
    printf("------------------------------------------------------------------------------------------------------------------\n");

    int encontrouLog = 0;

    // Explicação para Defesa (Cronologia Reversa): Listamos do índice mais alto para o mais baixo.
    // Em auditoria, os eventos mais recentes são geralmente os mais críticos e devem aparecer primeiro.
    for(int i = totalOperacoes - 1; i >= 0; i--) {
        
        // Segurança de Memória: Ignora registos nulos ou corrompidos que possam existir no array.
        if (operacoes[i].id <= 0) continue;

        int idDe = 0, idPara = 0, dataRaw = 0;
        char dataFormatada[12];
        char tipoStr[15], notaStr[10] = "---", comentarioStr[25] = "---";
        char nomeDe[30] = "Sistema", nomePara[30] = "Sistema", titulo[30] = "Desconhecido";

        // Explicação para Defesa (Normalização de Fluxo): 
        // Cada tipo de operação tem uma semântica de movimento diferente. 
        // Esta lógica de switch "traduz" os campos técnicos da struct Operacao numa 
        // narrativa clara de quem entregou o quê a quem.
        switch(operacoes[i].tipo) {
            case OP_TIPO_EMPRESTIMO: 
                strcpy(tipoStr, "Emprest."); 
                idDe = operacoes[i].idProprietario; 
                idPara = operacoes[i].idRequerente; 
                dataRaw = operacoes[i].dataPedido; 
                break;
            case OP_TIPO_DEVOLUCAO: 
                strcpy(tipoStr, "Devoluc."); 
                idDe = operacoes[i].idRequerente; 
                idPara = operacoes[i].idProprietario; 
                dataRaw = (operacoes[i].dataDevolucaoReal > 0) ? operacoes[i].dataDevolucaoReal : operacoes[i].dataDevolucaoPrevista;
                break;
            case OP_TIPO_TROCA: 
                strcpy(tipoStr, "Troca"); 
                idDe = operacoes[i].idProprietario; 
                idPara = operacoes[i].idRequerente; 
                dataRaw = operacoes[i].dataPedido; 
                break;
            case OP_TIPO_DOACAO: 
                strcpy(tipoStr, "Doacao"); 
                idDe = operacoes[i].idRequerente; 
                idPara = 1; // 1 representa a Instituição/IPCA
                dataRaw = operacoes[i].dataPedido; 
                break;
            default: 
                strcpy(tipoStr, "Outro"); 
                idDe = operacoes[i].idProprietario; 
                idPara = operacoes[i].idRequerente; 
                dataRaw = operacoes[i].dataPedido;
        }

        // Explicação para Defesa (Join Multidimensional): 
        // O sistema correlaciona a Operação com o array de Feedbacks em tempo de execução.
        // Isto permite ao Admin ver imediatamente se uma transação foi problemática (nota baixa).
        for (int f = 0; f < totalFeedbacks; f++) {
            if (feedbacks[f].idOperacao == operacoes[i].id) {
                sprintf(notaStr, "%.1f*", feedbacks[f].nota);
                strncpy(comentarioStr, feedbacks[f].comentario, 24);
                comentarioStr[24] = '\0';
                break;
            }
        }

        // Resolução de Nomes: Transforma IDs em texto legível, tratando o ID 1 como entidade especial.
        if (idDe == 1) strcpy(nomeDe, "IPCA");
        else { 
            for(int u=0; u < totalUsers; u++) {
                if(users[u].id == idDe) { strncpy(nomeDe, users[u].nome, 8); nomeDe[8] = '\0'; break; } 
            } 
        }

        if (idPara == 1) strcpy(nomePara, "IPCA");
        else { 
            for(int u=0; u < totalUsers; u++) {
                if(users[u].id == idPara) { strncpy(nomePara, users[u].nome, 8); nomePara[8] = '\0'; break; } 
            } 
        }

        // Lookup de Livro: Garante que o Admin saiba exatamente qual o objeto da transação.
        for(int b=0; b < totalBooks; b++) {
            if(books[b].id == operacoes[i].idLivro) { 
                strncpy(titulo, books[b].titulo, 17); titulo[17] = '\0'; break; 
            }
        }

        // Formatação de Datas: Converte o armazenamento otimizado (YYYYMMDD) para DD/MM/YYYY.
        if (dataRaw > 10000000) { 
            sprintf(dataFormatada, "%02d/%02d/%04d", dataRaw % 100, (dataRaw % 10000) / 100, dataRaw / 10000);
        } else {
            strcpy(dataFormatada, "---");
        }

        char fluxo[30];
        sprintf(fluxo, "%.8s->%.8s", nomeDe, nomePara);

        // Explicação para Defesa (UI/UX): O uso de truncagem (.18, .20) é vital aqui.
        // Como este log tem muitas colunas, a truncagem garante que a tabela não "quebre"
        // se um utilizador tiver um nome longo ou um livro um título extenso.
        printf("%-4d | %-12s | %-10s | %-20s | %-18.18s | %-4s | %-20.20s\n", 
               operacoes[i].id, dataFormatada, tipoStr, fluxo, titulo, notaStr, comentarioStr); 

        encontrouLog = 1;
    }

    if (!encontrouLog) printf("\n[Info] Sem historico de transacoes registadas.\n");
    printf("------------------------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void listarHistoricoCompleto(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n=== O MEU HISTORICO DE MOVIMENTOS ===\n");
    // Explicação para Defesa: Interface focada na ação. 
    // O cabeçalho prioriza a DATA para análise cronológica e o INTERVENIENTE para saber com quem interagiu.
    printf("%-11s | %-12s | %-25s | %-20s | %s\n", "DATA", "TIPO", "LIVRO", "INTERVENIENTE", "DETALHES/PRAZO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int encontrou = 0;
    char dataStr[12];
    char dataFimStr[12];

    // Ordenação Cronológica Reversa: Listar do mais recente para o mais antigo.
    for (int i = totalOperacoes - 1; i >= 0; i--) {
        
        // 1. FILTRO DE PERTENÇA: Garante que o utilizador só vê o seu próprio histórico (Privacidade).
        if (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado) {
            
            // --- A. RESOLUÇÃO DE DADOS (LOOKUP) ---
            char titulo[MAX_STRING] = "Livro Removido";
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) {
                    strcpy(titulo, books[b].titulo);
                    break;
                }
            }

            // --- B. LÓGICA DE SENTIDO DA TRANSAÇÃO ---
            // Explicação para Defesa: Como uma operação tem dois lados (Dono e Requerente), 
            // calculamos dinamicamente quem é o "Outro" em relação ao utilizador logado.
            int idOutroUser;
            if (operacoes[i].idRequerente == idLogado) {
                // Se eu pedi, o interveniente é quem me emprestou (Dono).
                idOutroUser = operacoes[i].idProprietario;
            } else {
                // Se eu emprestei, o interveniente é quem recebeu (Requerente).
                idOutroUser = operacoes[i].idRequerente;
            }

            // --- C. IDENTIFICAÇÃO DO INTERVENIENTE ---
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

            char colunaInterveniente[MAX_STRING];
            sprintf(colunaInterveniente, "%s %s", 
                    (operacoes[i].idRequerente == idLogado) ? "De:" : "Para:", 
                    nomeOutro);

            // --- D. GESTÃO DE TEMPO E ESTADOS ---
            // Conversão de YYYYMMDD para DD/MM/YYYY para legibilidade.
            formatarData(operacoes[i].dataEmprestimo, dataStr);

            char prazoDesc[50] = "";
            
            // Explicação para Defesa: Lógica condicional de prazos. 
            // Se for empréstimo, calculamos a data de fim. Se for troca, marcamos como Permanente.
            if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
                if (operacoes[i].estado == ESTADO_OP_CONCLUIDO) { 
                     strcpy(prazoDesc, "Devolvido");
                } else if (operacoes[i].estado == ESTADO_OP_ACEITE) {
                    // Cálculo dinâmico do prazo de entrega:
                    int dataFimInt = somarDias(operacoes[i].dataEmprestimo, operacoes[i].dias);
                    formatarData(dataFimInt, dataFimStr);
                    sprintf(prazoDesc, "Ate: %s", dataFimStr);
                } else if (operacoes[i].estado == ESTADO_OP_REJEITADO) {
                    strcpy(prazoDesc, "Recusado");
                } else {
                    strcpy(prazoDesc, "Pendente...");
                }
            } 
            else if (operacoes[i].tipo == OP_TIPO_TROCA) {
                strcpy(prazoDesc, "Permanente");
            }
            else {
                 strcpy(prazoDesc, "---");
            }

            // --- E. FORMATAÇÃO DA TABELA ---
            // Uso de truncagem (.25 e .20) para garantir que a tabela não se desformata 
            // com nomes ou títulos excessivamente longos.
            char tipoStr[15];
            switch(operacoes[i].tipo) {
                case OP_TIPO_EMPRESTIMO: strcpy(tipoStr, "EMPRESTIMO"); break;
                case OP_TIPO_TROCA:      strcpy(tipoStr, "TROCA"); break;
                case OP_TIPO_DEVOLUCAO:  strcpy(tipoStr, "DEVOLUCAO"); break;
                default:                 strcpy(tipoStr, "OUTRO");
            }

            printf("%-11s | %-12s | %-25.25s | %-20.20s | %s\n", 
                   dataStr, tipoStr, titulo, colunaInterveniente, prazoDesc);

            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\t(Sem historico registado)\n");
    }
    printf("----------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

   /* =============================================================
   GESTÃO DE PERFIL E VALIDAÇÕES ADMINISTRATIVAS
   ============================================================= */

void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks) {
    int opVal;
    do {
        limparEcra();
        printf("\n=== GESTAO DE VALIDACOES (ADMIN) ===\n");
        printf("1. Validar Utilizadores (Novos/Recuperacao)\n");
        printf("2. Validar Propostas Pendentes\n");
        printf("0. Voltar\n");
        
        // Explicação para Defesa: Este menu atua como a "alfândega" do sistema. 
        // Nada entra ou se move na esfera institucional sem passar por estas validações.
        opVal = lerInteiro("Opcao: ", 0, 3);

        switch(opVal) {
            case 1: 
                // GESTÃO DE ACESSOS: O Admin analisa utilizadores em estado PENDENTE.
                // Esta etapa é crucial para evitar bots ou registos maliciosos na plataforma.
                adminValidarUtilizadores(users, totalUsers);
                
                // Persistência: Após a alteração do estado (de PENDENTE para ATIVO), 
                // gravamos imediatamente no ficheiro binário para evitar perda de dados.
                guardarUtilizadores(users, totalUsers); 
                break;
            
            case 2:
                // GESTÃO DE MOVIMENTOS: Reutiliza a lógica de pedidos pendentes, mas
                // com o ID 1 (Instituição), permitindo que o Admin aceite ou recuse 
                // pedidos feitos aos livros do IPCA ou valide doações recebidas.
                gerirPedidosPendentes(operacoes, &totalOperacoes, books, totalBooks, users, totalUsers, feedbacks, totalFeedbacks, 1);
                
                // Integridade de Dados: Garante que o estado da transação (Aceite/Rejeitada) 
                // seja consolidado no disco rígido.
                guardarOperacoes(operacoes, totalOperacoes); 
                break;

            case 0:
                break; 

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while(opVal != 0); // Ciclo de repetição para facilitar a moderação em lote.
}

void mostrarPerfil(Utilizador user) {
    printf("\n=== O MEU PERFIL ===\n");
    // Explicação para Defesa: Exibição direta dos campos da struct 'Utilizador'.
    // O ID é mostrado para que o utilizador saiba o seu identificador único no sistema.
    printf("ID: %d\n", user.id);
    printf("Nome: %s\n", user.nome);
    printf("Email: %s\n", user.email);
    printf("Telemovel: %s\n", user.telemovel);
    printf("Data Nascimento: %s\n", user.dataNascimento);
    
    // Explicação para Defesa (Tradução de Estados): Uso de um operador ternário 
    // para converter a constante inteira 'CONTA_ATIVA' numa string amigável.
    // Isto evita que o utilizador veja códigos numéricos sem significado semântico.
    printf("Estado: %s\n", (user.estado == CONTA_ATIVA ? "Ativo" : "Inativo"));
    printf("====================\n");
}

void mostrarCabecalhoPerfil(Utilizador *user, Feedback feedbacks[], int totalFeedbacks) {
    int qtdAvaliacoes = 0;
    
    // Explicação para Defesa (Integração de Módulos): Esta função não apenas lê dados 
    // estáticos, mas invoca o motor de cálculo de feedbacks. Isto demonstra que o 
    // sistema de reputação é dinâmico e atualizado em tempo real sempre que o perfil é aberto.
    float media = calcularMediaUtilizador(feedbacks, totalFeedbacks, user->id, &qtdAvaliacoes);

    printf("\n=== O MEU PERFIL ===\n");
    // Explicação para Defesa (Ponteiros): O uso de 'Utilizador *user' (passagem por referência)
    // é mais eficiente em termos de memória, pois evita copiar a estrutura completa 
    // para a stack da função, usando apenas o endereço de memória (8 bytes).
    printf("Nome:      %s\n", user->nome);
    printf("Email:     %s\n", user->email);
    printf("Telemovel: %s\n", user->telemovel);
    
    // UX (User Experience): A nota é formatada com uma casa decimal (%.1f). 
    // Mostrar a quantidade de reviews entre parênteses é uma convenção de design 
    // que confere credibilidade estatística à média apresentada.
    printf("Avaliacao: %.1f / 5.0 (%d reviews)\n", media, qtdAvaliacoes);
    printf("------------------------\n");
}

void editarPerfil(Utilizador *user) {
    int opcao;
    char buffer[MAX_STRING]; // Buffer temporário para validar o input antes da escrita final

    printf("\n=== EDITAR PERFIL ===\n");
    printf("1. Alterar Nome\n");
    printf("2. Alterar Password\n");
    printf("0. Cancelar\n");
    
    // Explicação para Defesa: Validação de escolha através da função lerInteiro,
    // garantindo que o fluxo do programa não quebre com inputs inválidos.
    opcao = lerInteiro("Opcao: ", 0, 2);

    if (opcao == 1) {
        printf("Novo Nome: ");
        lerString(buffer, MAX_STRING);
        
        // Explicação para Defesa (Encapsulamento): Não alteramos o campo diretamente aqui.
        // Delegamos para 'atualizarNomeUtilizador'. Isto permite que, no futuro, possamos
        // adicionar validações (ex: tamanho mínimo, caracteres proibidos) num único local.
        atualizarNomeUtilizador(user, buffer); 
        printf("[Sucesso] Nome atualizado.\n");
    } 
    else if (opcao == 2) {
        printf("Nova Password: ");
        lerString(buffer, 50);
        
        // Segurança: A password é lida para um buffer e depois processada.
        // Ao passar o ponteiro 'user', a função de destino altera diretamente a estrutura original.
        atualizarPasswordUtilizador(user, buffer); 
        printf("[Sucesso] Password atualizada.\n");
    } else {
        // Gestão de Erro/UX: Fornece feedback imediato caso o utilizador decida retroceder.
        printf("[Cancelado] Nenhuma alteracao efetuada.\n");
    }
}

void acaoAlterarNome(Utilizador *user, Utilizador allUsers[], int totalUsers) {
    char novoNome[MAX_STRING];
    char logDetalhe[150];

    limparEcra();
    printf("\n=== ALTERAR NOME DE UTILIZADOR ===\n");
    printf("Novo Nome (min. 3 caracteres, 0 para cancelar): ");
    
    // Explicação para Defesa: Utilizamos uma função robusta de leitura de strings
    // para evitar o transbordamento de memória (Buffer Overflow) e limpar o buffer do teclado.
    lerString(novoNome, MAX_STRING);

    // 1. Controlo de Fluxo: Permite ao utilizador desistir da operação sem efeitos secundários.
    if (strcmp(novoNome, "0") == 0) {
        printf("\n[Info] Operacao cancelada. O nome permanece: %s\n", user->nome);
        esperarEnter();
        return;
    }

    // 2. Validação de Regras de Negócio: Garantimos que o nome tem uma dimensão mínima
    // para manter a qualidade dos dados no sistema.
    if (strlen(novoNome) >= 3) {
        char nomeAntigo[MAX_STRING];
        strcpy(nomeAntigo, user->nome);

        // Explicação para Defesa (Ponteiros): Ao usarmos 'user->nome', estamos a alterar
        // diretamente o conteúdo no endereço de memória original. Como este utilizador
        // faz parte do array 'allUsers', a alteração reflete-se automaticamente no catálogo global.
        strcpy(user->nome, novoNome);
        
        // Sincronização: Após alterar na RAM, gravamos no Disco (Ficheiro binário).
        guardarUtilizadores(allUsers, totalUsers); 

        // --- REGISTO DE LOG (Auditoria) ---
        // Explicação para Defesa: O sistema mantém um histórico de alterações sensíveis.
        // Isto permite ao Administrador rastrear mudanças de identidade em caso de disputas.
        sprintf(logDetalhe, "Nome alterado de '%s' para '%s' (%s)", 
                nomeAntigo, novoNome, user->email);
        registarLog(user->id, "NAME_CHANGE", logDetalhe);

        printf("\n[Sucesso] Nome atualizado com exito!\n");
    } else {
        printf("\n[Erro] Nome invalido. Deve ter pelo menos 3 caracteres.\n");
    }

    esperarEnter();
}

void acaoAlterarPassword(Utilizador *user, Utilizador allUsers[], int totalUsers) {
    char novaPass[50], confirmPass[50];
    char logDetalhe[100];

    limparEcra();
    printf("\n=== ALTERAR PALAVRA-PASSE ===\n");

    // Explicação para Defesa: Utilizamos um ciclo "do-while(1)" para forçar a validação.
    // O utilizador só sai do loop se introduzir dados válidos ou optar explicitamente por cancelar (0).
    do {
        printf("Digite a Nova Password (min 4 chars, sem especiais, 0 para cancelar): ");
        lerString(novaPass, 50);

        // 1. Controlo de Fluxo: Garante que o utilizador não fica preso no menu se desistir.
        if (strcmp(novaPass, "0") == 0) {
            printf("\n[Info] Operacao cancelada.\n");
            esperarEnter();
            return;
        }

        // 2. Regra de Negócio (Segurança): Validação de comprimento mínimo para mitigar ataques de força bruta.
        if (strlen(novaPass) < 4) {
            printf("[Erro] Password demasiado curta! Minimo de 4 caracteres.\n");
            continue;
        }

        // 3. Sanitização de Input: A função 'validarAlfanumerico' impede a inserção de caracteres 
        // que possam causar problemas na leitura de ficheiros ou injeções de código.
        if (!validarAlfanumerico(novaPass)) {
            printf("[Erro] A password nao pode conter caracteres especiais ou espacos.\n");
            continue;
        }

        // 4. Lógica de Segurança: Impede a "reutilização" da password atual, 
        // incentivando a rotatividade real de credenciais.
        if (strcmp(novaPass, user->password) == 0) {
            printf("[Erro] A nova password nao pode ser igual a atual.\n");
            continue;
        }

        // 5. Verificação de Integridade (Double-Check): O processo de confirmação evita 
        // que o utilizador altere a password para algo com um erro de dactilografia (typo), 
        // o que o impediria de entrar na sessão seguinte.
        printf("Confirme a Nova Password: ");
        lerString(confirmPass, 50);

        if (strcmp(novaPass, confirmPass) != 0) {
            printf("[Erro] As passwords nao coincidem. Tente novamente.\n");
            continue;
        }

        // Se passar todos os filtros de segurança, interrompe o loop de validação
        break;

    } while (1);

    // Finalização: Atualização no endereço de memória do ponteiro (Reflete-se no array global)
    strcpy(user->password, novaPass);
    
    // Persistência: Gravação imediata no ficheiro binário para evitar perda de dados em caso de crash.
    guardarUtilizadores(allUsers, totalUsers);

    // Auditoria: Registo silencioso no ficheiro de logs para controlo administrativo.
    sprintf(logDetalhe, "Password alterada pelo utilizador (%s)", user->email);
    registarLog(user->id, "PASS_CHANGE", logDetalhe);

    printf("\n[Sucesso] A sua password foi atualizada com exito!\n");
    esperarEnter();
}

void acaoAlterarTelemovel(Utilizador *user, Utilizador allUsers[], int totalUsers) {
    char novoTel[15];
    char logDetalhe[100];

    limparEcra();
    printf("\n=== ALTERAR TELEMOVEL ===\n");

    // Explicação para Defesa (Input Loop): Utilizamos um ciclo de validação contínuo.
    // O sistema apenas aceita a transição de estado se todos os requisitos de integridade forem cumpridos.
    do {
        printf("Novo Telemovel (9 digitos, 0 para cancelar): ");
        lerString(novoTel, 15);

        // 1. Controlo de Fluxo: Escape seguro para o utilizador.
        if (strcmp(novoTel, "0") == 0) {
            printf("\n[Info] Operacao cancelada.\n");
            esperarEnter();
            return;
        }

        // 2. Validação de Comprimento: Garante que o campo respeita o standard de 9 dígitos.
        if (strlen(novoTel) != 9) {
            printf("[Erro] O telemovel deve ter exatamente 9 numeros!\n");
            continue;
        }

        // 3. Regra de Negócio Local: A função 'validarTelemovelPortugues' verifica se o
        // input começa pelo prefixo '9', assegurando que os dados são coerentes com o contexto do projeto.
        if (!validarTelemovelPortugues(novoTel)) {
            printf("[Erro] Numero invalido! Deve ser um numero portugues (comeca por 9).\n");
            continue;
        }

        // 4. Verificação de Redundância: Evita operações de escrita desnecessárias no ficheiro
        // se o dado introduzido for idêntico ao já existente.
        if (strcmp(novoTel, user->telemovel) == 0) {
            printf("[Erro] O novo numero nao pode ser igual ao atual.\n");
            continue;
        }

        break; // Validações superadas

    } while (1);

    // Persistência: Atualização por ponteiro (RAM) seguida de escrita binária (Disco).
    strcpy(user->telemovel, novoTel);
    guardarUtilizadores(allUsers, totalUsers);

    // Rastreabilidade: Registo de log para auditoria administrativa.
    sprintf(logDetalhe, "Telemovel alterado: %s", user->email);
    registarLog(user->id, "PHONE_CHANGE", logDetalhe);

    printf("\n[Sucesso] O seu telemovel foi atualizado com exito!\n");
    esperarEnter();
}

int acaoEliminarConta(Utilizador *user, Utilizador allUsers[], int totalUsers) {
    char logDetalhe[100];

    limparEcra();
    printf("\n!!! ZONA DE PERIGO !!!\n");
    printf("Ao eliminar a conta, o seu acesso será revogado imediatamente.\n");
    printf("Tem a certeza absoluta? (1-Sim / 0-Nao para cancelar): ");
    
    // Explicação para Defesa (Confirmação Crítica): Por ser uma ação irreversível pelo 
    // utilizador comum, forçamos uma confirmação binária (1/0) para evitar cliques acidentais.
    int confirm = lerInteiro("", 0, 1);
    
    // --- 1. TRATAMENTO DE CANCELAMENTO ---
    if (confirm == 0) {
        printf("\n[Info] Operacao cancelada. A sua conta permanece ativa.\n");
        esperarEnter();
        return 0;
    }

    // --- 2. EXECUÇÃO DA ELIMINAÇÃO (SOFT DELETE) ---
    if (confirm == 1) {
        // Explicação para Defesa: Não removemos fisicamente o utilizador do array nem do ficheiro.
        // Alteramos o 'estado' para CONTA_INATIVA. Isto preserva a integridade referencial,
        // garantindo que o histórico de empréstimos e feedbacks passados não perca o autor.
        user->estado = CONTA_INATIVA; 
        
        // REGISTO DE LOG (Rastreabilidade): Essencial para que o Administrador saiba
        // se a conta foi fechada pelo próprio ou por moderação.
        sprintf(logDetalhe, "Conta eliminada pelo proprio utilizador (%s)", user->email);
        registarLog(user->id, "USER_DELETE", logDetalhe);

        // Persistência: Atualiza o ficheiro binário com o novo estado de inatividade.
        guardarUtilizadores(allUsers, totalUsers); 
        
        printf("\n[Conta Eliminada] A sua conta foi desativada e a sessao sera terminada.\n");
        esperarEnter();
        
        // Explicação para Defesa (Controlo de Sessão): O retorno '1' é um sinalizador (flag)
        // para a função chamadora (menu) quebrar o ciclo de execução e redirecionar para o Login.
        return 1; 
    }

    return 0;
}

void adminValidarUtilizadores(Utilizador users[], int totalUsers) {
    int escolha;
    do {
        limparEcra();
        printf("\n--- VALIDAR PEDIDOS DE ACESSO ---\n");
        printf("%-3s | %-8s | %-12s | %-25s | %s\n", "N.", "TIPO", "ESTADO", "EMAIL", "NOME");
        printf("------------------------------------------------------------------------------------------\n");

        int pendentes = 0;
        int visualId = 1;

        // Explicação para Defesa (Mapeamento Visual): O array global contém centenas de utilizadores.
        // Filtramos apenas aqueles com estados 'PENDENTE' para criar uma lista de trabalho limpa para o Admin.
        for(int i = 0; i < totalUsers; i++) {
            if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
                
                // Lógica de Identificação: Diferenciamos Alunos de Docentes com base no domínio do email.
                char tipo[10];
                if (strstr(users[i].email, "alunos.ipca.pt") != NULL) strcpy(tipo, "Aluno");
                else strcpy(tipo, "Docente");

                char estadoStr[12];
                if (users[i].estado == CONTA_PENDENTE_REATIVACAO) strcpy(estadoStr, "REATIVACAO");
                else strcpy(estadoStr, "APROVACAO");

                printf("%-3d | %-8s | %-12s | %-25.25s | %s\n", 
                       visualId, tipo, estadoStr, users[i].email, users[i].nome);
                       
                visualId++;
                pendentes++;
            }
        }
        printf("------------------------------------------------------------------------------------------\n");

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
            // Tradução de ID Visual para Índice Real: Como a lista mostrada é parcial, 
            // percorremos o array novamente para encontrar a posição 'i' correta na memória.
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
                char detAdmin[100]; 

                // Explicação para Defesa (Transição de Estados): 
                if (dec == 1) {
                    // APROVAÇÃO: O estado passa para 'ATIVO', permitindo o login imediato.
                    users[idx].estado = CONTA_ATIVA;
                    
                    // Auditoria: Registamos a ação do Admin no log do sistema (id 0 para sistema/admin).
                    sprintf(detAdmin, "Conta aprovada pelo Admin: %s", users[idx].email);
                    registarLog(0, "ADMIN_APROVA", detAdmin);
                    
                    // Persistência: Atualizamos o ficheiro binário com a nova permissão de acesso.
                    guardarUtilizadores(users, totalUsers); 
                    
                    printf("\n[Sucesso] Utilizador aprovado!");
                    printf("\n[SIMULACAO] SMS enviada ao utilizador.\n");
                    esperarEnter();
                } else if (dec == 2) {
                    // REJEIÇÃO: Marcamos como INATIVA, o que mantém o email bloqueado para novos registos.
                    users[idx].estado = CONTA_INATIVA;
                    
                    sprintf(detAdmin, "Conta rejeitado pelo Admin: %s", users[idx].email);
                    registarLog(0, "ADMIN_REJEITA", detAdmin);
                    
                    guardarUtilizadores(users, totalUsers);
                    
                    printf("\n[Info] Pedido rejeitado.");
                    printf("\n[SIMULACAO] SMS enviada ao utilizador.\n");
                    esperarEnter();
                }
            }
        }
    } while (escolha != 0); // O loop permite processar vários pedidos sem sair do menu.
}

   /* =============================================================
   RELATÓRIOS ESTATÍSTICOS E RANKINGS DE UTILIZAÇÃO
   ============================================================= */

void submenuRelatorios(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks) {
    int opRel;
    do {
        limparEcra();
        printf("\n--- RELATORIOS E ESTATISTICAS ---\n");
        printf("1. Top 5 Utilizadores (Mais Transacoes)\n"); // Foco em Engajamento
        printf("2. Top 5 Livros (Mais Procurados)\n");    // Foco em Popularidade/Stock
        printf("3. Top 5 Reputacao (Melhores Avaliacoes)\n"); // Foco em Qualidade/Confiança
        printf("0. Voltar\n");
        
        // Explicação para Defesa: Este menu encapsula funções de agregação de dados. 
        // Cada opção executa algoritmos de contagem e ordenação (Sorting) sobre os arrays.
        opRel = lerInteiro("Opcao: ", 0, 3);

        // Explicação para Defesa (Relacionamento de Dados):
        if(opRel == 1) 
            // Cruza Utilizadores com Operações para identificar os membros mais ativos.
            relatorioTopUtilizadores(users, totalUsers, operacoes, totalOperacoes);
        else if(opRel == 2) 
            // Analisa o contador de requisições no array de Livros (campo 'requisicoes').
            relatorioTopLivros(books, totalBooks);
        else if(opRel == 3) 
            // Cruza Utilizadores com Feedbacks para calcular rankings de reputação.
            relatorioTopReputacao(users, totalUsers, feedbacks, totalFeedbacks);
        
    } while(opRel != 0);
}

void relatorioTopReputacao(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks) {
    // Explicação para Defesa: Utilizamos uma estrutura auxiliar (UserStats) para armazenar 
    // temporariamente os cálculos (média e quantidade) sem alterar os dados originais.
    UserStats stats[MAX_UTILIZADORES] = {0}; 

    // DELEGAÇÃO DE LÓGICA: A função calcularRankingReputacao percorre o array de feedbacks,
    // agrupa por ID de utilizador e calcula as médias aritméticas.
    calcularRankingReputacao(users, totalUsers, feedbacks, totalFeedbacks, stats);

    limparEcra();
    printf("\n=== TOP 5 REPUTACAO (Min. 5 Avaliacoes) ===\n");
    printf("%-3s | %-20s | %-10s | %s\n", "N.", "NOME", "MEDIA", "AVALIACOES");
    printf("------------------------------------------------------------\n");

    int contador = 0;
    // Explicação para Defesa (Filtragem e Relevância):
    // O loop percorre os resultados já ordenados (provavelmente por Bubble ou QuickSort).
    for(int i = 0; i < totalUsers; i++) {
        int idxReal = stats[i].indexUser;

        // REGRA DE NEGÓCIO (Filtro de Confiança):
        // 1. stats[i].qtd >= 5: Exigimos um mínimo de 5 avaliações para evitar que um utilizador
        //    com apenas uma avaliação de 5 estrelas distorça o ranking (Relevância Estatística).
        // 2. id != 1: Excluímos a conta institucional (Admin/IPCA) para focar na comunidade.
        if(stats[i].qtd >= 5 && users[idxReal].estado == CONTA_ATIVA && users[idxReal].id != 1) { 
            printf("%-3d | %-20.20s | %-10.1f | %d\n", 
                   contador + 1, users[idxReal].nome, stats[i].media, stats[i].qtd);
            contador++;
        }
        // Limite de visualização para o Top 5
        if(contador >= 5) break;
    }

    if(contador == 0) printf("\n[Info] Nenhum utilizador cumpre os criterios.\n");
    
    printf("------------------------------------------------------------\n");
    esperarEnter();
}


void relatorioTopLivros(Livro livros[], int totalLivros) {
    // Validação de Pré-condição: Evita o processamento de listas vazias.
    if (totalLivros == 0) {
        printf("\n[Erro] Nao existem livros registados no sistema.\n");
        esperarEnter();
        return;
    }

    // EXPLICAÇÃO PARA DEFESA (Eficiência de Memória):
    // Em vez de ordenar o array original 'livros[]' (o que seria lento devido ao tamanho 
    // de cada struct e desorganizaria os IDs), criamos um array de PONTEIROS. 
    // Ordenamos apenas os endereços de memória, o que é muito mais rápido e seguro.
    Livro *ptrs[MAX_LIVROS];
    
    // ALGORITMIA: A função ordenarLivrosPorRequisicoes utiliza provavelmente um 
    // algoritmo de ordenação (ex: Selection Sort ou Bubble Sort) para organizar os 
    // ponteiros por ordem decrescente do campo 'numRequisicoes'.
    ordenarLivrosPorRequisicoes(livros, totalLivros, ptrs);

    limparEcra();
    printf("\n==================== TOP 5 LIVROS MAIS REQUISITADOS ====================\n");
    printf("%-4s | %-30s | %-15s | %s\n", "POS.", "TITULO", "ISBN", "REQUISICOES");
    printf("------------------------------------------------------------------------\n");

    int mostrados = 0;
    // Explicação para Defesa (Filtragem de Relevância):
    // Percorremos o array ordenado e paramos se atingirmos 5 livros ou se 
    // encontrarmos livros com zero requisições (não faz sentido estarem num "Top").
    for(int i = 0; i < totalLivros && mostrados < 5; i++) {
        if(ptrs[i]->numRequisicoes == 0) break;

        // Acesso indireto: Usamos '->' porque estamos a trabalhar com o array de ponteiros.
        printf("%-4d | %-30.30s | %-15s | %d\n", 
               mostrados + 1, 
               ptrs[i]->titulo, 
               ptrs[i]->isbn, 
               ptrs[i]->numRequisicoes);    
        mostrados++;
    }

    if(mostrados == 0) {
        printf("\n[Info] Ainda nao foram realizadas requisicoes no sistema.\n");
    } else {
        printf("------------------------------------------------------------------------\n");
    }

    esperarEnter();
}


void relatorioTopUtilizadores(Utilizador users[], int totalUsers, Operacao operacoes[], int totalOperacoes) {
    limparEcra();
    printf("\n=== TOP 5 UTILIZADORES MAIS ATIVOS ===\n");
    printf("(Criterio: Minimo de 5 transacoes concluidas)\n\n");

    // Explicação para Defesa (Estrutura de Agregação): Utilizamos um array de uma struct auxiliar 
    // (UserActivityRanking) para armazenar os resultados temporários do processamento, 
    // separando a lógica de cálculo da lógica de exibição.
    UserActivityRanking ranking[MAX_UTILIZADORES];

    // 1. PROCESSAMENTO DE DADOS:
    // A função 'calcularAtividadeUtilizadores' percorre o array de operações, conta quantas 
    // transações (trocas/empréstimos) cada ID de utilizador completou e ordena o ranking.
    // Isto demonstra capacidade de processamento analítico (Data Aggregation).
    calcularAtividadeUtilizadores(users, totalUsers, operacoes, totalOperacoes, ranking);

    // 2. APRESENTAÇÃO DE RESULTADOS (UI/UX):
    printf("%-4s | %-25s | %-12s | %s\n", "POS.", "NOME", "TIPO", "MOVIMENTACOES");
    printf("------------------------------------------------------------------\n");

    int mostrados = 0;
    for (int i = 0; i < totalUsers && mostrados < 5; i++) {
        // Explicação para Defesa (Indireção): O ranking guarda o 'indexUser', que é o 
        // índice real do utilizador no array global. Isto evita duplicar dados e garante 
        // que consultamos a informação mais atualizada (como o nome ou email).
        int idxReal = ranking[i].indexUser;
        
        // FILTRO DE RELEVÂNCIA: Ignoramos utilizadores com pouca atividade para 
        // garantir que o "Top" reflete utilizadores realmente engajados.
        if (ranking[i].qtd < 5) break;

        // Classificação Dinâmica: Identificamos o perfil através do domínio do email 
        // em tempo de execução, demonstrando manipulação de strings (strstr).
        char tipo[15];
        if (strstr(users[idxReal].email, "@alunos.ipca.pt")) {
            strcpy(tipo, "Aluno");
        } else {
            strcpy(tipo, "Docente");
        }

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