#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "interface.h"
#include "users.h"
#include "books.h"
#include "transactions.h"
#include "files.h"
#include "structs.h"

void exibirLogSistema() {
    limparEcra();
    FILE *f = fopen("data/log_sistema.txt", "r");
    
    if (f == NULL) {
        printf("\n[Aviso] O ficheiro de logs ainda nao foi criado ou esta vazio.\n");
        esperarEnter();
        return;
    }

    printf("\n==================== LOG DE AUDITORIA DO SISTEMA ====================\n");
    printf("%-18s | %-6s | %-15s | %s\n", "DATA E HORA", "ID", "ACAO", "DETALHES");
    printf("---------------------------------------------------------------------\n");

    char linha[256];
    // Lê o ficheiro linha a linha e imprime
    while (fgets(linha, sizeof(linha), f)) {
        printf("%s", linha);
    }
    
    printf("=====================================================================\n");
    fclose(f);
    esperarEnter();
}

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
            registarUtilizador(users, totalUsers);
            guardarUtilizadores(users, *totalUsers); 
            esperarEnter();
            return -1; // Continua visitante
        
        case 2: {
            int resultado = loginUtilizador(users, *totalUsers);
            if (resultado >= 0) {                // Login com sucesso! Retornamos o ID
                return resultado; 
            } 
            else {
                return -1; // Falhou login, continua visitante
            }
        }

        case 3:
            recuperarPassword(users, *totalUsers);
            guardarUtilizadores(users, *totalUsers);
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

void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks) {
    int opVal;
    do {
        limparEcra();
        printf("\n=== GESTAO DE VALIDACOES (ADMIN) ===\n");
        printf("1. Validar Utilizadores (Novos/Recuperacao)\n");
        printf("2. Validar Propostas Pendentes\n");
        printf("0. Voltar\n");
        
        opVal = lerInteiro("Opcao: ", 0, 3);

        switch(opVal) {
            case 1: 
                // Chama a lógica de validar utilizadores
                adminValidarUtilizadores(users, totalUsers);
                
                guardarUtilizadores(users, totalUsers); // Recomendado guardar aqui
                break;
            
            case 2:
                // Lógica de validar Empréstimos/Trocas
                gerirPedidosPendentes(operacoes, &totalOperacoes, books, totalBooks, users, totalUsers, feedbacks, totalFeedbacks, 1);
                guardarOperacoes(operacoes, totalOperacoes); // Recomendado guardar aqui
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
        printf("5. Consultar Log de Auditoria (Sistema)\n"); // ADICIONADO
        printf("0. Sair\n");
        printf("-----------------------------------------------\n");
        
        opcao = lerInteiro("Opcao: ", 0, 5); // Aumentado para 5

        switch(opcao) {
            case 1:
                // Centro de validações onde aprovas as contas CONTA_PENDENTE
                submenuValidacoes(users, *totalUsers, books, *totalBooks, operacoes, *totalOperacoes, feedbacks, *totalFeedbacks); 
                break;

            case 2:
                limparEcra();
                printf("\n=== STOCK DA INSTITUICAO (ADMIN) ===\n");

                // Cabeçalho atualizado para incluir o VisualID (N.)
                printf("%-4s | %-5s | %-30.30s | %-15s | %s\n", "N.", "ID", "TITULO", "ISBN", "ESTADO");
                printf("--------------------------------------------------------------------------------------\n");

                int vId = 1; // VisualID sequencial
                int stock = 0;
                int mapeamentoStock[MAX_LIVROS]; // Para guardar o índice real se quiseres editar/remover depois

                for(int i = 0; i < *totalBooks; i++) {
                    // Admin = ID 1 e apenas livros não eliminados
                    if(books[i].idProprietario == 1 && books[i].eliminado == 0) {
                        
                        char estadoTexto[20];
                        switch(books[i].estado) {
                            case LIVRO_INDISPONIVEL: 
                                strcpy(estadoTexto, "Indisponivel"); 
                                break;
                            case LIVRO_DISPONIVEL:   
                                strcpy(estadoTexto, "Disponivel"); 
                                break;
                            case LIVRO_EMPRESTADO:   
                                strcpy(estadoTexto, "Emprestado"); 
                                break;
                            case LIVRO_RESERVADO:    
                                strcpy(estadoTexto, "Reservado"); 
                                break;
                            default: 
                                strcpy(estadoTexto, "Desconhecido"); 
                                break;
                        }

                        // Imprime o VisualID (vId) e o ID real (books[i].id)
                        printf("%-4d | %-5d | %-30.30s | %-15s | %s\n", 
                                vId, 
                                books[i].id, 
                                books[i].titulo, 
                                books[i].isbn,
                                estadoTexto);
                        
                        mapeamentoStock[vId] = i; // Guarda a posição real
                        vId++;
                        stock++;
                    }
                }

                if(stock == 0) {
                    printf("\n[Info] A instituicao nao tem livros em stock.\n");
                } else {
                    printf("--------------------------------------------------------------------------------------\n");
                    printf("Total em stock: %d livros listados.\n", stock);
                }

                esperarEnter();
                break;

            case 3:
                submenuHistoricoGlobal(operacoes, *totalOperacoes, users, *totalUsers, books, *totalBooks, feedbacks, *totalFeedbacks);
                break;

            case 4: {
                int opRel;
                do {
                    limparEcra();
                    printf("\n--- RELATORIOS E ESTATISTICAS ---\n");
                    printf("1. Top 5 Utilizadores (Mais Transacoes)\n");
                    printf("2. Top 5 Livros (Mais Procurados)\n");
                    printf("3. Top 5 Reputacao (Melhores Avaliacoes)\n");
                    printf("0. Voltar\n");
                    opRel = lerInteiro("Opcao: ", 0, 3);

                    if(opRel == 1) relatorioTopUtilizadores(users, *totalUsers, operacoes, *totalOperacoes);
                    else if(opRel == 2) relatorioTopLivros(books, *totalBooks);
                    else if(opRel == 3) relatorioTopReputacao(users, *totalUsers, feedbacks, *totalFeedbacks);
                    
                } while(opRel != 0);
                break;
            }

            case 5:
                exibirLogSistema(); // A função que lê o log_sistema.txt
                break;

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

void submenuPesquisaLivros(Utilizador users[], int totalUsers, Livro books[], int totalBooks, int idLogado) {
    int opPesquisa;
    char termoOriginal[MAX_STRING], termoBusca[MAX_STRING];

    do {
        limparEcra();
        printf("\n=== CONSULTAR CATALOGO GERAL ===\n");
        printf("1. Por Titulo\n2. Por Autor\n3. Por Categoria\n0. Voltar\n");
        opPesquisa = lerInteiro("Opcao: ", 0, 3);

        if (opPesquisa == 0) return;

        printf("\nTermo a pesquisar (0 para cancelar): ");
        lerString(termoOriginal, MAX_STRING);
        if (strcmp(termoOriginal, "0") == 0) continue; 

        paraMinusculas(termoOriginal, termoBusca);

        limparEcra();
        printf("--- RESULTADOS PARA: '%s' ---\n", termoOriginal);
        // Removemos a coluna "N." pois não haverá seleção por número
        printf("%-25s | %-20s | %-15s | %-15s | %-15s\n", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO", "ESTADO");
        printf("----------------------------------------------------------------------------------------------\n");

        int encontrados = 0;

        for (int i = 0; i < totalBooks; i++) {
            // Filtro: Não mostrar eliminados, mas mostrar livros de outros (mesmo que emprestados)
            if (books[i].eliminado == 1) continue;

            char alvoComparacao[MAX_STRING];
            if (opPesquisa == 1) paraMinusculas(books[i].titulo, alvoComparacao);
            else if (opPesquisa == 2) paraMinusculas(books[i].autor, alvoComparacao);
            else if (opPesquisa == 3) paraMinusculas(books[i].categoria, alvoComparacao);

            if (strstr(alvoComparacao, termoBusca) != NULL) {
                char nomeDono[MAX_STRING] = "IPCA";
                if (books[i].idProprietario != 1) {
                    for (int u = 0; u < totalUsers; u++) {
                        if (users[u].id == books[i].idProprietario) {
                            strncpy(nomeDono, users[u].nome, 14); nomeDono[14] = '\0';
                            break;
                        }
                    }
                }

                // Definir texto do estado para o utilizador saber se vale a pena tentar requisitar
                char txtEstado[20];
                if (books[i].estado == LIVRO_DISPONIVEL) strcpy(txtEstado, "Disponivel");
                else if (books[i].estado == LIVRO_EMPRESTADO) strcpy(txtEstado, "Emprestado");
                else strcpy(txtEstado, "Reservado");

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
        
        esperarEnter();

    } while (opPesquisa != 0);
}

// --- MENU MERCADO ---


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
        opMercado = lerInteiro("Opcao: ", 0, 4);

        switch (opMercado) {
            case 1:
                limparEcra();
                printf("\n--- CATALOGO DO MERCADO ---\n");
                // Cabeçalho ajustado para as colunas que definiste
                printf("%-4s | %-25s | %-20s | %-15s | %s\n", "N.", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
                printf("----------------------------------------------------------------------------------------------------\n");

                int contVis = 1;
                int encontrou = 0;

                for (int i = 0; i < *totalBooks; i++) {
                    // 1. FILTRO DE SEGURANÇA E ESTADO
                    // - O livro tem de estar DISPONIVEL
                    // - O proprietário NÃO pode ser o utilizador logado
                    if (books[i].estado == LIVRO_DISPONIVEL && books[i].idProprietario != idLogado) {
                        
                        // 2. DEFINIR NOME DO PROPRIETÁRIO
                        char nomeDono[MAX_STRING];
                        
                        if (books[i].idProprietario == 1) {
                            strcpy(nomeDono, "Instituicao (IPCA)");
                        } else {
                            strcpy(nomeDono, "Utilizador"); // Fallback
                            for(int u = 0; u < totalUsers; u++) {
                                if(users[u].id == books[i].idProprietario) {
                                    // Copiamos apenas os primeiros 20 caracteres para não estragar a tabela
                                    strncpy(nomeDono, users[u].nome, 20);
                                    nomeDono[20] = '\0';
                                    break;
                                }
                            }
                        }

                        // 3. IMPRIMIR LINHA FORMATADA
                        // Usamos o contVis para o utilizador poder escolher pelo número da lista
                        printf("%-4d | %-25.25s | %-20.20s | %-15.15s | %-20.20s\n", 
                                contVis, 
                                books[i].titulo, 
                                books[i].autor, 
                                books[i].categoria, 
                                nomeDono);
                        
                        contVis++;
                        encontrou = 1;
                    }
                }

                if (!encontrou) {
                    printf("\n[Info] Nao existem livros de outros utilizadores disponiveis no momento.\n");
                }

                printf("----------------------------------------------------------------------------------------------------\n");
                // O esperarEnter() aqui é importante para o utilizador ler a lista antes de avançar
                esperarEnter();
                break;

            case 2:
                submenuPesquisaLivros(users, totalUsers, books, *totalBooks, idLogado);
                break;

            case 3: // === REQUISITAR LIVRO (Completo) ===
                limparEcra();
                printf("--- REQUISITAR LIVRO ---\n");
                printf("%-3s | %-25s | %-15s | %-20s | %s\n", "N.", "TITULO", "CATEGORIA", "PROPRIETARIO", "REPUTACAO");
                printf("----------------------------------------------------------------------------------------------------\n");

                int cVisual = 1;
                int disponiveis = 0;
                int mapeamento[MAX_LIVROS]; 

                // 1. LISTAGEM
                for (int i = 0; i < *totalBooks; i++) {
                    // Proteção contra livros apagados ou não disponíveis
                    if (books[i].eliminado != 0 || books[i].estado != LIVRO_DISPONIVEL) continue;

                    // Verificar se eu sou o dono
                    int souDono = (books[i].idProprietario == idLogado);
                    
                    if (!souDono) { 
                        char nomeDono[MAX_STRING] = "Desconhecido";
                        float mediaDono = 0.0f; // INICIALIZAÇÃO OBRIGATÓRIA A 0.0
                        int qtdAvaliacoes = 0;

                        // 2. LÓGICA DE DONO E REPUTAÇÃO
                        if (books[i].idProprietario == 1) {
                            strcpy(nomeDono, "Instituicao (IPCA)");
                            // Calcula a média do Admin (ID 1)
                            mediaDono = calcularMediaUtilizador(feedbacks, totalFeedbacks, 1, &qtdAvaliacoes);
                        } else {
                            for(int u = 0; u < totalUsers; u++) {
                                if(users[u].id == books[i].idProprietario) { 
                                    strncpy(nomeDono, users[u].nome, 19); 
                                    nomeDono[19] = '\0';
                                    // CALCULAR MÉDIA REAL
                                    mediaDono = calcularMediaUtilizador(feedbacks, totalFeedbacks, users[u].id, &qtdAvaliacoes);
                                    break; 
                                }
                            }
                        }

                        // 3. IMPRESSÃO: %.1f garante que o float é formatado corretamente
                        printf("%-3d | %-25.25s | %-15.15s | %-20.20s | %.1f* (%d)\n", 
                                cVisual, books[i].titulo, books[i].categoria, 
                                nomeDono, mediaDono, qtdAvaliacoes);
                        
                        mapeamento[cVisual] = i; 
                        cVisual++;
                        disponiveis++;
                    }
                }
                printf("----------------------------------------------------------------------------------------------------\n");

                if (disponiveis > 0) {
                    int escolha = lerInteiro("Escolha o N. do livro (0 para cancelar): ", 0, cVisual - 1);

                    if (escolha != 0) {
                        int idx = mapeamento[escolha];

                        printf("\n[Opcoes de Requisicao]\n");
                        printf("Livro selecionado: %s\n", books[idx].titulo);
                        printf("1. Pedir Emprestado\n");
                        printf("2. Propor Troca\n");
                        printf("0. Cancelar\n");

                        int tipoPedido = lerInteiro("Opcao: ", 0, 2);

                        if (tipoPedido == 1) {
                            // Criar operação de Empréstimo
                            operacoes[*totalOperacoes].id = (*totalOperacoes > 0) ? operacoes[*totalOperacoes - 1].id + 1 : 1;
                            operacoes[*totalOperacoes].idRequerente = idLogado;
                            operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                            operacoes[*totalOperacoes].idLivro = books[idx].id;
                            operacoes[*totalOperacoes].idLivroTroca = 0;
                            operacoes[*totalOperacoes].tipo = OP_TIPO_EMPRESTIMO;
                            operacoes[*totalOperacoes].dias = lerInteiro("Dias (1-30): ", 1, 30);
                            operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                            operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                            (*totalOperacoes)++;

                            books[idx].estado = LIVRO_RESERVADO;
                            
                            guardarOperacoes(operacoes, *totalOperacoes);
                            guardarLivros(books, *totalBooks);

                            char detalheOperacao[150];
                            sprintf(detalheOperacao, "Pedido de EMPRESTIMO: Livro ID %d por %d dias", 
                                    books[idx].id, operacoes[*totalOperacoes-1].dias);
                            registarLog(idLogado, "EMPRESTIMO_PEDIDO", detalheOperacao);

                            printf("[Sucesso] Pedido registado!\n");
                        } 
                        else if (tipoPedido == 2) {
                            limparEcra();
                            printf("\n--- PROPOR TROCA ---\n");
                            printf("Selecione qual dos seus livros deseja oferecer em troca:\n\n");
                            
                            printf("%-4s | %-25s | %-20s | %-15s\n", "N.", "TITULO", "AUTOR", "CATEGORIA");
                            printf("----------------------------------------------------------------------------\n");

                            int meusIndices[MAX_LIVROS];
                            int qtdMeus = 0;

                            for(int k=0; k < *totalBooks; k++) {
                                if(books[k].idProprietario == idLogado && books[k].estado == LIVRO_DISPONIVEL && books[k].eliminado == 0) {
                                    printf("%-4d | %-25.25s | %-20.20s | %-15.15s\n", 
                                        qtdMeus + 1, books[k].titulo, books[k].autor, books[k].categoria);
                                    
                                    meusIndices[qtdMeus + 1] = k;
                                    qtdMeus++;
                                }
                            }
                            printf("----------------------------------------------------------------------------\n");

                            if (qtdMeus == 0) {
                                printf("[Erro] Nao tem livros disponiveis para oferecer em troca.\n");
                            } else {
                                int escTroca = lerInteiro("Qual livro oferece (0 para cancelar): ", 0, qtdMeus);
                                
                                if (escTroca > 0) {
                                    int idxMeu = meusIndices[escTroca];
                                    
                                    operacoes[*totalOperacoes].id = (*totalOperacoes > 0) ? operacoes[*totalOperacoes - 1].id + 1 : 1;
                                    operacoes[*totalOperacoes].idRequerente = idLogado;
                                    operacoes[*totalOperacoes].idProprietario = books[idx].idProprietario;
                                    operacoes[*totalOperacoes].idLivro = books[idx].id;         
                                    operacoes[*totalOperacoes].idLivroTroca = books[idxMeu].id; 
                                    operacoes[*totalOperacoes].tipo = OP_TIPO_TROCA;
                                    operacoes[*totalOperacoes].estado = ESTADO_OP_PENDENTE;
                                    operacoes[*totalOperacoes].dataPedido = obterDataAtual();
                                    (*totalOperacoes)++;

                                    books[idx].estado = LIVRO_RESERVADO;
                                    books[idxMeu].estado = LIVRO_RESERVADO;

                                    guardarOperacoes(operacoes, *totalOperacoes);
                                    guardarLivros(books, *totalBooks);
                                    
                                    char detalheOperacao[150];
                                    sprintf(detalheOperacao, "Proposta de TROCA: Ofereceu ID %d pelo ID %d", 
                                            books[idxMeu].id, books[idx].id);
                                    registarLog(idLogado, "TROCA_PROPOSTA", detalheOperacao);

                                    printf("\n[Sucesso] Proposta de troca enviada!\n");
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
        printf("\n=== OS MEUS LIVROS ===\n");
        
        // Cabeçalho ajustado para as larguras das colunas
        printf("%-4s | %-15s | %-25s | %-15s | %-15s | %s\n", "N.", "ISBN", "TITULO", "AUTOR", "CATEGORIA", "ESTADO");
        printf("-----------------------------------------------------------------------------------------------------\n");

        int visualId = 1;
        int temLivros = 0;

        for (int i = 0; i < *totalBooks; i++) {
            // Filtro: Dono logado e não apagado
            if (books[i].idProprietario == idLogado && books[i].eliminado == 0) {
                
                char estadoStr[20];
                // Sincronizado com a tua enum EstadoLivro
                switch(books[i].estado) {
                    case LIVRO_DISPONIVEL:   strcpy(estadoStr, "Disponivel"); break;
                    case LIVRO_EMPRESTADO:   strcpy(estadoStr, "Emprestado"); break;
                    case LIVRO_RESERVADO:    strcpy(estadoStr, "Reservado"); break;
                    case LIVRO_INDISPONIVEL: strcpy(estadoStr, "Indisponivel"); break;
                    default: strcpy(estadoStr, "Desconhecido"); break;
                }

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

        printf("\n1. Registar Novo Livro (via ISBN)\n");
        printf("2. Eliminar Livro do Sistema\n");
        printf("0. Voltar\n");
        
        opcao = lerInteiro("Opcao: ", 0, 2);

        switch (opcao) {
            case 1:
                criarLivro(books, totalBooks, idLogado);
                break;

            case 2:
                if (!temLivros) {
                    printf("[Erro] Nao tem livros para eliminar.\n");
                    esperarEnter();
                } else {
                    int escolha = lerInteiro("Selecione o N. do livro a ELIMINAR (0 cancela): ", 0, visualId - 1);
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
                            // A função eliminarLivro já tem as verificações de detentor/estado
                            if (eliminarLivro(&books[idx], idLogado)) {
                                // IMPORTANTE: Atualizar o ficheiro após a alteração bem-sucedida
                                guardarLivros(books, *totalBooks);
                            }
                            esperarEnter();
                        }
                    }
                }
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
                gerirPedidosPendentes(operacoes, totalOperacoes, books, *totalBooks, users, totalUsers, feedbacks, *totalFeedbacks, idLogado);                
                guardarOperacoes(operacoes, *totalOperacoes); 
                guardarLivros(books, *totalBooks);
                break;

            case 2:
                submenuDevolverLivro(books, *totalBooks, operacoes, totalOperacoes, users, totalUsers, idLogado);                
                guardarOperacoes(operacoes, *totalOperacoes);
                break;

            case 3:
                submenuHistoricoPessoal(operacoes, *totalOperacoes, users, totalUsers, books, *totalBooks, feedbacks, *totalFeedbacks, idLogado); 
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

void submenuDevolverLivro(Livro books[], int totalBooks, Operacao operacoes[], int *totalOperacoes, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n=== DEVOLVER LIVRO ===\n");
    printf("Lista de livros que tem em sua posse:\n");
    printf("%-4s | %-25s | %-20s | %-12s\n", "N.", "TITULO", "PROPRIETARIO", "DATA LIMITE");
    printf("--------------------------------------------------------------------------------\n");

    int visualId = 1;
    int temLivros = 0;
    int mapeamentoLivro[MAX_LIVROS]; 
    int dataAtual = obterDataAtual();

    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idDetentor == idLogado && books[i].idProprietario != idLogado && books[i].estado == LIVRO_EMPRESTADO) {
            
            // 1. Encontrar o NOME do Proprietário
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

            // 2. Encontrar a DATA LIMITE
            char dataLimStr[20] = "n/a"; // Aumentado para 20 para evitar crash no strcat
            for (int k = 0; k < *totalOperacoes; k++) {
                if (operacoes[k].idLivro == books[i].id && operacoes[k].estado == ESTADO_OP_ACEITE) {
                    int dt = operacoes[k].dataDevolucaoPrevista;
                    sprintf(dataLimStr, "%02d/%02d/%04d", dt % 100, (dt % 10000) / 100, dt / 10000);
                    
                    if (dt < dataAtual) strcat(dataLimStr, " [!] ");
                    break;
                }
            }

            printf("%-4d | %-25.25s | %-20.20s | %-12s\n", 
                   visualId, books[i].titulo, donoNome, dataLimStr);
            
            if (visualId < MAX_LIVROS) { // Proteção de limite do array de mapeamento
                mapeamentoLivro[visualId] = i;
                visualId++;
                temLivros = 1;
            }
        }
    }

    if (!temLivros) {
        printf("[Info] Nao tem livros de terceiros para devolver.\n");
        esperarEnter();
        return;
    }

    printf("--------------------------------------------------------------------------------\n");
    int escolha = lerInteiro("Qual o N. do livro a devolver (0 para cancelar): ", 0, visualId - 1);
    if (escolha <= 0) return;

    // SEGURANÇA: Validar se a escolha mapeia para um índice válido
    int idxLivro = mapeamentoLivro[escolha];
    if (idxLivro < 0 || idxLivro >= totalBooks) {
        printf("\n[Erro] Erro de mapeamento de memoria.\n");
        esperarEnter();
        return;
    }

    int idLivroReal = books[idxLivro].id;
    int encontrouOperacao = 0;

    // 3. Fechar a operação
    for (int k = 0; k < *totalOperacoes; k++) {
        if (operacoes[k].idLivro == idLivroReal && operacoes[k].idRequerente == idLogado && operacoes[k].estado == ESTADO_OP_ACEITE) {
            
            operacoes[k].estado = ESTADO_OP_CONCLUIDO;
            operacoes[k].dataDevolucaoReal = obterDataAtual();
            operacoes[k].dataFecho = obterDataAtual();

            books[idxLivro].estado = LIVRO_DISPONIVEL;
            books[idxLivro].idDetentor = books[idxLivro].idProprietario;

            encontrouOperacao = 1;
            
            char detalhe[100];
            sprintf(detalhe, "Livro ID %d devolvido", idLivroReal);
            registarLog(idLogado, "LIVRO_DEVOLVIDO", detalhe);
            break;
        }
    }

    if (!encontrouOperacao) {
        printf("\n[Aviso] Operacao nao encontrada. Sincronizando estados...\n");
        books[idxLivro].estado = LIVRO_DISPONIVEL;
        books[idxLivro].idDetentor = books[idxLivro].idProprietario;
    }

    // 4. Persistência (Garantir que os totais são passados como valores)
    guardarLivros(books, totalBooks);
    guardarOperacoes(operacoes, *totalOperacoes); 

    printf("\n[OK] O livro '%s' foi devolvido com sucesso.\n", books[idxLivro].titulo);
    esperarEnter();
}

void submenuHistoricoPessoal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, 
                             Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    limparEcra();
    printf("\n======================================= O MEU HISTORICO PESSOAL =======================================\n");
    // 1. ALTERAÇÃO NO CABEÇALHO: Adicionada a coluna "NOTA" e "COMENTARIO"
    printf("%-4s | %-10s | %-18s | %-20s | %-10s | %-10s | %-5s | %-20s\n", 
           "N.", "TIPO", "FLUXO (DE->PARA)", "LIVRO", "ESTADO", "DATA", "NOTA", "COMENTARIO");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");

    int encontrou = 0;
    int visualID = 1;

    for(int i = totalOperacoes - 1; i >= 0; i--) {

        if (operacoes[i].idRequerente != idLogado && operacoes[i].idProprietario != idLogado) {
            continue;
        }

        int idDe = 0, idPara = 0, dataRaw = 0;
        char dataFormatada[12], tipoStr[15], estadoStr[15];
        char notaStr[10] = "---", comentarioStr[25] = "---"; // Nova variável para o comentário
        char nomeDe[30] = "Sistema", nomePara[30] = "Sistema", titulo[30] = "Livro Desconhecido";

        // Lógica de tipo
        switch(operacoes[i].tipo) {
            case OP_TIPO_EMPRESTIMO: strcpy(tipoStr, "Emprestimo"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = operacoes[i].dataPedido; break;
            case OP_TIPO_DEVOLUCAO: strcpy(tipoStr, "Devolucao"); idDe = operacoes[i].idRequerente; idPara = operacoes[i].idProprietario; dataRaw = operacoes[i].dataDevolucaoReal; break;
            case OP_TIPO_TROCA: strcpy(tipoStr, "Troca"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = (operacoes[i].estado == ESTADO_OP_CONCLUIDO) ? operacoes[i].dataFecho : operacoes[i].dataPedido; break;
            case OP_TIPO_DOACAO: strcpy(tipoStr, "Doacao"); idDe = operacoes[i].idRequerente; idPara = 1; dataRaw = operacoes[i].dataPedido; break;
            default: strcpy(tipoStr, "Outro"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; dataRaw = operacoes[i].dataPedido;
        }

        // 2. BUSCAR A NOTA E O COMENTÁRIO (Alterado)
        for (int f = 0; f < totalFeedbacks; f++) {
            if (feedbacks[f].idOperacao == operacoes[i].id) {
                sprintf(notaStr, "%.1f*", feedbacks[f].nota);
                // Copiamos o comentário e garantimos que não rebenta a tabela
                strncpy(comentarioStr, feedbacks[f].comentario, 20);
                comentarioStr[20] = '\0';
                break;
            }
        }

        // 3. BUSCAR NOMES E TÍTULO (Mantém o teu código)
        if (idDe == idLogado) strcpy(nomeDe, "EU");
        else if (idDe == 1) strcpy(nomeDe, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idDe) { strncpy(nomeDe, users[u].nome, 8); nomeDe[8]='\0'; break; } }

        if (idPara == idLogado) strcpy(nomePara, "EU");
        else if (idPara == 1) strcpy(nomePara, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idPara) { strncpy(nomePara, users[u].nome, 8); nomePara[8]='\0'; break; } }

        for(int b=0; b < totalBooks; b++) if(books[b].id == operacoes[i].idLivro) { strncpy(titulo, books[b].titulo, 19); titulo[19] = '\0'; break; }

        // 4. ESTADOS E DATA
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

        // 5. IMPRIMIR LINHA (Ajustada para incluir comentário)
        char fluxo[30];
        sprintf(fluxo, "%.7s->%.7s", nomeDe, nomePara); 

        printf("%-4d | %-10s | %-18s | %-20.20s | %-10s | %-10s | %-5s | %-20.20s\n", 
               visualID++, tipoStr, fluxo, titulo, estadoStr, dataFormatada, notaStr, comentarioStr); 

        encontrou = 1;
    }

    if (!encontrou) printf("\n[Info] Ainda nao realizou qualquer transacao.\n");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void submenuHistoricoGlobal(Operacao operacoes[], int totalOperacoes, Utilizador users[], int totalUsers, 
                            Livro books[], int totalBooks, Feedback feedbacks[], int totalFeedbacks) {
    limparEcra();
    printf("\n=============================== LOG GLOBAL DE TRANSACOES E FEEDBACKS ===============================\n");
    // 1. AJUSTE DO CABEÇALHO: Incluímos Nota e Comentário
    printf("%-4s | %-10s | %-20s | %-18s | %-4s | %-20s\n", 
           "ID", "TIPO", "FLUXO (DE->PARA)", "LIVRO", "NOTA", "COMENTARIO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int encontrouLog = 0;

    for(int i = totalOperacoes - 1; i >= 0; i--) {
        int idDe = 0, idPara = 0;
        char tipoStr[15], notaStr[10] = "---", comentarioStr[25] = "---";
        char nomeDe[30] = "Sistema", nomePara[30] = "Sistema", titulo[30] = "Desconhecido";

        // 1. LÓGICA DE TIPO (Igual ao que já tinhas)
        switch(operacoes[i].tipo) {
            case OP_TIPO_EMPRESTIMO: strcpy(tipoStr, "Emprest."); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; break;
            case OP_TIPO_DEVOLUCAO: strcpy(tipoStr, "Devoluc."); idDe = operacoes[i].idRequerente; idPara = operacoes[i].idProprietario; break;
            case OP_TIPO_TROCA: strcpy(tipoStr, "Troca"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente; break;
            case OP_TIPO_DOACAO: strcpy(tipoStr, "Doacao"); idDe = operacoes[i].idRequerente; idPara = 1; break;
            default: strcpy(tipoStr, "Outro"); idDe = operacoes[i].idProprietario; idPara = operacoes[i].idRequerente;
        }

        // 2. BUSCAR FEEDBACK (Nova Lógica para o Admin)
        for (int f = 0; f < totalFeedbacks; f++) {
            if (feedbacks[f].idOperacao == operacoes[i].id) {
                sprintf(notaStr, "%.1f*", feedbacks[f].nota);
                strncpy(comentarioStr, feedbacks[f].comentario, 24);
                comentarioStr[24] = '\0';
                break;
            }
        }

        // 3. BUSCAR NOMES (Otimizado para a tabela)
        if (idDe == 1) strcpy(nomeDe, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idDe) { strncpy(nomeDe, users[u].nome, 8); nomeDe[8] = '\0'; break; } }

        if (idPara == 1) strcpy(nomePara, "IPCA");
        else { for(int u=0; u < totalUsers; u++) if(users[u].id == idPara) { strncpy(nomePara, users[u].nome, 8); nomePara[8] = '\0'; break; } }

        for(int b=0; b < totalBooks; b++) if(books[b].id == operacoes[i].idLivro) { strncpy(titulo, books[b].titulo, 17); titulo[17] = '\0'; break; }

        // 4. IMPRIMIR LINHA
        char fluxo[30];
        sprintf(fluxo, "%.8s->%.8s", nomeDe, nomePara);

        printf("%-4d | %-10s | %-20s | %-18.18s | %-4s | %-20.20s\n", 
               operacoes[i].id, tipoStr, fluxo, titulo, notaStr, comentarioStr); 

        encontrouLog = 1;
    }

    if (!encontrouLog) printf("\n[Info] Sem historico de transacoes.\n");
    printf("----------------------------------------------------------------------------------------------------\n");
    esperarEnter();
}

void submenuAvaliarTransacoes(Operacao operacoes[], int totalOperacoes, Feedback feeds[], int *totalFeeds, Livro books[], int totalBooks, int idLogado) {
    limparEcra();
    printf("\n=== AVALIAR TRANSACOES PENDENTES ===\n");
    printf("%-4s | %-12s | %-30s | %s\n", "N.", "TIPO", "LIVRO", "ESTADO");
    printf("--------------------------------------------------------------------------\n");

    int visualId = 1;
    int pendentes = 0;
    int mapeamentoOp[MAX_OPERACOES];

    for (int i = 0; i < totalOperacoes; i++) {
        int participei = (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado);
        
        if (operacoes[i].estado == ESTADO_OP_CONCLUIDO && participei &&
            jaAvaliou(feeds, *totalFeeds, operacoes[i].id, idLogado) == 0) {

            char titulo[MAX_STRING] = "Livro Desconhecido";
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[i].idLivro) {
                    strncpy(titulo, books[b].titulo, 29); titulo[29] = '\0';
                    break;
                }
            }

            printf("%-4d | %-12s | %-30.30s | Pendente\n", 
                   visualId, 
                   (operacoes[i].tipo == OP_TIPO_EMPRESTIMO ? "Emprestimo" : "Troca"), 
                   titulo);
            
            mapeamentoOp[visualId] = i;
            visualId++;
            pendentes++;
        }
    }

    if (pendentes == 0) {
        printf("[Info] Nao tem transacoes pendentes de avaliacao.\n");
        printf("--------------------------------------------------------------------------\n");
        esperarEnter();
        return;
    }

    printf("--------------------------------------------------------------------------\n");
    int escolha = lerInteiro("Escolha o N. para avaliar (0 para voltar): ", 0, visualId - 1);
    if (escolha == 0) return;

    int idxOp = mapeamentoOp[escolha];

    if (idxOp != -1) {
        printf("\n--- Avaliar Experiencia ---\n");
        float nota = lerFloat("Atribua uma nota (1 a 5): ", 1.0, 5.0);
        
        char comentario[200];
        printf("Comentario (Max 200 chars): ");
        lerString(comentario, 200);

        if (*totalFeeds < MAX_FEEDBACKS) {
            feeds[*totalFeeds].id = (*totalFeeds > 0) ? feeds[*totalFeeds - 1].id + 1 : 1;
            feeds[*totalFeeds].idOperacao = operacoes[idxOp].id;
            feeds[*totalFeeds].idAvaliador = idLogado;
            
            int idAvaliado;
            if (idLogado == operacoes[idxOp].idRequerente) {
                idAvaliado = operacoes[idxOp].idProprietario;
            } else {
                idAvaliado = operacoes[idxOp].idRequerente;
            }
            feeds[*totalFeeds].idAvaliado = idAvaliado;
            
            feeds[*totalFeeds].nota = nota;
            strncpy(feeds[*totalFeeds].comentario, comentario, 199);
            feeds[*totalFeeds].comentario[199] = '\0';
            feeds[*totalFeeds].dataAvaliacao = obterDataAtual();

            // === ATUALIZAÇÃO DO REGISTO DE LOG ===
            char msgLog[250]; // Buffer maior para incluir o comentário se necessário
            if (nota <= 2.5) {
                // Alerta especial no log para notas baixas
                sprintf(msgLog, "ALERTA: Nota BAIXA (%.1f*) dada ao utilizador %d na operacao %d. Comentário: %s", 
                        nota, idAvaliado, operacoes[idxOp].id, comentario);
                registarLog(idLogado, "FEEDBACK_NEGATIVO", msgLog);
            } else {
                sprintf(msgLog, "Nota %.1f* atribuída ao utilizador %d na operacao %d", 
                        nota, idAvaliado, operacoes[idxOp].id);
                registarLog(idLogado, "FEEDBACK_ADD", msgLog);
            }

            (*totalFeeds)++;
            guardarFeedbacks(feeds, *totalFeeds);

            printf("\n[Sucesso] Obrigado! A sua avaliacao ajuda a comunidade.\n");
        } else {
            printf("\n[Erro] Base de dados de feedback cheia.\n");
        }
    }
    esperarEnter();
}

/// --- MENU PERFIL ---

int menuGestaoPerfil(Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    int idx = -1;
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].id == idLogado) {
            idx = i;
            break;
        }
    }

    if (idx == -1) return 0;

    int opcao;
    do {
        limparEcra();
        int qtdAvaliacoes = 0;
        float media = calcularMediaUtilizador(feedbacks, totalFeedbacks, idLogado, &qtdAvaliacoes);

        printf("\n=== O MEU PERFIL ===\n");
        printf("Nome:      %s\n", users[idx].nome);
        printf("Email:     %s\n", users[idx].email);
        printf("Telemovel: %s\n", users[idx].telemovel);
        printf("Avaliacao: %.1f / 5.0 (%d reviews)\n", media, qtdAvaliacoes);
        printf("------------------------\n");
        printf("1. Alterar Nome\n");
        printf("2. Alterar Password\n");
        printf("3. Alterar Telemovel\n");
        printf("4. Eliminar Conta\n");
        printf("0. Voltar\n");
        printf("------------------------\n");
        
        opcao = lerInteiro("Opcao: ", 0, 4);

        switch (opcao) {
            case 1: 
            {
                char novoNome[MAX_STRING];
                printf("\n--- Alterar Nome ---\n");
                printf("Novo Nome (0 para cancelar): ");
                lerString(novoNome, MAX_STRING);

                if (strcmp(novoNome, "0") != 0 && strlen(novoNome) >= 3) {
                    strcpy(users[idx].nome, novoNome);
                    guardarUtilizadores(users, totalUsers); // <--- IMPORTANTE
                    printf("[Sucesso] Nome atualizado.\n");
                } else if (strcmp(novoNome, "0") != 0) {
                    printf("[Erro] Nome invalido ou muito curto.\n");
                }
                esperarEnter();
                break;
            }

            case 2: 
            {
                char novaPass[50], confirmPass[50];
                printf("\n--- Alterar Password ---\n");
                printf("Digite a Nova Password (0 para cancelar): ");
                lerString(novaPass, 50);

                if (strcmp(novaPass, "0") != 0 && strlen(novaPass) >= 4) {
                    printf("Confirme a Nova Password: ");
                    lerString(confirmPass, 50);

                    if (strcmp(novaPass, confirmPass) == 0) {
                        strcpy(users[idx].password, novaPass);
                        guardarUtilizadores(users, totalUsers); // <--- IMPORTANTE
                        printf("[Sucesso] Password alterada.\n");
                    } else {
                        printf("[Erro] As passwords nao coincidem.\n");
                    }
                }
                esperarEnter();
                break;
            }

            case 3: 
            {
                char novoTel[15];
                printf("\n--- Alterar Telemovel ---\n");
                printf("Novo Telemovel (0 para cancelar): ");
                lerString(novoTel, 15);

                if (strcmp(novoTel, "0") != 0 && strlen(novoTel) >= 9) {
                    strcpy(users[idx].telemovel, novoTel);
                    guardarUtilizadores(users, totalUsers); // <--- IMPORTANTE
                    printf("[Sucesso] Telemovel atualizado.\n");
                }
                esperarEnter();
                break;
            }

            case 4: 
            {
                printf("\n!!! ZONA DE PERIGO !!!\n");
                printf("Tem a certeza absoluta? (1-Sim / 0-Nao): ");
                int confirm = lerInteiro("", 0, 1);
                
                if (confirm == 1) {
                    users[idx].estado = CONTA_INATIVA; 
                    guardarUtilizadores(users, totalUsers); // <--- GARANTIR QUE FICA INATIVA NO DISCO
                    
                    printf("\n[Conta Eliminada] A sessao sera terminada.\n");
                    esperarEnter();
                    return 1; // Sai da função e sinaliza expulsão
                }
                break;
            }
        }
    } while (opcao != 0);

    return 0; // Se chegou aqui, o utilizador apenas retrocedeu e continua logado
}

void verificarNotificacoes(Operacao operacoes[], int totalOperacoes, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    int pedidosParaAprovar = 0;
    int atrasados = 0;
    int resultadosRecentes = 0;
    int novasAvaliacoes = 0; // <--- NOVO CONTADOR
    int dataAtual = obterDataAtual();

    for(int i = 0; i < totalOperacoes; i++) {
        
        // 1. Pedidos que EU tenho de aprovar (Dono do livro)
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            if (operacoes[i].idProprietario == idLogado) {
                pedidosParaAprovar++;
            }
        }

        // 2. Alerta de Atraso (Eu tenho o livro de alguém e o prazo passou)
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado == ESTADO_OP_ACEITE) {
            if (operacoes[i].dataDevolucaoPrevista > 0 && operacoes[i].dataDevolucaoPrevista < dataAtual) {
                atrasados++;
            }
        }

        // 3. Resultados de pedidos que EU fiz (Dono aceitou ou rejeitou hoje)
        if (operacoes[i].idRequerente == idLogado && operacoes[i].estado != ESTADO_OP_PENDENTE) {
            if (operacoes[i].dataFecho == dataAtual || operacoes[i].dataEmprestimo == dataAtual) {
                resultadosRecentes++;
            }
        }
    }

    // 4. Novas Avaliações que recebi hoje
    for (int f = 0; f < totalFeedbacks; f++) {
        if (feedbacks[f].idAvaliado == idLogado && feedbacks[f].dataAvaliacao == dataAtual) {
            novasAvaliacoes++;
        }
    }

    // Exibição do Painel
    if(pedidosParaAprovar > 0 || atrasados > 0 || resultadosRecentes > 0 || novasAvaliacoes > 0) {
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
    }
}

void verificarNotificacoesAdmin(Utilizador users[], int totalUsers, Livro books[], int totalBooks, 
                                Operacao operacoes[], int totalOperacoes, 
                                Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    int usersPendentes = 0;
    int propostasPendentes = 0;
    int doacoesPendentes = 0;
    int propostasAceites = 0;   
    int propostasRejeitadas = 0; 
    int novasAvaliacoes = 0;
    int dataAtual = obterDataAtual(); 

    // 1. Contar Utilizadores Pendentes
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].estado == CONTA_PENDENTE_APROVACAO || users[i].estado == CONTA_PENDENTE_REATIVACAO) {
            usersPendentes++;
        }
    }

    // 2. Analisar Operações
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

    // 3. Analisar Novas Avaliações (Recebidas por mim hoje)
    for (int f = 0; f < totalFeedbacks; f++) {
        if (feedbacks[f].idAvaliado == idLogado && feedbacks[f].dataAvaliacao == dataAtual) {
            novasAvaliacoes++;
        }
    }

    // 4. Exibir Painel de Alertas
    if (usersPendentes > 0 || propostasPendentes > 0 || doacoesPendentes > 0 || 
        propostasAceites > 0 || propostasRejeitadas > 0 || novasAvaliacoes > 0) {
        
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
    } else {
        printf("\n[Admin] Nenhuma pendencia urgente encontrada.\n");
    }
}

