#include <stdio.h>
#include <string.h>
#include "interface.h"
#include "utils.h"
#include "files.h" 
#include "users.h"
#include "books.h"
#include "loans.h"
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
    printf("Opcao: ");
    opcao = lerInteiro();

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
    printf("Opcao: ");
    return lerInteiro();
}

// --- SUB-MENU MERCADO ---

/**
 * @brief Gere o menu do Mercado de Livros (Pesquisa, Requisição, Doação).
 * @param books Array de livros.
 * @param totalBooks Apontador para o total de livros (pode mudar se houver doação).
 * @param loans Array de empréstimos.
 * @param totalLoans Apontador para o total de empréstimos.
 * @param idLogado ID do utilizador logado.
 */
void menuMercadoLivros(Livro books[], int *totalBooks, Emprestimo loans[], int *totalLoans, int idLogado) {
    int opMercado;
    char buffer[MAX_STRING]; // Buffer para leituras de texto

    do {
        limparEcra();
        printf("\n--- MERCADO DE LIVROS ---\n");
        printf("1. Ver Catalogo Completo\n");
        printf("2. Pesquisar por Titulo\n");
        printf("3. Pesquisar por Autor\n");
        printf("3. Pesquisar por Categoria\n");
        printf("3. Pesquisar por ISBN\n");
        printf("4. Requisitar Livro (Troca/Levantamento)\n");
        printf("5. Doar Livro a Instituicao\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        opMercado = lerInteiro();

        switch (opMercado) {
            case 1:
                // Usamos *totalBooks porque é um ponteiro
                listarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 2:
                printf("Titulo a pesquisar: ");
                lerString(buffer, MAX_STRING); // Usamos a nossa função segura
                pesquisarLivroPorTitulo(books, *totalBooks, buffer);
                esperarEnter();
                break;

            case 3:
                printf("Autor a pesquisar: ");
                lerString(buffer, MAX_STRING);
                pesquisarLivroPorAutor(books, *totalBooks, buffer);
                esperarEnter();
                break;

            case 4:
                limparEcra(); // Começa com o ecrã limpo
                printf("--- REQUISITAR LIVRO (TROCA) ---\n");
                printf("Livros disponiveis na comunidade (Exclui os seus e os da Instituicao):\n");
                printf("------------------------------------------------------------------------------\n");
                printf("%-5s | %-30s | %-20s\n", "ID", "TITULO", "AUTOR");
                printf("------------------------------------------------------------------------------\n");

                int disponiveis = 0;
                
                // 1. LISTAGEM FILTRADA
                for (int i = 0; i < *totalBooks; i++) {
                    // FILTRO:
                    // - Livro disponivel (retido == 0)
                    // - Nao e meu (userId != idLogado)
                    // - Nao e da instituicao (userId != 0) -> AJUSTA O 0 SE O ID DA INSTITUICAO FOR OUTRO
                    if (books[i].retido == 0 && 
                        books[i].userId != idLogado && 
                        books[i].userId != 0) {
                        
                        printf("%-5d | %-30s | %-20s\n", 
                               books[i].id, 
                               books[i].titulo, 
                               books[i].autor);
                        disponiveis++;
                    }
                }
                printf("------------------------------------------------------------------------------\n");

                if (disponiveis == 0) {
                    printf("\n[Info] De momento nao existem livros disponiveis para troca.\n");
                } else {
                    // 2. SELEÇÃO
                    printf("\nIntroduza o ID do Livro que quer requisitar (0 para cancelar): ");
                    int idAlvo = lerInteiro();

                    if (idAlvo != 0) {
                        // Encontrar o livro escolhido e validar novamente
                        int idx = -1;
                        for(int i = 0; i < *totalBooks; i++) {
                            if(books[i].id == idAlvo) {
                                idx = i;
                                break;
                            }
                        }

                        // Validação de segurança final (para garantir que ele escolheu um da lista)
                        if (idx != -1) {
                            if (books[idx].retido == 1) {
                                printf("[Erro] Esse livro ja nao esta disponivel.\n");
                            } 
                            else if (books[idx].userId == idLogado) {
                                printf("[Erro] Nao pode requisitar o seu proprio livro.\n");
                            } 
                            else if (books[idx].userId == 0) {
                                printf("[Erro] Livros da instituicao nao estao disponiveis para troca direta.\n");
                            } 
                            else {
                                // TUDO VÁLIDO -> CRIAR PEDIDO
                                solicitarEmprestimo(loans, totalLoans, &books[idx], idLogado);
                                guardarEmprestimos(loans, *totalLoans); // Persistência imediata
                            }
                        } else {
                            printf("[Erro] ID de livro invalido.\n");
                        }
                    }
                }
                esperarEnter();
                break;

            case 5:
                // Nota: Assume-se que a tua função doarLivro adiciona ao array
                // e incrementa o *totalBooks lá dentro.
                // Ajusta os argumentos conforme a função que o teu colega fez.
                /* Exemplo hipotético:
                   doarLivro(books, totalBooks, idLogado); 
                */
                
                // Se a função "doarLivro" não estiver disponível, avisa:
                printf("Funcionalidade de doar livro a ser implementada.\n");
                
                // Se funcionou, guarda:
                guardarLivros(books, *totalBooks);
                esperarEnter();
                break;

            case 0:
                // Voltar ao menu anterior
                break;

            default:
                printf("Opcao invalida.\n");
                esperarEnter();
        }

    } while (opMercado != 0);
}

// --- SUB-MENU MEUS LIVROS ---

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
        printf("Opcao: ");
        opMeus = lerInteiro();

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
                printf("\nIntroduza o ID do Livro (0 para cancelar): ");
                int idEdit = lerInteiro();

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

// --- SUB-MENU MOVIMENTOS ---

/**
 * @brief Menu principal para gerir devoluções, aceitar pedidos e dar feedback.
 */
void menuGestaoMovimentos(Emprestimo loans[], int *totalLoans, Livro books[], int *totalBooks, Feedback feedbacks[], int *totalFeedbacks, int idLogado) {
    int opMov;

    do {
        limparEcra();
        printf("\n--- GESTAO DE MOVIMENTOS ---\n");
        printf("1. Ver Pedidos Pendentes (Aceitar/Recusar)\n");
        printf("2. Devolver Livro (Emprestimos)\n");
        printf("3. Historico Global\n");
        printf("4. Avaliar Troca/Emprestimo (Feedback)\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        opMov = lerInteiro();

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
                printf("Introduza o ID do Livro que quer devolver: ");
                int idDev = lerInteiro();
                
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
                
                printf("\nID da Operacao (Pedido #) a avaliar: ");
                int idLoan = lerInteiro();
                
                Emprestimo *loanAlvo = NULL;
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

/// --- SUB-MENU PERFIL ---

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