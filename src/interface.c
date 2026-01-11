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
int menuModoVisitante(Utilizador users[], int *totalUsers) {
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
            //guardarUtilizadores(users, *totalUsers); 
            esperarEnter();
            return -1; // Continua visitante
        
        case 2: {
            int resultado = loginUtilizador(users, *totalUsers);
            if (resultado >= 0) {
                // Login com sucesso! Retornamos o ID
                return resultado; 
            } 
            else {
                if (resultado == -1) printf("\n[Erro] Email nao encontrado.\n");
                else if (resultado == -2) printf("\n[Erro] Password incorreta.\n");
                else if (resultado == -3) printf("\n[Aviso] Conta inativa.\n");
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

void submenuValidacoes(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao loans[], int totalLoans) {
    int opVal;
    do {
        limparEcra();
        printf("\n=== GESTAO DE VALIDACOES ===\n");
        printf("1. Validar Utilizadores (Novos/Recuperacao)\n");
        printf("2. Validar Livros (Cat. 'Outros')\n");
        printf("3. Validar Propostas Pendentes\n");
        printf("0. Voltar\n");
        
        opVal = lerInteiro("Opcao: ", 0, 3);

        switch(opVal) {
            case 1: 
                // Chama a lógica de validar utilizadores
                // Nota: Precisas de ter esta lógica implementada no users.c ou aqui mesmo.
                // Vou colocar aqui a lógica direta para garantir que as variáveis 'users' são usadas.
                {
                    limparEcra();
                    printf("\n--- VALIDAR UTILIZADORES ---\n");
                    int pendentes = 0;
                    printf("%-4s | %-20s | %s\n", "ID", "NOME", "TIPO");
                    
                    for(int i=0; i<totalUsers; i++) {
                        if(users[i].ativo == PENDENTE_APROVACAO || users[i].ativo == PENDENTE_REATIVACAO) {
                             printf("%-4d | %-20.20s | %s\n", users[i].id, users[i].nome, 
                                    (users[i].ativo == PENDENTE_APROVACAO) ? "NOVO" : "RECUPERACAO");
                             pendentes++;
                        }
                    }

                    if(pendentes == 0) {
                        printf("Nao existem utilizadores pendentes.\n");
                    } else {
                        int idAlvo = lerInteiro("ID a validar (0 sair): ", 0, 999999);
                        if(idAlvo != 0) {
                            for(int i=0; i<totalUsers; i++) {
                                if(users[i].id == idAlvo) {
                                    printf("1-Aprovar 2-Rejeitar: ");
                                    int dec = lerInteiro("", 1, 2);
                                    if(dec == 1) users[i].ativo = ATIVO;
                                    else users[i].ativo = INATIVO;
                                    break;
                                }
                            }
                        }
                    }
                    esperarEnter();
                }
                break;
            
            case 2:
                // Lógica de validar Livros (usa a variável 'books')
                {
                    limparEcra();
                    printf("\n--- VALIDAR LIVROS ---\n");
                    int lPendentes = 0;
                    for(int i=0; i < totalBooks; i++) {
                        if(books[i].Disponivel == PENDENTE_CATEGORIA) {
                            printf("ID: %d | Titulo: %s | Cat. Proposta: %s\n", 
                                   books[i].id, books[i].titulo, books[i].categoriaManual);
                            lPendentes++;
                        }
                    }
                    
                    if(lPendentes == 0) printf("Sem livros pendentes.\n");
                    else {
                        int idL = lerInteiro("ID Livro a validar (0 sair): ", 0, 999999);
                        if(idL != 0) {
                            for(int i=0; i<totalBooks; i++) {
                                if(books[i].id == idL) {
                                    printf("1-Aprovar 2-Rejeitar: ");
                                    int dec = lerInteiro("", 1, 2);
                                    if(dec == 1) books[i].Disponivel = DISPONIVEL;
                                    else { books[i].retido = 1; books[i].Disponivel = INDISPONIVEL; }
                                    break;
                                }
                            }
                        }
                    }
                    esperarEnter();
                }
                break;

            case 3:
                // Lógica de validar Empréstimos (usa a variável 'loans')
                {
                    limparEcra();
                    printf("\n--- PROPOSTAS PENDENTES ---\n");
                    int count = 0;
                    for(int i=0; i<totalLoans; i++) {
                        if(loans[i].estado == PENDENTE_PROPOSTA) { // Assumindo estado PENDENTE
                             printf("Op ID: %d | User %d pede Livro %d\n", loans[i].id, loans[i].userId, loans[i].bookId);
                             count++;
                        }
                    }
                    if(count == 0) printf("Nenhuma proposta pendente.\n");
                    esperarEnter();
                }
                break;
        }
    } while(opVal != 0);
}

// --- MENU ADMINISTRADOR ---

void menuAdministrador(Utilizador users[], int totalUsers, Livro books[], int totalBooks, Operacao loans[], int totalLoans, Feedback feedbacks[], int totalFeedbacks) {
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
                submenuValidacoes(users, totalUsers, books, totalBooks, loans, totalLoans);
                break;

            case 2:
                // Listar apenas livros onde userId == 0
                limparEcra();
                printf("\n--- STOCK DA INSTITUICAO ---\n");
                int stock = 0;
                for(int i=0; i<totalBooks; i++) {
                    if(books[i].userId == 0) {
                        printf("ID: %d | Titulo: %s | Estado: %d\n", books[i].id, books[i].titulo, books[i].Disponivel);
                        stock++;
                    }
                }
                if(stock == 0) printf("A instituicao nao tem livros.\n");
                esperarEnter();
                break;

            case 3:
                limparEcra();
                printf("\n--- HISTORICO GLOBAL DE TRANSACOES ---\n");
                for(int i=0; i<totalLoans; i++) {
                     printf("Op #%d | De: %d -> Para: %d | Livro: %d | Data: %d | Estado: %d\n", loans[i].id, loans[i].userIdEmprestimo, loans[i].userId, loans[i].bookId, loans[i].dataEmprestimo, loans[i].estado);
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
                    if(opRel == 1) relatorioTopTransacoes(users, totalUsers, loans, totalLoans);
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
 * @brief Gere o sub-menu de pesquisa de livros (por título, autor, categoria).
 * @param books Array de livros.
 * @param totalBooks Total de livros no array.
 */
void submenuPesquisaLivros(Livro books[], int totalBooks) {
    int opPesquisa;
    char buffer[MAX_STRING];

    do {
        limparEcra();
        printf("\n--- PESQUISAR LIVROS ---\n");
        printf("1. Por Titulo\n");
        printf("2. Por Autor\n");
        printf("3. Por Categoria\n");
        printf("0. Voltar\n");
        opPesquisa = lerInteiro("Opcao: ", 0, 3);

        switch (opPesquisa) {
            case 1:
                printf("Titulo a pesquisar: ");
                lerString(buffer, MAX_STRING);
                pesquisarLivroPorTitulo(books, totalBooks, buffer);
                esperarEnter();
                break;

            case 2:
                printf("Autor a pesquisar: ");
                lerString(buffer, MAX_STRING);
                pesquisarLivroPorAutor(books, totalBooks, buffer);
                esperarEnter();
                break;

            case 3:
                printf("Categoria (Ficcao, Tecnico, Romance...): ");
                lerString(buffer, MAX_STRING);
                pesquisarLivroPorCategoria(books, totalBooks, buffer);
                esperarEnter();
                break;

            case 0:
                // Sai do loop e volta ao menu anterior
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while (opPesquisa != 0);
}

// --- MENU MERCADO ---

/**
 * @brief Gere o menu do "Mercado de Livros" (Procurar, Requisitar, Doar).
 * @param books Array de livros.
 * @param totalBooks Apontador para o total de livros (pode mudar se houver doação).
 * @param loans Array de empréstimos.
 * @param totalLoans Apontador para o total de empréstimos (pode mudar se houver pedido).
 * @param idLogado ID do utilizador logado.
 */
void menuMercadoLivros(Utilizador users[], int totalUsers, Livro books[], int *totalBooks, Operacao loans[], int *totalLoans, int idLogado) {
    int opMercado;

    do {
        limparEcra();
        printf("\n--- MERCADO DE LIVROS ---\n");
        printf("1. Ver Catalogo Completo\n");
        printf("2. Pesquisar Livros (Titulo, Autor, Categoria)\n"); // Opção Agrupada
        printf("3. Requisitar Livro (Troca/Levantamento)\n");
        printf("4. Doar Livro a Instituicao\n");
        printf("0. Voltar\n");
        opMercado = lerInteiro("Opcao: ", 0, 4);

        switch (opMercado) {
            case 1:
                listarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 2:
                submenuPesquisaLivros(books, *totalBooks);
                break;

            case 3:
                limparEcra();
                printf("--- REQUISITAR LIVRO ---\n");
                printf("Livros disponiveis na comunidade:\n");
                
                int disponiveis = 0;

                // 1. Cabeçalho da Tabela (Adicionada coluna PROPRIETARIO)
                printf("--------------------------------------------------------------------------------------------\n");
                printf("%-5s | %-30s | %-20s | %-15s | %s\n", "ID", "TITULO", "AUTOR", "CATEGORIA", "PROPRIETARIO");
                printf("--------------------------------------------------------------------------------------------\n");

                // 2. Listagem
                for (int i = 0; i < *totalBooks; i++) {
                    // Filtro: Livro não retido E não é meu
                    if (books[i].retido == 0 && books[i].userId != idLogado) {
                        
                        // Tratamento visual para o Dono
                        char donoStr[MAX_STRING]; // Garante que tens espaço suficiente
                    
                        if (books[i].userId == 0) {
                            strcpy(donoStr, "Instituicao (IPCA)");
                        } else {
                            // Lógica de busca do nome pelo ID
                            strcpy(donoStr, "Desconhecido"); // Valor por defeito caso não encontre
                            
                            for(int u = 0; u < totalUsers; u++) {
                                if(users[u].id == books[i].userId) {
                                    // Copiamos o nome para a variável de exibição
                                    strcpy(donoStr, users[u].nome);
                                    break; // Encontrou, pode parar de procurar
                                }
                            }
                        }

                        // Print com a nova coluna
                        printf("%-5d | %-30.30s | %-20.20s | %-15.15s | %-20.20s\n", 
                                books[i].id, 
                                books[i].titulo, 
                                books[i].autor, 
                                obterNomeCategoria(books[i].categoria),
                                donoStr);
                        
                        disponiveis++;
                    }
                }
                printf("--------------------------------------------------------------------------------------------\n");

                if (disponiveis > 0) {
                    int idAlvo = lerInteiro("ID do Livro a requisitar (0 cancelar): ", 0, 999999);

                    if (idAlvo != 0) {
                        int idx = -1;
                        for(int i = 0; i < *totalBooks; i++) {
                            if(books[i].id == idAlvo) {
                                idx = i;
                                break;
                            }
                        }
                        
                        if (idx != -1) {
                            if (books[idx].userId == idLogado) {
                                printf("[Erro] Nao pode requisitar o seu proprio livro.\n");
                            } else {
                                // Solicitar Empréstimo
                                solicitarEmprestimo(loans, totalLoans, &books[idx], idLogado);
                                guardarEmprestimos(loans, *totalLoans);
                            }
                        } else {
                            printf("[Erro] ID invalido.\n");
                        }
                    }
                } else {
                    printf("[Info] Nao existem livros disponiveis para troca.\n");
                }
                esperarEnter();
                break;

            case 4:
                doarLivro(books, *totalBooks, idLogado, loans, totalLoans);
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

/**
 * @brief Gere o menu de "Meus Livros" (Registar, Editar, Eliminar).
 * @param books Array de livros.
 * @param totalBooks Apontador para o total de livros (pode mudar se houver registo/eliminacao).
 * @param idLogado ID do utilizador logado.
 */
void menuMeusLivros(Livro books[], int *totalBooks, int idLogado) {
    int opMeus;
    
    do {
        limparEcra();
        printf("\n--- GESTAO DE INVENTARIO (MEUS LIVROS) ---\n");
        printf("1. Listar Meus Livros\n");
        printf("2. Registar Novo Livro\n");
        printf("3. Editar Livro\n");
        printf("4. Eliminar Livro\n");
        printf("0. Voltar\n");
        opMeus = lerInteiro("Opcao: ", 0, 4);

        switch (opMeus) {
            case 1:
                // Se o teu colega não tiver esta função específica, 
                // podes usar a listarLivros e ignorar, ou criamos uma filtrada.
                // Assumindo que existe ou que usamos a lógica de filtro:
                listarMeusLivros(books, *totalBooks, idLogado);
                esperarEnter();
                break;

            case 2:
                // Passamos o endereço do total para ele incrementar lá dentro
                registarLivro(books, *totalBooks, idLogado);
                (*totalBooks)++;
                
                // Guardar logo a seguir
                guardarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 3: // Editar
            case 4: // Eliminar
                listarMeusLivros(books, *totalBooks, idLogado);
                int idEdit = lerInteiro("ID do Livro a editar/eliminar (0 cancelar): ", 0, 999999);

                if (idEdit != 0) {
                    Livro *livroAlvo = NULL;
                    
                    // Procurar o livro E garantir que pertence ao utilizador logado
                    for(int i = 0; i < *totalBooks; i++) {
                        // CORREÇÃO: Usar idDono em vez de userId
                        if(books[i].id == idEdit && books[i].userId == idLogado) {
                            livroAlvo = &books[i];
                            break;
                        }
                    }

                    if (livroAlvo != NULL) {
                        if (opMeus == 3) {
                            editarLivro(livroAlvo, idLogado); // Passar idLogado por segurança
                            printf("[Sucesso] Livro editado.\n");
                        } 
                        else {
                            // Nota: eliminarLivro deve tratar de remover do array ou marcar como inativo
                            eliminarLivro(livroAlvo, idLogado);
                            // Se for remoção física (shift array), precisarias de passar o array e total.
                            // Se for Soft Delete (mudar estado), assim chega.
                        }
                        guardarLivros(books, *totalBooks); // Persistência
                    } else {
                        printf("[Erro] Livro nao encontrado ou nao lhe pertence.\n");
                    }
                }
                esperarEnter();
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }
    } while (opMeus != 0);
}

// --- MENU MOVIMENTOS ---

/**
 * @brief Menu principal para gerir devoluções, aceitar pedidos e dar feedback.
 */
void menuGestaoMovimentos(Operacao loans[], int *totalLoans, Livro books[], int *totalBooks, Feedback feedbacks[], int *totalFeedbacks, int idLogado) {
    int opMov;

    do {
        limparEcra();
        printf("\n--- GESTAO DE MOVIMENTOS ---\n");
        printf("1. Ver Pedidos Pendentes (Aceitar/Recusar)\n");
        printf("2. Devolver Livro (Emprestimos)\n");
        printf("3. Historico Global\n");
        printf("4. Avaliar Troca/Emprestimo (Feedback)\n");
        printf("0. Voltar\n");
        opMov = lerInteiro("Opcao: ", 0, 4);

        switch (opMov) {
            case 1:
                // Gere os pedidos onde eu sou o DONO do livro
                // Nota: Assumimos que gerirPedidosPendentes foi feita pelo teu colega
                // e altera o estado do emprestimo e o estado 'retido' do livro.
                gerirPedidosPendentes(loans, *totalLoans, books, *totalBooks, idLogado);
                
                // Guardar tudo porque os estados mudaram
                guardarEmprestimos(loans, *totalLoans);
                guardarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 2:
                printf("--- DEVOLVER LIVRO ---\n");
                int idDev = lerInteiro("ID do Livro a devolver: ", 0, 999999);
                
                Livro *livroDev = NULL;
                // Procura o livro pelo ID
                for(int i = 0; i < *totalBooks; i++) {
                    if(books[i].id == idDev) {
                        livroDev = &books[i];
                        break;
                    }
                }
                
                if(livroDev != NULL) {
                    // Chama a função de devolução (assumindo que o colega a fez)
                    // Passamos *totalLoans (valor) ou totalLoans (ponteiro) dependendo da implementação dele.
                    // Vou assumir que ele pediu o valor int simples:
                    devolverLivro(livroDev, loans, *totalLoans, idLogado);
                    
                    // Guardar alterações
                    guardarEmprestimos(loans, *totalLoans);
                    guardarLivros(books, *totalBooks);
                } else {
                    printf("[Erro] Livro nao encontrado com esse ID.\n");
                }
                esperarEnter();
                break;

            case 3:
                // Reutilizamos a função que já corrigimos antes
                listarEmprestimos(loans, *totalLoans, idLogado);
                esperarEnter();
                break;

            case 4:
                printf("--- AVALIAR / DAR FEEDBACK ---\n");
                // É boa prática listar primeiro o que se pode avaliar
                listarEmprestimos(loans, *totalLoans, idLogado); 
                
                int idLoan = lerInteiro("ID da Operacao (Pedido #) a avaliar: ", 0, 999999);
                
                Operacao *loanAlvo = NULL;
                for(int i = 0; i < *totalLoans; i++) {
                    if(loans[i].id == idLoan) {
                        loanAlvo = &loans[i];
                        break;
                    }
                }

                if(loanAlvo != NULL) {
                    // Verifica se o user faz parte deste empréstimo
                    if (loanAlvo->userId == idLogado || loanAlvo->userIdEmprestimo == idLogado) {
                        
                        // Função do colega para avaliar
                        avaliarEmprestimo(feedbacks, totalFeedbacks, loanAlvo, idLogado);
                        
                        // Guardar feedbacks (Se tiveres esta função no files.c)
                        // guardarFeedbacks(feedbacks, *totalFeedbacks); 
                        printf("[Sucesso] Feedback registado.\n");
                    } else {
                        printf("[Erro] Nao tem permissao para avaliar este emprestimo.\n");
                    }
                } else {
                    printf("[Erro] Operacao nao encontrada.\n");
                }
                esperarEnter();
                break;

            case 0:
                break; // Voltar

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while (opMov != 0);
}

/// --- SMENU PERFIL ---

/**
 * @brief Gerencia o perfil do utilizador logado.
 * @param users Array de utilizadores.
 * @param total Número total de utilizadores.
 * @param idLogado Ponteiro para o ID do utilizador logado (pode ser alterado se eliminar conta).
 */
void menuGestaoPerfil(Utilizador users[], int total, int *idLogado) {
    int subOpcao;
    int id = *idLogado;

    do {
        limparEcra();
        printf("\n--- GESTAO DE PERFIL ---\n");
        printf("1. Ver Meus Dados\n");
        printf("2. Editar Dados\n");
        printf("3. Eliminar Conta\n");
        printf("0. Voltar\n");
        subOpcao = lerInteiro("Opcao: ", 0, 4);

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