#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "interface.h"
#include "users.h"
#include "books.h"
#include "transactions.h"
#include "files.h"
#include "structs.h"

void menuGestaoMovimentos(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks, int idLogado);
void submenuDevolverLivro(Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, int idLogado);
void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers);
void submenuAvaliarTransacoes(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int *totalFeeds, Livro books[], int totalBooks, int idLogado);

// --- LOGIN & MENU PRINCIPAL ---

/**
 * @brief Apresenta o menu de visitante (Login, Registo, Recuperar).
 * @return Retorna o ID do utilizador se fizer login, -1 se continuar visitante, ou -10 para Sair do programa.
 */
int menuModoVisitante(Utilizador users[], int *totalUsers, Operacao operacoes[], int totalOperacoes) {
    int opcao;
    limparEcra();
    printf("\n=== BEM-VINDO AO WOOKINTED ===\n");
    printf("1. Registar Conta\n");
    printf("2. Login\n");
    printf("3. Recuperar Password\n");
    printf("0. Sair\n");
    opcao = lerInteiro("Opcao: ", 0, 3);

    switch(opcao) {
        case 1:
            registarUtilizador(users, totalUsers, operacoes, totalOperacoes);
            guardarUtilizadores(users, *totalUsers); 
            esperarEnter();
            return -1; // Continua visitante
        
        case 2: {
            int resultado = loginUtilizador(users, *totalUsers, operacoes, totalOperacoes);
            if (resultado >= 0) {
                // Login com sucesso! Retornamos o ID
                return resultado; 
            } 
            else {
                esperarEnter();
                return -1; // Falhou login, continua visitante
            }
        }

        case 3:
            recuperarPassword(users, *totalUsers);
            //guardarUtilizadores(users, *totalUsers);
            esperarEnter();
            return -1;

        case 0:
            return -10; // Código especial para fechar o programa

        default:
            printf("Opcao invalida.\n");
            esperarEnter();
            return -1;
    }
}

/**
 * @brief Mostra o menu principal do utilizador logado e pede a opção.
 * @param nome Nome do utilizador para personalizar a mensagem.
 * @return Retorna o número da opção escolhida pelo utilizador.
 */
int mostrarMenuPrincipal(char *nome) {
    limparEcra();
    printf("\n=== MENU PRINCIPAL (%s) ===\n", nome);
    printf("1. Mercado de Livros (Procurar, Pedir, Doar)\n");
    printf("2. Meus Livros (Registar, Editar, Eliminar)\n");
    printf("3. Gestao de Movimentos (Pedidos, Devolucoes, Feedback)\n");
    printf("4. Meu Perfil\n");
    printf("0. Logout\n");
    return lerInteiro("Opcao: ", 0, 4);
}

// --- SUB-MENU VALIDACOES ADMINISTRADOR ---

void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes) {
int opVal;
    do {
        limparEcra();
        printf("\n=== GESTAO DE VALIDACOES (ADMIN) ===\n");
        printf("1. Validar Utilizadores (Novos/Recuperacao)\n");
        printf("2. Validar Livros (Cat. 'Outros')\n");
        printf("3. Validar Propostas Pendentes\n");
        printf("0. Voltar\n");
        
        opVal = lerInteiro("Opcao: ", 0, 3);

        switch(opVal) {
            case 1: 
                // Chama a lógica de validar utilizadores
                adminValidarUtilizadores(users, totalUsers);
                
                guardarUtilizadores(users, totalUsers); // Recomendado guardar aqui
                break;
            
            case 2:
                // Lógica de validar Livros
                adminValidarLivros(users, totalUsers, books, totalBooks);                
                guardarLivros(books, totalBooks); // Recomendado guardar aqui
                break;

            case 3:
                // Lógica de validar Empréstimos
                // Necessita de users e totalUsers para mostrar nomes!
                adminGerirPropostas(operacoes, totalOperacoes, books, totalBooks, users, totalUsers);
                
                guardarOperacoes(operacoes, totalOperacoes); // Recomendado guardar aqui
                guardarLivros(books, totalBooks);
                break;

            case 0:
                break; // Voltar

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while(opVal != 0);
}

// --- MENU ADMINISTRADOR ---

void menuAdministrador(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks) {
    int opcao;
    do {
        limparEcra();
        printf("\n=== PAINEL ADMIN (IPCA - ID 0) ===\n");
        printf("1. Centro de Validacoes (Users, Livros, Propostas)\n");
        printf("2. Gestao de Stock Institucional (Catalogo IPCA)\n");
        printf("3. Monitorizacao (Historico e Duracoes)\n");
        printf("4. Relatorios e Estatisticas\n");
        printf("0. Sair\n");
        opcao = lerInteiro("Opcao: ", 0, 4);

        switch(opcao) {
            case 1:
                submenuValidacoes(users, totalUsers, books, totalBooks, operacoes, totalOperacoes);
                break;

            case 2:
                // Listar apenas livros onde idProprietario == 0
                limparEcra();
                printf("\n--- STOCK DA INSTITUICAO ---\n");
                int stock = 0;
                for(int i=0; i<totalBooks; i++) {
                    if(books[i].idProprietario == 0) {
                        printf("ID: %d | Titulo: %s | Estado: %d\n", books[i].id, books[i].titulo, books[i].estado);
                        stock++;
                    }
                }
                if(stock == 0) printf("A instituicao nao tem livros.\n");
                esperarEnter();
                break;

            case 3:
                limparEcra();
                printf("\n--- HISTORICO GLOBAL DE TRANSACOES ---\n");
                for(int i=0; i<totalOperacoes; i++) {
                     printf("Op #%d | De: %d -> Para: %d | Livro: %d | Data: %d | Estado: %d\n", operacoes[i].id, operacoes[i].idRequerente, operacoes[i].idProprietario, operacoes[i].idLivro, operacoes[i].dataPedido, operacoes[i].estado);
                }
                esperarEnter();
                break;

            case 4: // Relatórios e Estatísticas
            {
                int opRel;
                do {
                    limparEcra();
                    printf("\n--- RELATORIOS E ESTATISTICAS ---\n");
                    printf("1. Top 5 Utilizadores (Mais Transacoes)\n");
                    printf("2. Top 5 Livros (Mais Procurados)\n");
                    printf("3. Top 5 Reputacao (Melhores Avaliacoes)\n");
                    printf("0. Voltar\n");
                    opRel = lerInteiro("Opcao: ", 0, 3);

                    // Certifica-te que tens estas funções auxiliares definidas no users.c ou transactions.c
                    if(opRel == 1) relatorioTopTransacoes(operacoes, totalOperacoes, books, totalBooks);
                    else if(opRel == 2) relatorioTopLivros(books, totalBooks);
                    else if(opRel == 3) relatorioTopReputacao(users, totalUsers, feedbacks, totalFeedbacks);
                    
                    if(opRel != 0) esperarEnter();
                } while(opRel != 0);
                break;
            }

            case 0:
                printf("A sair do Painel de Administrador...\n");
                break;
            
            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while(opcao != 0);
}

// --- SUB MENU PESQUISA LIVROS ---

/**
 * @brief Submenu para pesquisar livros por título, autor ou categoria.
 */
void submenuPesquisaLivros(Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado) {
    int opPesquisa;
    char termoOriginal[MAX_STRING];
    char termoBusca[MAX_STRING];

    do {
        limparEcra();
        printf("\n--- PESQUISAR LIVROS ---\n");
        printf("1. Por Titulo\n");
        printf("2. Por Autor\n");
        printf("3. Por Categoria\n");
        printf("0. Voltar\n");
        opPesquisa = lerInteiro("Opcao: ", 0, 3);

        if (opPesquisa == 0) return;

        // Variáveis de controlo
        int categoriaAlvo = -1;
        int usarFiltroTexto = 0;   // 1 = Título/Autor
        int filtroManualOutro = 0; // 1 = Pesquisa texto DENTRO da categoria Outro
        char termoManualBusca[MAX_STRING]; // Texto específico para "Outro"

        // === PREPARAÇÃO DA PESQUISA ===
        
        if (opPesquisa == 1 || opPesquisa == 2) {
            printf("\nTermo a pesquisar (0 para cancelar): ");
            lerString(termoOriginal, MAX_STRING);
            
            if (strcmp(termoOriginal, "0") == 0) continue; 

            paraMinusculas(termoOriginal, termoBusca);
            usarFiltroTexto = 1;
        } 
        else if (opPesquisa == 3) {
            limparEcra();
            printf("\n--- SELECIONE A CATEGORIA ---\n");
            printf("1. Ficcao\n");
            printf("2. Nao Ficcao\n");
            printf("3. Ciencia\n");
            printf("4. Historia\n");
            printf("5. Biografia\n");
            printf("6. Tecnologia\n");
            printf("7. Outro (Pesquisa Especifica)\n");
            printf("0. Cancelar\n");
            
            int escolha = lerInteiro("Escolha: ", 0, 7);
            if (escolha == 0) continue;

            categoriaAlvo = escolha; 

            // === LÓGICA SIMPLES PARA "OUTRO" (7) ===
            if (categoriaAlvo == 7) {
                printf("\nQual a categoria especifica procura? (Ex: Fantasia): ");
                lerString(termoOriginal, MAX_STRING);
                
                // Se escreveu algo, ativamos o filtro
                if (strlen(termoOriginal) > 0) {
                    paraMinusculas(termoOriginal, termoManualBusca);
                    filtroManualOutro = 1; 
                }
                // Se deixou vazio, filtroManualOutro fica a 0 e mostra todos os "Outros"
            }
        }

        // === EXIBIÇÃO DOS RESULTADOS ===

        limparEcra();
        if (usarFiltroTexto) 
            printf("--- RESULTADOS DA PESQUISA (%s) ---\n", termoOriginal);
        else if (filtroManualOutro)
            printf("--- RESULTADOS EM 'OUTRO' (%s) ---\n", termoOriginal);
        else 
            printf("--- LIVROS DA CATEGORIA SELECIONADA ---\n");

        printf("%-3s | %-30s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
        printf("----------------------------------------------------------------------------------------------------\n");

        int contVis = 1;
        int encontrou = 0;
        char alvoComparacao[MAX_STRING];

        for (int i = 0; i < totalBooks; i++) {
            
            // 1. FILTRO DE VISIBILIDADE (IPCA + Meus Livros)
            int souDono = 0;
            if (books[i].idProprietario == idLogado) souDono = 1;
            if (idLogado == 1 && books[i].idProprietario == 0) souDono = 1;

            if (books[i].eliminado == 1 || souDono == 1) continue; 

            // 2. FILTRO DE MATCH
            int match = 0;

            if (usarFiltroTexto) {
                // Pesquisa Título/Autor
                if (opPesquisa == 1) paraMinusculas(books[i].titulo, alvoComparacao);
                else paraMinusculas(books[i].autor, alvoComparacao);
                
                if (strstr(alvoComparacao, termoBusca) != NULL) match = 1;
            } 
            else {
                // Pesquisa por Categoria ID
                if ((int)books[i].categoria == categoriaAlvo) {
                    match = 1; // Assume match inicial
                    
                    // Se for categoria 7 e tivermos um termo escrito, refinamos
                    if (categoriaAlvo == 7 && filtroManualOutro == 1) {
                        char catManualTemp[MAX_STRING];
                        paraMinusculas(books[i].categoriaManual, catManualTemp);
                        
                        // Se a categoria manual do livro NÃO contiver o termo procurado
                        if (strstr(catManualTemp, termoManualBusca) == NULL) {
                            match = 0; // Remove o match
                        }
                    }
                }
            }

            // 3. IMPRIMIR SE HOUVER MATCH
            if (match) {
                char nomeDono[MAX_STRING];
                if (books[i].idProprietario == 0 || books[i].idProprietario == 1) {
                    strcpy(nomeDono, "Instituicao (IPCA)");
                } else {
                    strcpy(nomeDono, "Desconhecido");
                    for (int u = 0; u < totalUsers; u++) {
                        if (users[u].id == books[i].idProprietario) {
                            strcpy(nomeDono, users[u].nome); break;
                        }
                    }
                }

                // A função auxiliar garante que aparece "Fantasia" em vez de "Outro"
                printf("%-3d | %-30.30s | %-20.20s | %-15.15s | %-20.20s\n", 
                        contVis, 
                        books[i].titulo, 
                        books[i].autor, 
                        obterNomeVisualCategoria(&books[i]), 
                        nomeDono);
                
                contVis++;
                encontrou = 1;
            }
        }

        if (!encontrou) {
            printf("\n[Info] Nenhum livro encontrado.\n");
        }
        printf("----------------------------------------------------------------------------------------------------\n");
        esperarEnter();

    } while (opPesquisa != 0);
}

// --- MENU MERCADO ---


void menuMercadoLivros(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado) {
    int opMercado;

    do {
        limparEcra();
        printf("\n--- MERCADO DE LIVROS ---\n");
        printf("1. Ver Catalogo Completo\n");
        printf("2. Pesquisar Livros\n"); 
        printf("3. Requisitar Livro (Troca/Levantamento)\n");
        printf("4. Doar Livro a Instituicao\n");
        printf("0. Voltar\n");
        opMercado = lerInteiro("Opcao: ", 0, 4);

        switch (opMercado) {
            case 1: // === VER CATALOGO ===
                limparEcra();
                printf("\n--- CATALOGO DO MERCADO ---\n");
                printf("%-3s | %-30s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
                printf("----------------------------------------------------------------------------------------------------\n");

                int contVis = 1;
                int encontrou = 0;

                for (int i = 0; i < *totalBooks; i++) {
                    // 1. Verificar se sou o dono (Lógica Unificada 0 e 1)
                    int souDono = 0;
                    if (books[i].idProprietario == idLogado) souDono = 1;
                    // Se eu sou Admin (1), também sou dono dos livros do sistema (0)
                    if (idLogado == 1 && books[i].idProprietario == 0) souDono = 1;

                    // Filtro: Não mostrar os meus livros, nem livros retidos
                    if (books[i].eliminado == 0 && souDono == 0) {
                        
                        // 2. Definir Nome do Proprietário
                        char nomeDono[MAX_STRING];
                        
                        // UNIFICAÇÃO: 0 e 1 são a Instituição
                        if (books[i].idProprietario == 0 || books[i].idProprietario == 1) {
                            strcpy(nomeDono, "Instituicao (IPCA)");
                        } else {
                            strcpy(nomeDono, "Desconhecido");
                            for(int u = 0; u < totalUsers; u++) {
                                if(users[u].id == books[i].idProprietario) {
                                    strcpy(nomeDono, users[u].nome); break;
                                }
                            }
                        }

                        printf("%-3d | %-30.30s | %-20.20s | %-15.15s | %-20.20s\n", 
                                contVis, books[i].titulo, books[i].autor, 
                                obterNomeCategoria(books[i].categoria, books[i].categoriaManual), nomeDono);
                        
                        contVis++;
                        encontrou = 1;
                    }
                }
                if (!encontrou) printf("\n[Info] Nao existem livros disponiveis.\n");
                printf("----------------------------------------------------------------------------------------------------\n");
                esperarEnter();
                break;

            case 2:
                submenuPesquisaLivros(users, totalUsers, books, *totalBooks, idLogado);
                break;

            case 3: // === REQUISITAR LIVRO (Completo) ===
                limparEcra();
                printf("--- REQUISITAR LIVRO ---\n");
                printf("%-3s | %-30s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
                printf("----------------------------------------------------------------------------------------------------\n");

                int cVisual = 1;
                int disponiveis = 0;

                // 1. LISTAGEM
                for (int i = 0; i < *totalBooks; i++) {
                    int souDono = (books[i].idProprietario == idLogado) || (idLogado == 1 && books[i].idProprietario == 0);
                    
                    if (books[i].eliminado == 0 && books[i].estado == LIVRO_DISPONIVEL && !souDono) { 
                        char nomeDono[MAX_STRING];
                        if (books[i].idProprietario <= 1) strcpy(nomeDono, "Instituicao (IPCA)");
                        else {
                            strcpy(nomeDono, "Desconhecido");
                            for(int u = 0; u < totalUsers; u++) if(users[u].id == books[i].idProprietario) { strcpy(nomeDono, users[u].nome); break; }
                        }

                        printf("%-3d | %-30.30s | %-20.20s | %-15.15s | %-20.20s\n", 
                                cVisual, books[i].titulo, books[i].autor, 
                                obterNomeVisualCategoria(&books[i]), nomeDono);
                        cVisual++;
                        disponiveis++;
                    }
                }
                printf("----------------------------------------------------------------------------------------------------\n");

                if (disponiveis > 0) {
                    int escolha = lerInteiro("Escolha o N. do livro (0 para cancelar): ", 0, disponiveis);

                    if (escolha != 0) {
                        // 2. RECUPERAR ID REAL
                        int busca = 1;
                        int idx = -1;
                        for (int i = 0; i < *totalBooks; i++) {
                            int souDono = (books[i].idProprietario == idLogado) || (idLogado == 1 && books[i].idProprietario == 0);
                            if (books[i].eliminado == 0 && books[i].estado == LIVRO_DISPONIVEL && !souDono) {
                                if (busca == escolha) { idx = i; break; }
                                busca++;
                            }
                        }

                        if (idx != -1) {
                            // --- A. DONO É O IPCA -> EMPRÉSTIMO ---
                            if (books[idx].idProprietario <= 1) {
                                printf("\n[Pedido de Emprestimo a Instituicao]\n");
                                int dias = lerInteiro("Dias de requisicao (1-30): ", 1, 30);
                                
                                // Criar Operação
                                operacoes[*totalOperacoes].id = *totalOperacoes + 1;
                                operacoes[*totalOperacoes].idRequerente = idLogado;
                                operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                                operacoes[*totalOperacoes].idLivro = books[idx].id;
                                operacoes[*totalOperacoes].tipo = OP_TIPO_EMPRESTIMO;
                                operacoes[*totalOperacoes].dias = dias;
                                operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                                operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                                (*totalOperacoes)++;

                                // Ocultar o livro
                                books[idx].estado = LIVRO_RESERVADO;
                                printf("[Sucesso] Pedido de emprestimo registado.\n");
                            } 
                            // --- B. DONO É OUTRO USER -> TROCA ---
                            else {
                                printf("\n[Pedido de Troca]\n");
                                printf("Para trocar, tem de oferecer um livro seu.\n");
                                
                                // Sub-listagem dos MEUS livros
                                int meusIds[50];
                                int qtdMeus = 0;
                                printf("\n--- SEUS LIVROS DISPONIVEIS ---\n");
                                for(int k=0; k < *totalBooks; k++) {
                                    if(books[k].idProprietario == idLogado && books[k].estado == LIVRO_DISPONIVEL) {
                                        printf("%d. %s\n", qtdMeus + 1, books[k].titulo);
                                        meusIds[qtdMeus] = k;
                                        qtdMeus++;
                                    }
                                }

                                if (qtdMeus == 0) {
                                    printf("[Erro] Nao tem livros disponiveis para oferecer.\n");
                                } else {
                                    int escTroca = lerInteiro("Qual oferece (0 cancelar): ", 0, qtdMeus);
                                    if (escTroca > 0) {
                                        int idxMeu = meusIds[escTroca - 1];

                                        // Criar Operação de Troca
                                        operacoes[*totalOperacoes].id = *totalOperacoes + 1;
                                        operacoes[*totalOperacoes].idRequerente = idLogado;
                                        operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                                        operacoes[*totalOperacoes].idLivro = books[idx].id;      // Quero este
                                        operacoes[*totalOperacoes].idLivroTroca = books[idxMeu].id; // Dou este
                                        operacoes[*totalOperacoes].tipo = OP_TIPO_TROCA;
                                        operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                                        operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                                        (*totalOperacoes)++;

                                        // Ocultar AMBOS os livros
                                        books[idx].estado = LIVRO_RESERVADO;
                                        books[idxMeu].estado = LIVRO_RESERVADO;

                                        printf("[Sucesso] Proposta de troca enviada!\n");
                                    }
                                }
                            }
                        }
                    }
                } else {
                    printf("[Info] Nao ha livros disponiveis.\n");
                }
                esperarEnter();
                break;

            case 4:
                doarLivro(books, *totalBooks, idLogado, operacoes, totalOperacoes);
                guardarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while (opMercado != 0);
}

// --- MENU MEUS LIVROS ---

void menuMeusLivros(Livro books[], int *totalBooks, int idLogado) {
    int opcao;
    do {
        limparEcra();
        printf("\n=== MEUS LIVROS ===\n");
        
        // --- LISTAGEM ---
        printf("%-3s | %-30s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "ESTADO");
        printf("------------------------------------------------------------------------------------------\n");

        int visualId = 1;
        int temLivros = 0;

        for (int i = 0; i < *totalBooks; i++) {
            if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
                
                char estadoStr[20];
                if (books[i].estado == LIVRO_DISPONIVEL) strcpy(estadoStr, "Disponivel");
                else if (books[i].estado == LIVRO_EMPRESTADO) strcpy(estadoStr, "Emprestado");
                else if (books[i].estado == LIVRO_RESERVADO) strcpy(estadoStr, "Reservado");
                else strcpy(estadoStr, "Indisponivel");

                // Uso da nova função auxiliar para obter o nome da categoria
                printf("%-3d | %-30.30s | %-20.20s | %-15.15s | %s\n", 
                       visualId, 
                       books[i].titulo, 
                       books[i].autor, 
                       obterNomeCategoria(books[i].categoria, books[i].categoriaManual), 
                       estadoStr);
                
                visualId++;
                temLivros = 1;
            }
        }
        
        if (!temLivros) printf("Nenhum livro registado.\n");
        printf("------------------------------------------------------------------------------------------\n");

        printf("\n1. Registar Novo Livro\n");
        printf("2. Editar Livro\n");
        printf("3. Eliminar Livro\n");
        printf("0. Voltar\n");
        
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                criarLivro(books, totalBooks, idLogado);
                break;

            case 2: // === EDITAR ===
                if (!temLivros) {
                    printf("[Erro] Nao tem livros para editar.\n");
                    esperarEnter();
                } else {
                    int escolha = lerInteiro("N. do livro a editar (0 cancela): ", 0, visualId - 1);
                    
                    if (escolha != 0) {
                        int contador = 1;
                        int idx = -1;
                        for (int i = 0; i < *totalBooks; i++) {
                            if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
                                if (contador == escolha) { idx = i; break; }
                                contador++;
                            }
                        }

                        if (idx != -1) {
                            if (books[idx].estado != LIVRO_DISPONIVEL) {
                                printf("\n[Aviso] Nao pode editar livros Emprestados ou Reservados.\n");
                                esperarEnter();
                            } else {
                                int opEdit;
                                do {
                                    limparEcra();
                                    printf("\n--- EDITAR: '%s' ---\n", books[idx].titulo);
                                    printf("1. Alterar Titulo\n");
                                    printf("2. Alterar Autor\n");
                                    printf("3. Alterar Categoria\n");
                                    printf("0. Concluir\n");
                                    opEdit = lerInteiro("Opcao: ", 0, 3);

                                    switch(opEdit) {
                                        case 1:
                                            printf("Novo Titulo: "); lerString(books[idx].titulo, MAX_STRING);
                                            break;
                                        case 2:
                                            printf("Novo Autor: "); lerString(books[idx].autor, MAX_STRING);
                                            break;
                                        case 3:
                                            // === NOVA SELEÇÃO DE CATEGORIAS ===
                                            limparEcra();
                                            printf("\n--- Selecione a Categoria ---\n");
                                            printf("1. Ficcao\n");
                                            printf("2. Nao Ficcao\n");
                                            printf("3. Ciencia\n");
                                            printf("4. Historia\n");
                                            printf("5. Biografia\n");
                                            printf("6. Tecnologia\n");
                                            printf("7. Outro (Manual)\n");
                                            
                                            int novaCat = lerInteiro("Opcao: ", 1, 7);
                                            
                                            // Cast direto porque os números batem certo com o Enum (1 a 7)
                                            books[idx].categoria = (CategoriaLivro)novaCat;

                                            if (books[idx].categoria == CAT_OUTRO) {
                                                printf("Especifique a Categoria: ");
                                                lerString(books[idx].categoriaManual, 30);
                                                // Se quiseres que volte a aprovação admin:
                                                // books[idx].estado = LIVRO_PENDENTE_CAT; 
                                            } else {
                                                // Limpa o manual se escolheu uma categoria padrão
                                                strcpy(books[idx].categoriaManual, "");
                                            }
                                            printf("[Sucesso] Categoria atualizada.\n");
                                            esperarEnter();
                                            break;
                                    }
                                } while(opEdit != 0);
                            }
                        }
                    }
                }
                break;

            case 3: // === ELIMINAR ===
                if (!temLivros) {
                    printf("[Erro] Nao tem livros para eliminar.\n");
                    esperarEnter();
                } else {
                    int escolha = lerInteiro("N. do livro a ELIMINAR (0 cancela): ", 0, visualId - 1);
                    if (escolha != 0) {
                        int contador = 1;
                        int idx = -1;
                        for (int i = 0; i < *totalBooks; i++) {
                            if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
                                if (contador == escolha) { idx = i; break; }
                                contador++;
                            }
                        }
                        if (idx != -1) {
                            if (books[idx].estado != LIVRO_DISPONIVEL) {
                                printf("\n[Erro] O livro esta emprestado/reservado. Nao pode eliminar.\n");
                            } else {
                                printf("\nTem a certeza que deseja eliminar '%s'? (1-Sim, 0-Nao): ", books[idx].titulo);
                                if (lerInteiro("", 0, 1) == 1) {
                                    books[idx].eliminado = 1;
                                    printf("[Sucesso] Livro removido.\n");
                                }
                            }
                            esperarEnter();
                        }
                    }
                }
                break;
            case 0: break;
        }
    } while (opcao != 0);
}

// --- MENU MOVIMENTOS ---

/**
 * @brief Menu principal para gerir devoluções, aceitar pedidos e dar feedback.
 */
// Certifica-te que esta função recebe 'users' para o histórico funcionar
void menuGestaoMovimentos(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao operacoes[], int *totalOperacoes, Feedback feedbacks[], int *totalFeedbacks, int idLogado) {
    int opMov;

    do {
        limparEcra();
        printf("\n--- GESTAO DE MOVIMENTOS E HISTORICO ---\n");
        printf("1. Ver Pedidos Recebidos (Aceitar/Recusar)\n");
        printf("2. Devolver Livro (Que tenho comigo)\n");
        printf("3. Historico Global de Transacoes\n");
        printf("4. Avaliar Troca/Emprestimo (Pendentes)\n");
        printf("0. Voltar\n");
        
        opMov = lerInteiro("Opcao: ", 0, 4);

        switch (opMov) {
            case 1:
                gerirPedidosPendentes(operacoes, *totalOperacoes, books, *totalBooks, users, totalUsers, idLogado);
                guardarOperacoes(operacoes, *totalOperacoes);
                guardarLivros(books, *totalBooks);
                break;

            case 2:
                submenuDevolverLivro(books, *totalBooks, operacoes, *totalOperacoes, idLogado);
                
                guardarOperacoes(operacoes, *totalOperacoes);
                break;

            case 3:
                submenuHistoricoGlobal(operacoes, *totalOperacoes, books, *totalBooks, users, totalUsers);
                break;

            case 4:
                // Chama a função de avaliação inteligente (exclui avaliados)
                submenuAvaliarTransacoes(operacoes, *totalOperacoes, feedbacks, totalFeedbacks, books, *totalBooks, idLogado);
                
                guardarFeedbacks(feedbacks, *totalFeedbacks);
                break;

            case 0:
                break; // Sai do loop e volta ao menu principal

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while (opMov != 0);
}

void submenuDevolverLivro(Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, int idLogado) {
    limparEcra();
    printf("\n=== DEVOLVER LIVRO ===\n");
    printf("Lista de livros que tem em sua posse:\n");
    printf("%-3s | %-30s | %-20s | %-15s\n", "N.", "TITULO", "AUTOR", "PROPRIETARIO");
    printf("--------------------------------------------------------------------------------\n");

    int visualId = 1;
    int temLivros = 0;

    // 1. Listar livros COMIGO (Detentor) que não são meus
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idDetentor == idLogado && 
            books[i].idProprietario != idLogado && 
            books[i].estado == LIVRO_EMPRESTADO) {
            
            char donoStr[20];
            if (books[i].idProprietario <= 1) strcpy(donoStr, "IPCA");
            else sprintf(donoStr, "ID %d", books[i].idProprietario);

            printf("%-3d | %-30.30s | %-20.20s | %s\n", 
                   visualId, books[i].titulo, books[i].autor, donoStr);
            
            visualId++;
            temLivros = 1;
        }
    }
    printf("--------------------------------------------------------------------------------\n");

    if (!temLivros) {
        printf("[Info] Nao tem livros emprestados para devolver.\n");
        esperarEnter();
        return;
    }

    // 2. Selecionar
    int escolha = lerInteiro("Qual o N. do livro a devolver (0 para cancelar): ", 0, visualId - 1);
    
    if (escolha == 0) return;

    // 3. Encontrar o ID do Livro Real
    int contador = 1;
    int idLivroReal = -1;
    
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idDetentor == idLogado && 
            books[i].idProprietario != idLogado && 
            books[i].estado == LIVRO_EMPRESTADO) {
            
            if (contador == escolha) {
                idLivroReal = books[i].id;
                break;
            }
            contador++;
        }
    }

    if (idLivroReal != -1) {
        // === ALTERAÇÃO CRÍTICA AQUI ===
        // Não usamos adicionarOperacao. Vamos procurar a operação existente!
        
        int encontrouOperacao = 0;
        for (int k = 0; k < totalOperacoes; k++) {
            // Procuramos: O mesmo livro + O mesmo user + Estado EM CURSO
            if (operacoes[k].idLivro == idLivroReal && 
                operacoes[k].idRequerente == idLogado && 
                operacoes[k].estado == ESTADO_OP_PENDENTE) {
                
                // MUDANÇA DE ESTADO
                operacoes[k].estado = ESTADO_OP_DEVOLUCAO_PENDENTE;
                encontrouOperacao = 1;
                
                printf("\n[Sucesso] Devolucao sinalizada!\n");
                printf("Estado do pedido alterado para 'Aguardar Confirmacao'.\n");
                printf("Por favor, entregue o livro ao balcao/proprietario para fechar o processo.\n");
                break;
            }
        }

        if (!encontrouOperacao) {
            printf("\n[Erro] Nao foi possivel encontrar o registo original deste emprestimo.\n");
            printf("Contacte o Administrador.\n");
        }
    }
    esperarEnter();
}

/**
 * @brief Menu para visualizar o histórico global de transações.
 * @param operacoes Array de operações/empréstimos.
 * @param totalOperacoes Total de operações.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param users Array de utilizadores.
 * @param totalUsers Total de utilizadores.
 */
void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers) {
    limparEcra();
    printf("\n=== HISTORICO GLOBAL DE TRANSACOES ===\n");
    printf("%-3s | %-12s | %-12s | %s\n", "N.", "DATA", "TIPO", "DETALHES");
    printf("----------------------------------------------------------------------------------------------------\n");

    if (totalOperacoes == 0) {
        printf("[Info] Sem historico de transacoes.\n");
        esperarEnter();
        return;
    }

    char dataStr[12];     // Para data de início
    char dataFimStr[12];  // Para data final

    // Loop Inverso (Mais recentes primeiro)
    for (int i = totalOperacoes - 1; i >= 0; i--) {
        Operacao op = operacoes[i];

        // Formatar Data da Operação
        formatarData(op.dataEmprestimo, dataStr);

        char detalhes[200] = "";
        char nomeLivro[MAX_STRING] = "Removido";
        char nomeReq[MAX_STRING] = "Desc.";

        // Buscar Nomes
        for (int b = 0; b < totalBooks; b++) {
            if (books[b].id == op.idLivro) {
                strcpy(nomeLivro, books[b].titulo); break;
            }
        }
        for (int u = 0; u < totalUsers; u++) {
            if (users[u].id == op.idRequerente) {
                strcpy(nomeReq, users[u].nome); break;
            }
        }

        // --- FORMATAÇÃO DOS DETALHES ---
        if (op.tipo == OP_TIPO_EMPRESTIMO) {
            // Calcula Data Final
            int dataFimInt = somarDias(op.dataEmprestimo, op.dias);
            formatarData(dataFimInt, dataFimStr);

            sprintf(detalhes, "'%s' -> %s | Ate: %s (%d dias)", 
                    nomeLivro, nomeReq, dataFimStr, op.dias);
        }
        else if (op.tipo == OP_TIPO_DOACAO) {
            sprintf(detalhes, "'%s' | Doador: %s -> IPCA", 
                    nomeLivro, nomeReq);
        }
        else if (op.tipo == OP_TIPO_TROCA) {
            sprintf(detalhes, "'%s' | Novo Dono: %s (Troca)", 
                    nomeLivro, nomeReq);
        }
        else if (op.tipo == OP_TIPO_DEVOLUCAO) {
            sprintf(detalhes, "'%s' | Devolvido por: %s", 
                    nomeLivro, nomeReq);
        }

        // --- IMPRIMIR LINHA ---
        printf("%-3d | %-12s | ", i + 1, dataStr); // Sequencial e Data

        switch(op.tipo) {
            case OP_TIPO_EMPRESTIMO: printf("%-12s | ", "EMPRESTIMO"); break;
            case OP_TIPO_TROCA:      printf("%-12s | ", "TROCA"); break;
            case OP_TIPO_DOACAO:     printf("%-12s | ", "DOACAO"); break;
            case OP_TIPO_DEVOLUCAO:  printf("%-12s | ", "DEVOLUCAO"); break;
            default:                 printf("%-12s | ", "OUTRO"); break;
        }

        printf("%s\n", detalhes);
    }
    
    printf("----------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void submenuAvaliarTransacoes(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int *totalFeeds, Livro books[], int totalBooks, int idLogado) {
    limparEcra();
    printf("\n=== AVALIAR TRANSACOES PENDENTES ===\n");
    printf("%-3s | %-12s | %-30s | %s\n", "N.", "TIPO", "LIVRO", "ESTADO");
    printf("--------------------------------------------------------------------------\n");

    int visualId = 1;
    int pendentes = 0;

    // Loop para listar o que posso avaliar
    for (int i = 0; i < totalOperacoes; i++) {
        
        // FILTRO:
        // 1. A operação tem de estar CONCLUÍDA (já acabou)
        // 2. Eu tenho de estar envolvido (sou o Requerente OU era o Dono/Detentor original?)
        //    (Aqui assumo que avalias se foste o Requerente que recebeu o livro)
        // 3. Ainda NÃO avaliei (jaAvaliou == 0)
        
        if (operacoes[i].estado == ESTADO_OP_CONCLUIDO && 
            operacoes[i].idRequerente == idLogado &&
            jaAvaliou(feeds, *totalFeeds, operacoes[i].id, idLogado) == 0) {

            // Buscar nome do livro
            char titulo[MAX_STRING] = "Desconhecido";
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[i].idLivro) {
                    strcpy(titulo, books[b].titulo); break;
                }
            }

            printf("%-3d | %-12s | %-30.30s | Aguarda Avaliacao\n", 
                   visualId, 
                   (operacoes[i].tipo == OP_TIPO_EMPRESTIMO ? "Emprestimo" : "Troca"), 
                   titulo);
            
            visualId++;
            pendentes++;
        }
    }

    if (pendentes == 0) {
        printf("Nao tem transacoes pendentes de avaliacao.\n");
        printf("--------------------------------------------------------------------------\n");
        esperarEnter();
        return;
    }
    printf("--------------------------------------------------------------------------\n");

    // Seleção
    int escolha = lerInteiro("Escolha a transacao a avaliar (0 para voltar): ", 0, visualId - 1);
    if (escolha == 0) return;

    // Encontrar ID Real
    int cont = 1;
    int idxOp = -1;
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].estado == ESTADO_OP_CONCLUIDO && 
            operacoes[i].idRequerente == idLogado &&
            jaAvaliou(feeds, *totalFeeds, operacoes[i].id, idLogado) == 0) {
            
            if (cont == escolha) {
                idxOp = i;
                break;
            }
            cont++;
        }
    }

    if (idxOp != -1) {
        printf("\n--- Avaliar Transacao ---\n");
        int nota = lerInteiro("Atribua uma nota (1 a 5): ", 1, 5);
        
        char comentario[200];
        printf("Comentario curto (Enter para vazio): ");
        lerString(comentario, 200);

        // CRIAR FEEDBACK
        int novoId = (*totalFeeds) + 1;
        
        // Preencher struct (adaptar aos teus nomes exatos)
        feeds[*totalFeeds].id = novoId;
        feeds[*totalFeeds].idOperacao = operacoes[idxOp].id;
        feeds[*totalFeeds].idAvaliador = idLogado;
        feeds[*totalFeeds].idAvaliado = 0; // <--- AQUI: Precisas de lógica para descobrir quem avalias (o antigo dono?)
                                           // Se for Empréstimo IPCA, avaliado = 1 (Admin).
        feeds[*totalFeeds].nota = nota;
        strcpy(feeds[*totalFeeds].comentario, comentario);
        // data...

        (*totalFeeds)++;
        printf("\n[Sucesso] Avaliacao registada! Esta transacao nao aparecera mais na lista pendente.\n");
        
    }
    esperarEnter();
}


/// --- MENU PERFIL ---

/**
 * @brief Gerencia o perfil do utilizador logado.
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 * @param idLogado Ponteiro para o ID do utilizador logado (pode ser alterado se eliminar conta).
 */
void menuGestaoPerfil(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    // 1. Encontrar o utilizador
    int idx = -1;
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].id == idLogado) {
            idx = i;
            break;
        }
    }

    if (idx == -1) return;

    int opcao;
    do {
        limparEcra();
        
        // === USANDO A NOVA FUNÇÃO ===
        int qtdAvaliacoes = 0;
        // Passamos &qtdAvaliacoes para a função preencher esse valor
        float media = calcularMediaUtilizador(feedbacks, totalFeedbacks, idLogado, &qtdAvaliacoes);
        // ============================

        printf("\n=== O MEU PERFIL ===\n");
        printf("Nome:      %s\n", users[idx].nome);
        printf("Email:     %s\n", users[idx].email);
        printf("Telemovel: %s\n", users[idx].telemovel);
        
        // Agora mostramos os dados calculados pela função
        printf("Avaliacao: %.1f / 5.0 (%d reviews)\n", media, qtdAvaliacoes);
        
        printf("------------------------\n");
        
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1: // === ALTERAR NOME ===
            {
                char novoNome[MAX_STRING];
                printf("\n--- Alterar Nome ---\n");
                printf("Nome Atual: %s\n", users[idx].nome);
                printf("Novo Nome (0 para cancelar): ");
                lerString(novoNome, MAX_STRING);

                // 1. Cancelar
                if (strcmp(novoNome, "0") == 0) {
                    printf("Operacao cancelada.\n");
                    break;
                }

                // 2. Validar Igualdade
                if (strcmp(novoNome, users[idx].nome) == 0) {
                    printf("[Aviso] O novo nome e igual ao atual. Nenhuma alteracao efetuada.\n");
                }
                // 3. Validar Tamanho
                else if (strlen(novoNome) < 3) {
                    printf("[Erro] Nome muito curto (minimo 3 caracteres).\n");
                } 
                else {
                    strcpy(users[idx].nome, novoNome);
                    printf("[Sucesso] Nome atualizado.\n");
                    // guardarUtilizadores(users, totalUsers);
                }
                esperarEnter();
                break;
            }

            case 2: // === ALTERAR PASSWORD ===
            {
                char novaPass[50];
                char confirmPass[50];
                
                printf("\n--- Alterar Password ---\n");
                printf("Digite a Nova Password (0 para cancelar): ");
                lerString(novaPass, 50);

                // 1. Cancelar
                if (strcmp(novaPass, "0") == 0) {
                    printf("Operacao cancelada.\n");
                    break;
                }

                // 2. Validar Igualdade (Não pode ser igual à antiga)
                if (strcmp(novaPass, users[idx].password) == 0) {
                    printf("[Erro] A nova password nao pode ser igual a atual.\n");
                    esperarEnter();
                    break;
                }

                // 3. Validar Tamanho
                if (strlen(novaPass) < 4) {
                    printf("[Erro] A password deve ter pelo menos 4 caracteres.\n");
                    esperarEnter();
                    break;
                }

                // 4. Confirmar Password
                printf("Confirme a Nova Password: ");
                lerString(confirmPass, 50);

                if (strcmp(novaPass, confirmPass) != 0) {
                    printf("[Erro] As passwords nao coincidem.\n");
                } else {
                    strcpy(users[idx].password, novaPass);
                    printf("[Sucesso] Password alterada com sucesso.\n");
                    // guardarUtilizadores(users, totalUsers);
                }
                esperarEnter();
                break;
            }

            case 3: // === ALTERAR TELEMÓVEL ===
            {
                char novoTel[15];
                printf("\n--- Alterar Telemovel ---\n");
                printf("Atual: %s\n", users[idx].telemovel);
                printf("Novo Telemovel (0 para cancelar): ");
                lerString(novoTel, 15);

                // 1. Cancelar
                if (strcmp(novoTel, "0") == 0) {
                    printf("Operacao cancelada.\n");
                    break;
                }

                // 2. Validar Igualdade
                if (strcmp(novoTel, users[idx].telemovel) == 0) {
                    printf("[Aviso] O numero introduzido e igual ao atual.\n");
                }
                // 3. Validar Tamanho/Formato
                else if (strlen(novoTel) < 9) { 
                    printf("[Erro] Numero invalido (minimo 9 digitos).\n");
                } 
                else {
                    strcpy(users[idx].telemovel, novoTel);
                    printf("[Sucesso] Telemovel atualizado.\n");
                    // guardarUtilizadores(users, totalUsers);
                }
                esperarEnter();
                break;
            }

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

void verificarNotificacoes(Operacao operacoes[], int totalOperacoes, int idLogado) {
    int pedidosParaAprovar = 0;
    int respostasNovas = 0;
    int atrasados = 0;
    int dataAtual = obterDataAtual();

    for(int i = 0; i < totalOperacoes; i++) {
        // 1. Pedidos que EU tenho de aprovar (Sou o dono do livro)
        if(operacoes[i].idProprietario == idLogado && operacoes[i].estado == ESTADO_OP_PENDENTE) {
            pedidosParaAprovar++;
        }

        // 2. Respostas a pedidos que EU fiz (Sou o requerente e o dono já aceitou/recusou)
        // Nota: Isto assume que tens um estado "LIDO" ou semelhante para limpar a notificação
        if(operacoes[i].idRequerente == idLogado && operacoes[i].estado == ESTADO_OP_ACEITE) {
            respostasNovas++;
        }

        // 3. Alerta de Atraso (Eu tenho o livro e o prazo passou)
        if(operacoes[i].idRequerente == idLogado && operacoes[i].estado == ESTADO_OP_PENDENTE) {
            // Se a data de entrega prevista for menor que hoje
            if(operacoes[i].dataDevolucaoPrevista < dataAtual) {
                atrasados++;
            }
        }
    }

    // --- MOSTRAR O PAINEL DE ALERTAS ---
    if(pedidosParaAprovar > 0 || respostasNovas > 0 || atrasados > 0) {
        printf("\n==================== NOTIFICACOES ====================\n");
        
        if(pedidosParaAprovar > 0)
            printf(" [!] Tens %d pedido(s) aguardar a tua aprovacao.\n", pedidosParaAprovar);
            
        if(respostasNovas > 0)
            printf(" [i] Tens %d pedido(s) que foram ACEITES recentemente!\n", respostasNovas);
            
        if(atrasados > 0)
            printf(" [X] ATENCAO: Tens %d livro(s) com DEVOLUCAO EM ATRASO!\n", atrasados);
            
        printf("======================================================\n");
        // Não precisamos de esperarEnter() aqui se chamarmos isto dentro do Login 
        // porque o Login já tem um esperarEnter() no final.
    }
}

void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes) {
    int usersPendentes = 0;
    int livrosPendentes = 0;
    int propostasPendentes = 0;

    // 1. Contar Utilizadores Pendentes (Novos + Reativações)
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
            usersPendentes++;
        }
    }

    // 2. Contar Livros com Categoria Pendente
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].estado == LIVRO_PENDENTE_CAT) {
            livrosPendentes++;
        }
    }

    // 3. Contar Propostas para o IPCA (Onde o Admin/ID 1 é o proprietário e ainda não respondeu)
    // Assumindo que o ID 1 é o Admin/Instituição
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].idProprietario == 1 && operacoes[i].estado == ESTADO_OP_PENDENTE) {
            propostasPendentes++;
        }
    }

    // --- EXIBIR ALERTAS ---
    if (usersPendentes > 0 || livrosPendentes > 0 || propostasPendentes > 0) {
        printf("\n================= ALERTAS ADMINISTRADOR =================\n");
        if (usersPendentes > 0) 
            printf(" [!] %d Utilizador(es) aguardam aprovacao.\n", usersPendentes);
        
        if (livrosPendentes > 0) 
            printf(" [!] %d Livro(s) com categoria por validar.\n", livrosPendentes);
            
        if (propostasPendentes > 0) 
            printf(" [!] %d Proposta(s) de requisicao ao IPCA pendentes.\n", propostasPendentes);
        printf("=========================================================\n");
        esperarEnter();
    }
}