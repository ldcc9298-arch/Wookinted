#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Necessário para gerar datas automáticas
#include "loans.h"
#include "books.h"
#include "utils.h"
#include "files.h"

/**
+ * @brief Obtém a data atual no formato YYYYMMDD.
+ * @return Data atual como inteiro.
+ */
int obterDataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // Ex: 2025 * 10000 + 1 * 100 + 1 = 20250101
    return ((tm.tm_year + 1900) * 10000) + ((tm.tm_mon + 1) * 100) + tm.tm_mday;
}

/**
 * @brief Solicita uma operação de empréstimo ou troca.
 * Lógica:
 * - Verifica se o utilizador já tem o livro.
 * - Cria um novo registo de empréstimo com estado PENDENTE.
 * - Define o tipo de operação com base na posse do livro.
 */
void solicitarEmprestimo(Emprestimo loans[], int *totalLoans, Livro *book, int idRequisitante) {
    // Validação básica: não pedir o que já se tem
    if (book->userIdEmprestimo == idRequisitante) {
        printf("Erro: Voce ja tem este livro na sua posse.\n");
        return;
    }

    // Prepara a estrutura do novo pedido
    Emprestimo novo;
    novo.id = (*totalLoans) + 1;
    novo.bookId = book->id;
    novo.userId = idRequisitante;           // Quem está a pedir
    novo.userIdEmprestimo = book->userIdEmprestimo; // Quem tem o livro agora (Dador)
    novo.estado = PENDENTE;                 // Fica à espera de aprovação
    novo.dataEmprestimo = obterDataAtual(); // Data de hoje
    novo.dataDevolucao = 0;                 // Ainda não devolvido
    novo.idOperacao = 0;                    // ID reservado para uso futuro

    // Lógica de Decisão do Tipo de Operação
    if (book->userId == 0) {
        // Cenário A: Livro da Biblioteca (Instituição)
        novo.tipoOperacao = EMPRESTIMO; 
        printf("Pedido de LEVANTAMENTO (Instituicao) registado.\n");
    } else {
        // Cenário B: Livro de outro Utilizador
        int escolha;
        printf("\nO livro pertence ao Utilizador %d.\n", book->userId);
        printf("Qual a sua intencao?\n");
        printf("1 - Pedir Emprestado (Ler e Devolver)\n");
        printf("2 - Propor Troca (Ficar com o livro para sempre)\n");
        printf("Escolha: ");
        scanf("%d", &escolha);
        getchar(); // Limpar buffer do enter

        if (escolha == 2) {
            novo.tipoOperacao = TROCA; // Mudança de Dono
            printf("Proposta de TROCA enviada.\n");
        } else {
            novo.tipoOperacao = EMPRESTIMO; // Mudança de Posse apenas
            printf("Pedido de EMPRESTIMO enviado.\n");
        }
    }

    // Guarda no array e incrementa contador
    loans[*totalLoans] = novo;
    (*totalLoans)++;
}

/**
 * @brief Gere os pedidos pendentes para o utilizador logado.
 * Lógica:
 * - Mostra apenas pedidos onde o utilizador logado é quem tem o livro atualmente.
 * - Permite aceitar ou recusar cada pedido.
 * - Atualiza o estado do empréstimo e a posse/propriedade do livro conforme necessário.
 */
void gerirPedidosPendentes(Emprestimo loans[], int totalLoans, Livro books[], int totalBooks, int idLogado) {
    int encontrou = 0;
    int opcao;

    for (int i = 0; i < totalLoans; i++) {
        // Filtro: Mostra apenas pedidos onde o utilizador logado é quem tem o livro atualmente
        if (loans[i].userIdEmprestimo == idLogado && loans[i].estado == PENDENTE) {
            encontrou = 1;
            
            char tipo[20];
            if (loans[i].tipoOperacao == TROCA) strcpy(tipo, "TROCA (Permanente)");
            else strcpy(tipo, "EMPRESTIMO (Temp.)");

            printf("\n--- Pedido Pendente [%s] ID: %d ---", tipo, loans[i].id);
            printf("\nO Utilizador %d quer o Livro ID %d.", loans[i].userId, loans[i].bookId);
            
            printf("\nAceitar? (1-Sim, 2-Nao): ");
            scanf("%d", &opcao);
            getchar();

            if (opcao == 1) {
                loans[i].estado = ACEITE; // Marca como aceite inicialmente
                
                // Procura o livro correspondente para atualizar a posse/propriedade
                for (int j = 0; j < totalBooks; j++) {
                    if (books[j].id == loans[i].bookId) {
                        
                        if (loans[i].tipoOperacao == TROCA) {
                            // --- LÓGICA DE TROCA ---
                            books[j].userId = loans[i].userId;           // O Dono muda!
                            books[j].userIdEmprestimo = loans[i].userId; // A Posse muda!
                            books[j].Disponivel = DISPONIVEL; // Fica disponível na mão do novo dono
                            
                            loans[i].estado = CONCLUIDO; // Troca finalizada, não há devolução
                            printf("Troca realizada! O Utilizador %d e agora o novo dono.\n", loans[i].userId);

                        } else {
                            // --- LÓGICA DE EMPRÉSTIMO ---
                            books[j].userIdEmprestimo = loans[i].userId; // Só a Posse muda!
                            books[j].Disponivel = INDISPONIVEL;          // Ocupado (saiu do dono)
                            printf("Emprestimo aceite! O livro esta agora com o Utilizador %d (Temporario).\n", loans[i].userId);
                        }
                        break;
                    }
                }
            } else {
                loans[i].estado = REJEITADO;
                printf("Pedido recusado.\n");
            }
        }
    }
    if (!encontrou) printf("\nNao tens pedidos pendentes.\n");
}

/**
 * @brief Processa a devolução de um livro.
 * Lógica:
 * - Verifica se o utilizador tem o livro.
 * - Atualiza o estado do empréstimo para CONCLUIDO.
 * - Restaura a posse do livro ao dono original.
 */
void devolverLivro(Livro *book, Emprestimo loans[], int totalLoans, int idLogado) {
    // 1. Verificação de Posse
    if (book->userIdEmprestimo != idLogado) {
        printf("Erro: Este livro nao esta consigo.\n");
        return;
    }

    // 2. Verificação de Propriedade (Não se devolve o que é nosso)
    if (book->userId == idLogado) {
        printf("Erro: Este livro e seu (Dono). Nao pode devolver o que lhe pertence.\n");
        return;
    }

    int loanFound = 0;
    // 3. Procura o registo do empréstimo ativo para fechar
    for(int i = 0; i < totalLoans; i++) {
        // Procura: Mesmo livro, mesmo utilizador, estado ACEITE (Ativo)
        if (loans[i].bookId == book->id && loans[i].userId == idLogado && loans[i].estado == ACEITE) {
            
            // Segurança: Trocas nunca deveriam chegar aqui pois ficam CONCLUIDO logo
            if (loans[i].tipoOperacao == TROCA) {
                printf("Erro: Inconsistencia detectada. Trocas nao sao devolvidas.\n");
                return;
            }

            loans[i].estado = CONCLUIDO;          // Fecha o ciclo
            loans[i].dataDevolucao = obterDataAtual(); // Data de hoje
            loanFound = 1;

            // --- LÓGICA DE RETORNO ---
            // O detentor volta a ser o dono original
            book->userIdEmprestimo = book->userId; 
            book->Disponivel = DISPONIVEL; 

            if (book->userId == 0) {
                printf("Livro devolvido a Instituicao.\n");
            } else {
                printf("Livro devolvido ao dono original (Utilizador %d).\n", book->userId);
            }
            break;
        }
    }

    if (!loanFound) {
        printf("Erro: Nao foi encontrado registo de emprestimo ativo para este livro.\n");
    }
}

/**
 * @brief Doação de um livro à Instituição.
 * Lógica:
 * - O livro é adicionado com `userId` e `userIdEmprestimo` como 0 (Instituição).
 * - Regista uma operação de doação para histórico.
 */
void doarLivro(Livro books[], int *totalBooks, int userId, Emprestimo loans[], int *totalLoans) {
    Livro novo;
    novo.id = (*totalBooks) + 1;
    novo.userId = 0;                // Dono: Instituição
    novo.userIdEmprestimo = 0;      // Detentor: Instituição
    
    printf("Titulo: ");
    fgets(novo.titulo, MAX_STRING, stdin);
    novo.titulo[strcspn(novo.titulo, "\n")] = 0;

    printf("Autor: ");
    fgets(novo.autor, MAX_STRING, stdin);
    novo.autor[strcspn(novo.autor, "\n")] = 0;

    novo.Disponivel = DISPONIVEL;
    novo.retido = 0;

    // Regista histórico para auditoria
    Emprestimo op;
    op.id = (*totalLoans) + 1;
    op.bookId = novo.id;
    op.userId = 0;                 // Destino
    op.userIdEmprestimo = userId;  // Origem
    op.tipoOperacao = DOACAO;
    op.estado = CONCLUIDO;
    op.dataEmprestimo = obterDataAtual();
    op.dataDevolucao = 0;
    op.idOperacao = 0;

    books[*totalBooks] = novo;
    (*totalBooks)++;
    loans[*totalLoans] = op;
    (*totalLoans)++;

    printf("Livro doado a Instituicao com sucesso!\n");
}

/**
 * @brief Avalia um empréstimo concluído.
 * Lógica:
 * - Apenas permite avaliar se o empréstimo está ACEITE ou CONCLUIDO.
 * - Identifica quem é o avaliado com base em quem está logado.
 * - Evita avaliações duplicadas para o mesmo empréstimo pelo mesmo avaliador.
 */
void avaliarEmprestimo(Feedback feedbacks[], int *totalFeedbacks, Emprestimo *loan, int idLogado) {
    // Só avalia operações terminadas ou em curso (aceites)
    if (loan->estado != ACEITE && loan->estado != CONCLUIDO) {
        printf("Erro: A operacao tem de estar ativa ou concluida.\n");
        return;
    }

    // Identifica quem é o alvo da avaliação com base em quem está logado
    int idAvaliado = -1;
    if (idLogado == loan->userId) idAvaliado = loan->userIdEmprestimo; // Quem pediu avalia quem deu
    else if (idLogado == loan->userIdEmprestimo) idAvaliado = loan->userId; // Quem deu avalia quem pediu
    else {
        printf("Erro: Nao pertence a esta operacao.\n");
        return;
    }

    // Evita duplicados (spam de avaliações)
    for (int i = 0; i < *totalFeedbacks; i++) {
        if (feedbacks[i].loanId == loan->id && feedbacks[i].avaliadorId == idLogado) {
            printf("Ja avaliou esta operacao.\n");
            return;
        }
    }

    Feedback novo;
    novo.id = (*totalFeedbacks) + 1;
    novo.loanId = loan->id;
    novo.avaliadorId = idLogado;
    novo.avaliadoId = idAvaliado;
    novo.userId = idAvaliado;
    novo.feedbackId = 0;
    novo.dataAvaliacao = obterDataAtual();

    // Mostra contexto ao utilizador
    char acao[30];
    if (loan->tipoOperacao == DOACAO) strcpy(acao, "Doacao");
    else if (loan->tipoOperacao == TROCA) strcpy(acao, "Troca");
    else strcpy(acao, "Emprestimo");

    printf("\n--- Avaliar %s (ID: %d) ---\n", acao, loan->id);
    if (idAvaliado == 0) printf("Avaliando: Instituicao\n");
    else printf("Avaliando: Utilizador %d\n", idAvaliado);

    printf("Nota (1-5): ");
    if (scanf("%d", &novo.nota) != 1) { getchar(); return; }
    getchar();

    // Validação de intervalo
    if (novo.nota < 1) novo.nota = 1;
    if (novo.nota > 5) novo.nota = 5;

    feedbacks[*totalFeedbacks] = novo;
    (*totalFeedbacks)++;
    printf("Avaliacao guardada!\n");
}

/**
 * @brief Lista os feedbacks de um utilizador avaliado.
 * @param feedbacks array de feedbacks.
 * @param totalFeedbacks número total de feedbacks.
 * @param idAvaliado ID do utilizador avaliado.
 */
void listarFeedbacks(Feedback feedbacks[], int totalFeedbacks, int idAvaliado) {
    int s = 0, c = 0;
    printf("\n--- Perfil de Reputacao: User %d ---\n", idAvaliado);
    
    for(int i=0; i<totalFeedbacks; i++){
        if(feedbacks[i].avaliadoId == idAvaliado){
            printf("- Nota: %d (de User %d)\n", feedbacks[i].nota, feedbacks[i].avaliadorId);
            s += feedbacks[i].nota;
            c++;
        }
    }
    
    if(c > 0) printf(">> Media Global: %.1f / 5.0\n", (float)s/c);
    else printf(">> Sem avaliacoes.\n");
}

// NOVO

/**
 * @brief Lista todos os empréstimos no sistema.
 * @param loans array de empréstimos.
 * @param totalLoans número total de empréstimos.
 * @param idLogado ID do utilizador logado.
 */
void listarEmprestimos(Emprestimo loans[], int totalLoans, int idLogado) {
    printf("\n=== MEUS EMPRESTIMOS E TROCAS ===\n");
    int encontrados = 0;

    for (int i = 0; i < totalLoans; i++) {
        
        if (loans[i].userId == idLogado || loans[i].userIdEmprestimo == idLogado) {
            
            printf("Pedido #%d | Livro ID: %d | Estado: %d\n", 
                   loans[i].id, 
                   loans[i].bookId, 
                   loans[i].estado);
                   
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("    (Nenhum historico encontrado)\n");
    }
}

/**
 * @brief Verifica se o utilizador tem pedidos pendentes e mostra um alerta.
 */
void verificarNotificacoes(Emprestimo loans[], int totalLoans, int idLogado) {
    int pendentes = 0;
    
    // Conta quantos pedidos estão à espera de aprovação deste utilizador
    for(int i = 0; i < totalLoans; i++) {
        if(loans[i].userIdEmprestimo == idLogado && loans[i].estado == 0) { // 0 = PENDENTE
            pendentes++;
        }
    }

    // Se houver, mostra o alerta
    if(pendentes > 0) {
        printf("\n***************************************************\n");
        printf(" ALERTA: Tens %d pedido(s) de troca/emprestimo pendentes!\n", pendentes);
        printf(" Vai a 'Gestao de Movimentos' para responder.\n");
        printf("***************************************************\n");
        esperarEnter(); // Pausa para ele ver o alerta
    }
}




