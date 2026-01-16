#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "interface.h"
#include "users.h"
#include "books.h"
#include "transactions.h"
#include "files.h"
#include "structs.h"

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
            if (resultado >= 0) {                // Login com sucesso! Retornamos o ID
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
                // Lógica de validar Empréstimos/Trocas
                gerirPedidosPendentes(operacoes, totalOperacoes, books, totalBooks, users, totalUsers, 1);

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
        printf("3. Monitorizacao (Historico Global)\n");
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
                    if(books[i].idProprietario == 1) {
                        printf("ID: %d | Titulo: %s | Estado: %d\n", books[i].id, books[i].titulo, books[i].estado);
                        stock++;
                    }
                }
                if(stock == 0) printf("A instituicao nao tem livros.\n");
                esperarEnter();
                break;

            case 3:
                limparEcra();

                submenuHistoricoGlobal(operacoes, totalOperacoes, users, totalUsers, books, totalBooks);

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
                    if(opRel == 1) relatorioTopUtilizadores(users, totalUsers, operacoes, totalOperacoes);
                    else if(opRel == 2) relatorioTopLivros(books, totalBooks);
                    else if(opRel == 3) relatorioTopReputacao(users, totalUsers, feedbacks, totalFeedbacks);
                    
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
                            // Lógica Universal: Serve para IPCA e para Users Normais
                            
                            printf("\n[Opcoes de Requisicao]\n");
                            printf("Livro selecionado: %s\n", books[idx].titulo);
                            printf("1. Pedir Emprestado (Devolucao com prazo)\n");
                            printf("2. Propor Troca (Definitivo, oferece um livro)\n");
                            printf("0. Cancelar\n");

                            int tipoPedido = lerInteiro("Opcao: ", 0, 2);

                            // --- OPÇÃO 1: EMPRÉSTIMO ---
                            if (tipoPedido == 1) {
                                limparEcra();
                                int dias = lerInteiro("Dias de requisicao (1-30): ", 1, 30);

                                // Registar Operação
                                operacoes[*totalOperacoes].id = *totalOperacoes + 1;
                                operacoes[*totalOperacoes].idRequerente = idLogado;
                                operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                                operacoes[*totalOperacoes].idLivro = books[idx].id;
                                operacoes[*totalOperacoes].idLivroTroca = 0; // Sem troca
                                
                                operacoes[*totalOperacoes].tipo = OP_TIPO_EMPRESTIMO; // Certifica-te que tens este ENUM
                                operacoes[*totalOperacoes].dias = dias;
                                operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                                operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                                (*totalOperacoes)++;

                                // Ocultar o livro do mercado
                                books[idx].estado = LIVRO_RESERVADO;

                                printf("[Sucesso] Pedido de emprestimo registado! Aguarde aprovacao.\n");
                            } 
                            
                            // --- OPÇÃO 2: TROCA ---
                            else if (tipoPedido == 2) {
                                limparEcra();
                                printf("\n[Pedido de Troca]\n");
                                printf("Para trocar, tem de oferecer um livro seu em troca.\n");
                                
                                // Sub-listagem dos MEUS livros
                                int meusIds[50];
                                int qtdMeus = 0;
                                printf("\n--- SEUS LIVROS DISPONIVEIS ---\n");
                                for(int k=0; k < *totalBooks; k++) {
                                    
                                    if(books[k].idProprietario == idLogado && books[k].estado == LIVRO_DISPONIVEL && books[k].eliminado == 0) {
                                        
                                        printf("%d. %s\n", qtdMeus + 1, books[k].titulo);
                                        meusIds[qtdMeus] = k;
                                        qtdMeus++;
                                    }
                                }

                                if (qtdMeus == 0) {
                                    printf("[Erro] Nao tem livros disponiveis para dar em troca.\n");
                                } else {
                                    int escTroca = lerInteiro("Qual livro oferece (0 cancelar): ", 0, qtdMeus);
                                    if (escTroca > 0) {
                                        int idxMeu = meusIds[escTroca - 1];

                                        // Criar Operação de Troca
                                        operacoes[*totalOperacoes].id = *totalOperacoes + 1;
                                        operacoes[*totalOperacoes].idRequerente = idLogado;
                                        operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                                        operacoes[*totalOperacoes].idLivro = books[idx].id;         // O que eu quero
                                        operacoes[*totalOperacoes].idLivroTroca = books[idxMeu].id; // O que eu dou
                                        
                                        operacoes[*totalOperacoes].tipo = OP_TIPO_TROCA; // Certifica-te que tens este ENUM
                                        operacoes[*totalOperacoes].dias = 0; // Troca não tem dias
                                        operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                                        operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                                        (*totalOperacoes)++;

                                        // Ocultar AMBOS os livros (o que quero e o que dou)
                                        books[idx].estado = LIVRO_RESERVADO;
                                        books[idxMeu].estado = LIVRO_RESERVADO;

                                        printf("[Sucesso] Proposta de troca enviada! Aguarde aprovacao.\n");
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
            case 0: 
                break;
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
        printf("3. Historico de Transacoes\n");
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
                submenuHistoricoPessoal(operacoes, *totalOperacoes, users, totalUsers, books, *totalBooks, idLogado);                
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

    // 1. Listar livros COMIGO
    for (int i = 0; i < totalBooks; i++) {
        // Verifica se o livro está comigo e não é meu
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

    // 3. Recuperar ID Real e Índice do Array
    int contador = 1;
    int idLivroReal = -1;
    int idxLivroArray = -1; // Precisamos disto para alterar o estado do livro
    
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idDetentor == idLogado && 
            books[i].idProprietario != idLogado && 
            books[i].estado == LIVRO_EMPRESTADO) {
            
            if (contador == escolha) {
                idLivroReal = books[i].id;
                idxLivroArray = i;
                break;
            }
            contador++;
        }
    }

    if (idLivroReal != -1 && idxLivroArray != -1) {
        
        int encontrouOperacao = 0;

        for (int k = 0; k < totalOperacoes; k++) {
            // CORREÇÃO 1: Verificar se é o livro certo e o utilizador certo
            if (operacoes[k].idLivro == idLivroReal && 
                operacoes[k].idRequerente == idLogado) {
                
                // CORREÇÃO 2: Verificar se está ATIVO (Empréstimo a decorrer)
                // Adicionei 'PENDENTE' apenas por segurança caso haja dados antigos errados
                if (operacoes[k].estado == ESTADO_OP_ACEITE || operacoes[k].estado == ESTADO_OP_PENDENTE) {
                    
                    // === AÇÃO DE DEVOLUÇÃO ===
                    
                    // 1. Atualizar a Operação (Fica Concluída)
                    operacoes[k].estado = ESTADO_OP_CONCLUIDO;
                    operacoes[k].dataDevolucaoReal = obterDataAtual();
                    operacoes[k].dataFecho = obterDataAtual();

                    // 2. Atualizar o Livro (Volta para o Dono e fica Disponível)
                    books[idxLivroArray].estado = LIVRO_DISPONIVEL;
                    books[idxLivroArray].idDetentor = books[idxLivroArray].idProprietario;

                    encontrouOperacao = 1;
                    
                    printf("\n[Sucesso] Livro devolvido e operacao fechada.\n");
                    printf("O livro '%s' esta agora disponivel novamente.\n", books[idxLivroArray].titulo);
                    
                    // Gravar alterações para não perder dados
                    guardarLivros(books, totalBooks);
                    guardarOperacoes(operacoes, totalOperacoes); // Se tiveres esta função, chama-a aqui
                    break;
                }
            }
        }

        if (!encontrouOperacao) {
            printf("\n[Erro] O livro esta consigo, mas nao encontrei o registo do emprestimo ATIVO.\n");
            
            // CORREÇÃO DE SEGURANÇA:
            // Se o sistema falhou a encontrar a operação, libertamos o livro na mesma
            // para não ficar preso na tua conta para sempre.
            printf("[Sistema] A forcar a devolucao manual do livro...\n");
            books[idxLivroArray].estado = LIVRO_DISPONIVEL;
            books[idxLivroArray].idDetentor = books[idxLivroArray].idProprietario;
            guardarLivros(books, totalBooks);
            printf("[Sucesso] Livro devolvido manualmente.\n");
        }
    }
    esperarEnter();
}

void submenuHistoricoPessoal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado) {
    limparEcra();
    printf("\n========================= O MEU HISTORICO PESSOAL ==========================\n");
    // Mesma estrutura visual do Global
    printf("%-4s | %-12s | %-30s | %-20s | %-14s | %-10s\n", "N.", "TIPO", "FLUXO (DE -> PARA)", "LIVRO", "ESTADO", "DATA");
    printf("-----------------------------------------------------------------------------------------------------------------\n");

    int encontrou = 0;
    int visualID = 1;

    // LOOP INVERTIDO: Do mais recente para o mais antigo
    for(int i = totalOperacoes - 1; i >= 0; i--) {

        // === O FILTRO MÁGICO ===
        // Só mostra se EU (idLogado) estiver envolvido como Requerente OU Proprietário
        if (operacoes[i].idRequerente != idLogado && operacoes[i].idProprietario != idLogado) {
            continue; // Salta esta operação se não for minha
        }

        // 1. Variáveis Auxiliares
        int idDe, idPara;
        int dataRaw;
        char dataFormatada[12];
        char tipoStr[20], estadoStr[20];
        char nomeDe[50] = "Desc.", nomePara[50] = "Desc.", titulo[50] = "Desc.";

        // 2. Lógica de Tipo e Inversão
        if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
            if (operacoes[i].estado == ESTADO_OP_CONCLUIDO) {
                strcpy(tipoStr, "Devolucao");
                idDe = operacoes[i].idRequerente;
                idPara = operacoes[i].idProprietario;
                dataRaw = operacoes[i].dataDevolucaoReal; 
            } else {
                strcpy(tipoStr, "Emprestimo");
                idDe = operacoes[i].idProprietario;
                idPara = operacoes[i].idRequerente;
                dataRaw = operacoes[i].dataPedido;
            }
        }
        else if (operacoes[i].tipo == OP_TIPO_TROCA) {
            strcpy(tipoStr, "Troca");
            idDe = operacoes[i].idProprietario;
            idPara = operacoes[i].idRequerente;
            dataRaw = (operacoes[i].estado == ESTADO_OP_CONCLUIDO) ? operacoes[i].dataFecho : operacoes[i].dataPedido;
        }
        else if (operacoes[i].tipo == OP_TIPO_DOACAO) {
            strcpy(tipoStr, "Doacao");
            idDe = operacoes[i].idRequerente; 
            idPara = 1; // IPCA
            dataRaw = operacoes[i].dataPedido;
        }
        else {
            strcpy(tipoStr, "Outro");
            idDe = operacoes[i].idProprietario;
            idPara = operacoes[i].idRequerente;
            dataRaw = operacoes[i].dataPedido;
        }

        // 3. Formatar Data
        int dia = dataRaw % 100;
        int mes = (dataRaw % 10000) / 100;
        int ano = dataRaw / 10000;
        sprintf(dataFormatada, "%02d/%02d/%04d", dia, mes, ano);

        // 4. Converter Estado
        switch(operacoes[i].estado) {
            case ESTADO_OP_PENDENTE:           strcpy(estadoStr, "Pendente"); break;
            case ESTADO_OP_ACEITE:             strcpy(estadoStr, "Em Curso"); break;
            case ESTADO_OP_CONCLUIDO:          strcpy(estadoStr, "Concluido"); break;
            case ESTADO_OP_REJEITADO:          strcpy(estadoStr, "Rejeitado"); break;
            case ESTADO_OP_DEVOLUCAO_PENDENTE: strcpy(estadoStr, "Dev. Pendente"); break;
            default:                           strcpy(estadoStr, "Outro");
        }

        // 5. Buscar Nomes (Para ficar bonito na tabela)
        // Nome DE
        if (idDe <= 1) strcpy(nomeDe, "IPCA");
        else if (idDe == idLogado) strcpy(nomeDe, "EU"); // Opcional: Mostra "EU" em vez do nome
        else {
            for(int u=0; u < totalUsers; u++) if(users[u].id == idDe) { 
                strncpy(nomeDe, users[u].nome, 12); nomeDe[12]='\0'; break; 
            }
        }

        // Nome PARA
        if (idPara <= 1) strcpy(nomePara, "IPCA");
        else if (idPara == idLogado) strcpy(nomePara, "EU"); // Opcional: Mostra "EU" em vez do nome
        else {
            for(int u=0; u < totalUsers; u++) if(users[u].id == idPara) { 
                strncpy(nomePara, users[u].nome, 12); nomePara[12]='\0'; break; 
            }
        }

        // Título Livro
        for(int b=0; b < totalBooks; b++) {
            if(books[b].id == operacoes[i].idLivro) { 
                strncpy(titulo, books[b].titulo, 19); titulo[19] = '\0'; break; 
            }
        }

        // 6. Imprimir
        char fluxo[60];
        sprintf(fluxo, "%s -> %s", nomeDe, nomePara);

        printf("%-4d | %-12s | %-30.30s | %-20.20s | %-14s | %s\n", 
               visualID, tipoStr, fluxo, titulo, estadoStr, dataFormatada); 

        encontrou = 1;
    }

    if (!encontrou) printf("Nao tem movimentos registados.\n");
    
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}


void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, Livro books[], int totalBooks) {
    limparEcra();
    printf("\n========================= LOG GLOBAL DE TRANSACOES =========================\n");
    // Ajustei a coluna ESTADO para 14 espaços para caber "Dev. Pendente" sem desalinhar
    printf("%-4s | %-12s | %-30s | %-20s | %-14s | %-10s\n", "ID", "TIPO", "FLUXO (DE -> PARA)", "LIVRO", "ESTADO", "DATA");
    printf("-----------------------------------------------------------------------------------------------------------------\n");

    int encontrouLog = 0;

    // LOOP INVERTIDO: Do mais recente para o mais antigo
    for(int i = totalOperacoes - 1; i >= 0; i--) {
        
        // --- 1. FILTRO REMOVIDO ---
        // Apagamos o 'if' que ignorava os pendentes. Agora mostra tudo.

        // --- 2. VARIÁVEIS AUXILIARES ---
        int idDe, idPara;
        int dataRaw;            // Data em formato YYYYMMDD
        char dataFormatada[12]; // Data em formato DD/MM/AAAA
        char tipoStr[20], estadoStr[20];
        char nomeDe[50] = "Desc.", nomePara[50] = "Desc.", titulo[50] = "Desc.";

        // --- 3. LÓGICA DE TIPO E IDs ---
        
        // A. EMPRÉSTIMOS
        if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
            if (operacoes[i].estado == ESTADO_OP_CONCLUIDO) {
                strcpy(tipoStr, "Devolucao");
                idDe = operacoes[i].idRequerente;     // O Leitor devolve
                idPara = operacoes[i].idProprietario; // O Dono recebe
                dataRaw = operacoes[i].dataDevolucaoReal; 
            } else {
                strcpy(tipoStr, "Emprestimo");
                idDe = operacoes[i].idProprietario;   // O Dono empresta
                idPara = operacoes[i].idRequerente;   // O Leitor recebe
                dataRaw = operacoes[i].dataPedido;
            }
        }
        // B. TROCAS
        else if (operacoes[i].tipo == OP_TIPO_TROCA) {
            strcpy(tipoStr, "Troca");
            idDe = operacoes[i].idProprietario;
            idPara = operacoes[i].idRequerente;
            dataRaw = (operacoes[i].estado == ESTADO_OP_CONCLUIDO) ? operacoes[i].dataFecho : operacoes[i].dataPedido;
        }
        // C. DOAÇÕES
        else if (operacoes[i].tipo == OP_TIPO_DOACAO) {
            strcpy(tipoStr, "Doacao");
            // Na doação, quem requer a operação é o doador (ex-proprietário)
            idDe = operacoes[i].idRequerente; 
            idPara = 1; // O destino é sempre o IPCA
            dataRaw = operacoes[i].dataPedido;
        }
        else {
            strcpy(tipoStr, "Outro");
            idDe = operacoes[i].idProprietario;
            idPara = operacoes[i].idRequerente;
            dataRaw = operacoes[i].dataPedido;
        }

        // --- 4. FORMATAR DATA (DD/MM/AAAA) ---
        int dia = dataRaw % 100;
        int mes = (dataRaw % 10000) / 100;
        int ano = dataRaw / 10000;
        sprintf(dataFormatada, "%02d/%02d/%04d", dia, mes, ano);

        // --- 5. DEFINIR ESTADO ---
        switch(operacoes[i].estado) {
            case ESTADO_OP_PENDENTE:           strcpy(estadoStr, "Pendente"); break; // Adicionado
            case ESTADO_OP_ACEITE:             strcpy(estadoStr, "Em Curso"); break;
            case ESTADO_OP_CONCLUIDO:          strcpy(estadoStr, "Concluido"); break;
            case ESTADO_OP_REJEITADO:          strcpy(estadoStr, "Rejeitado"); break;
            case ESTADO_OP_DEVOLUCAO_PENDENTE: strcpy(estadoStr, "Dev. Pendente"); break;
            default:                           strcpy(estadoStr, "Outro");
        }

        // --- 6. BUSCAR NOMES ---
        
        // Nome DE
        if (idDe <= 1) strcpy(nomeDe, "IPCA");
        else {
            for(int u=0; u < totalUsers; u++) {
                if(users[u].id == idDe) { 
                    strncpy(nomeDe, users[u].nome, 12); nomeDe[12]='\0'; 
                    break; 
                }
            }
        }

        // Nome PARA
        if (idPara <= 1) strcpy(nomePara, "IPCA");
        else {
            for(int u=0; u < totalUsers; u++) {
                if(users[u].id == idPara) { 
                    strncpy(nomePara, users[u].nome, 12); nomePara[12]='\0'; 
                    break; 
                }
            }
        }

        // Título Livro
        for(int b=0; b < totalBooks; b++) {
            if(books[b].id == operacoes[i].idLivro) { 
                strncpy(titulo, books[b].titulo, 19); titulo[19] = '\0'; 
                break; 
            }
        }

        // --- 7. IMPRIMIR ---
        char fluxo[60];
        sprintf(fluxo, "%s -> %s", nomeDe, nomePara);

        printf("%-4d | %-12s | %-30.30s | %-20.20s | %-14s | %s\n", 
               operacoes[i].id, 
               tipoStr, 
               fluxo, 
               titulo, 
               estadoStr,
               dataFormatada); 

        encontrouLog = 1;
    }

    if (!encontrouLog) printf("Nenhum registo de transacoes encontrado.\n");
    
    printf("-----------------------------------------------------------------------------------------------------------------\n");
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
    int atrasados = 0;
    int dataAtual = obterDataAtual(); // Certifica-te que devolve YYYYMMDD (int)

    for(int i = 0; i < totalOperacoes; i++) {
        
        // 1. Pedidos que EU tenho de aprovar
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            // Se sou o dono OU (Sou Admin e o livro é da instituição ID 0)
            if (operacoes[i].idProprietario == idLogado || (idLogado == 1 && operacoes[i].idProprietario == 0)) {
                pedidosParaAprovar++;
            }
        }

        // 2. Alerta de Atraso (Eu tenho o livro e o prazo passou)
        // O estado tem de ser ACEITE (Empréstimo a decorrer) e não PENDENTE
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado == ESTADO_OP_ACEITE) {
            
            // Verifica se tem data definida e se já passou
            if (operacoes[i].dataDevolucaoPrevista > 0 && operacoes[i].dataDevolucaoPrevista < dataAtual) {
                atrasados++;
            }
        }
    }

    // --- MOSTRAR O PAINEL DE ALERTAS ---
    if(pedidosParaAprovar > 0 || atrasados > 0) {
        printf("\n");
        printf("==================== NOTIFICACOES ====================\n");
        
        if(pedidosParaAprovar > 0)
            printf(" [!] ATENCAO: Tem %d pedido(s) a aguardar a sua aprovacao.\n", pedidosParaAprovar);
            
        if(atrasados > 0)
            printf(" [X] URGENTE: Tem %d livro(s) com DEVOLUCAO EM ATRASO!\n", atrasados);
            
        printf("======================================================\n");

    }
}

void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes) {
    int usersPendentes = 0;
    int livrosPendentes = 0;
    int propostasPendentes = 0;

    // 1. Contar Utilizadores Pendentes
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
            usersPendentes++;
        }
    }

    // 2. Contar Livros com Categoria Pendente (Manual)
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].estado == LIVRO_PENDENTE_CAT) {
            livrosPendentes++;
        }
    }

    // 3. CORREÇÃO AQUI: Contar Propostas para ID 0 (Sistema) E ID 1 (Admin)
    for (int i = 0; i < totalOperacoes; i++) {
        // Verifica se está pendente
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            // Verifica se o dono é o Admin (1) OU o Sistema (0)
            if (operacoes[i].idProprietario <= 1) { 
                propostasPendentes++;
            }
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
        
        // Se quiseres que ele mostre o menu a seguir sem flash, não ponhas esperarEnter aqui
        // porque já tens um no main.c
    } else {
        printf("\n[Admin] Nenhuma pendencia urgente encontrada.\n");
    }
}

