#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Necessário para gerar datas automáticas

#include "books.h"
//#include "users.h"
#include "transactions.h"
#include "utils.h"
#include "files.h"
//#include "interface.h"
#include "structs.h"

void gerirPedidosPendentes(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, 
                            Utilizador users[], int totalUsers, Feedback feedbacks[], int totalFeedbacks, int idLogado) {
    limparEcra();
    printf("\n=== PEDIDOS PENDENTES (Meus Livros) ===\n");
    printf("%-3s | %-12s | %-25s | %-15s | %-10s | %-10s\n", 
           "N.", "TIPO", "LIVRO", "REQUERENTE", "DURACAO", "REPUTACAO");
    printf("----------------------------------------------------------------------------------------------------\n");

    int visualId = 1;
    int temPedidos = 0;
    int mapeamentoOp[MAX_OPERACOES];

    for (int i = 0; i < *totalOperacoes; i++) {
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            int idxLivro = -1;
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) {
                    idxLivro = b;
                    break;
                }
            }

            if (idxLivro != -1 && books[idxLivro].idProprietario == idLogado) {
                char nomeReq[20] = "Desconhecido";
                for (int u = 0; u < totalUsers; u++) {
                    if (users[u].id == operacoes[i].idRequerente) {
                        strncpy(nomeReq, users[u].nome, 19);
                        nomeReq[19] = '\0';
                        break;
                    }
                }

                int qtdAval;
                float mediaReq = calcularMediaUtilizador(feedbacks, totalFeedbacks, operacoes[i].idRequerente, &qtdAval);

                char tipoStr[15];
                char duracaoStr[15];
                if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) {
                    strcpy(tipoStr, "EMPRESTIMO");
                    sprintf(duracaoStr, "%d dias", operacoes[i].dias);
                } else {
                    strcpy(tipoStr, "TROCA");
                    strcpy(duracaoStr, "Definitiva");
                }

                printf("%-3d | %-12s | %-25.25s | %-15.15s | %-10s | %.1f* (%d)\n", 
                       visualId, tipoStr, books[idxLivro].titulo, nomeReq, duracaoStr, mediaReq, qtdAval);
                
                mapeamentoOp[visualId] = i;
                visualId++;
                temPedidos = 1;
            }
        }
    }

    if (!temPedidos) {
        printf("\n[Info] Nao tem pedidos pendentes de aprovacao.\n");
        printf("--------------------------------------------------------------------------------\n");
        esperarEnter();
        return;
    }

    printf("--------------------------------------------------------------------------------\n");
    int escolha = lerInteiro("Escolha o N. do pedido a tratar (0 para voltar): ", 0, visualId - 1);
    if (escolha == 0) return;

    int idxOp = mapeamentoOp[escolha];
    int idxLivro = -1;

    for (int b = 0; b < totalBooks; b++) {
        if (books[b].id == operacoes[idxOp].idLivro) { 
            idxLivro = b; 
            break; 
        }
    }

    if (idxOp != -1 && idxLivro != -1) {
        limparEcra();
        printf("\n--- DETALHES DO PEDIDO ---\n");
        printf("Livro Solicitado: %s\n", books[idxLivro].titulo);
        
        int idxLivroOferecido = -1;
        if (operacoes[idxOp].tipo == OP_TIPO_TROCA) {
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[idxOp].idLivroTroca) {
                    idxLivroOferecido = b;
                    printf("Proposta de Troca por: '%s' de %s\n", books[b].titulo, books[b].autor);
                    break;
                }
            }
        } else {
            printf("Tipo: EMPRESTIMO\nPeriodo solicitado: %d dias\n", operacoes[idxOp].dias);
        }

        printf("\n1. Aceitar\n2. Recusar\n0. Cancelar\n");
        int acao = lerInteiro("Opcao: ", 0, 2);

        char detalheLog[200]; 

        if (acao == 1) { // === ACEITAR ===
            int dataHoje = obterDataAtual();
            operacoes[idxOp].dataEmprestimo = dataHoje; 

            if (operacoes[idxOp].tipo == OP_TIPO_EMPRESTIMO) {
                operacoes[idxOp].estado = ESTADO_OP_ACEITE; 
                books[idxLivro].estado = LIVRO_EMPRESTADO; 
                books[idxLivro].idDetentor = operacoes[idxOp].idRequerente; 
                books[idxLivro].numRequisicoes++;
                
                int dtLimite = adicionarDias(dataHoje, operacoes[idxOp].dias);
                operacoes[idxOp].dataDevolucaoPrevista = dtLimite;

                sprintf(detalheLog, "EMPRESTIMO ACEITE: Livro '%s' (ID %d) emprestado ao Utilizador %d. Limite: %d", 
                        books[idxLivro].titulo, books[idxLivro].id, operacoes[idxOp].idRequerente, dtLimite);
                registarLog(idLogado, "EMPRESTIMO_APROVADO", detalheLog);

                printf("\n[Sucesso] Emprestimo aprovado!\n");
                printf("Data limite de devolucao: %02d/%02d/%04d\n", 
                        dtLimite % 100,           // Dia
                        (dtLimite % 10000) / 100, // Mês
                        dtLimite / 10000);        // Ano
            } 
            else if (operacoes[idxOp].tipo == OP_TIPO_TROCA) {
                operacoes[idxOp].estado = ESTADO_OP_CONCLUIDO;
                operacoes[idxOp].dataFecho = dataHoje;

                if (*totalOperacoes < MAX_OPERACOES) {
                    int newIdx = *totalOperacoes;
                    operacoes[newIdx].id = (*totalOperacoes) + 1;
                    operacoes[newIdx].tipo = OP_TIPO_TROCA;
                    operacoes[newIdx].estado = ESTADO_OP_CONCLUIDO;
                    operacoes[newIdx].idLivro = operacoes[idxOp].idLivroTroca;
                    operacoes[newIdx].idProprietario = operacoes[idxOp].idRequerente;
                    operacoes[newIdx].idRequerente = idLogado;
                    operacoes[newIdx].dataPedido = operacoes[idxOp].dataPedido;
                    operacoes[newIdx].dataFecho = dataHoje;
                    (*totalOperacoes)++;
                }

                books[idxLivro].idProprietario = operacoes[idxOp].idRequerente;
                books[idxLivro].idDetentor = operacoes[idxOp].idRequerente;
                books[idxLivro].estado = LIVRO_DISPONIVEL;
                books[idxLivro].numRequisicoes++;

                if (idxLivroOferecido != -1) {
                    books[idxLivroOferecido].idProprietario = idLogado;
                    books[idxLivroOferecido].idDetentor = idLogado;
                    books[idxLivroOferecido].estado = LIVRO_DISPONIVEL;
                    books[idxLivroOferecido].numRequisicoes++;

                    sprintf(detalheLog, "TROCA CONCLUIDA: Livro '%s' (ID %d) trocado por '%s' (ID %d) com Utilizador %d", 
                            books[idxLivro].titulo, books[idxLivro].id, 
                            books[idxLivroOferecido].titulo, books[idxLivroOferecido].id, 
                            operacoes[idxOp].idRequerente);
                    registarLog(idLogado, "TROCA_CONCLUIDA", detalheLog);
                }

                printf("\n[Sucesso] Troca concluida com sucesso!\n");
            }
            
            guardarOperacoes(operacoes, *totalOperacoes);
            guardarLivros(books, totalBooks);

        } else if (acao == 2) { // === RECUSAR ===
            operacoes[idxOp].estado = ESTADO_OP_REJEITADO;
            books[idxLivro].estado = LIVRO_DISPONIVEL;
        
            if (operacoes[idxOp].tipo == OP_TIPO_TROCA && idxLivroOferecido != -1) {
                books[idxLivroOferecido].estado = LIVRO_DISPONIVEL;
            }

            sprintf(detalheLog, "PEDIDO RECUSADO: O proprietario %d recusou o pedido (Tipo %d) do livro ID %d", 
                    idLogado, operacoes[idxOp].tipo, books[idxLivro].id);
            registarLog(idLogado, "PEDIDO_REJEITADO", detalheLog);
            
            guardarOperacoes(operacoes, *totalOperacoes);
            guardarLivros(books, totalBooks);
            printf("\n[Info] Pedido recusado.\n");
        }
        esperarEnter();
    }
}

void doarLivro(Livro books[], int totalBooks, int idLogado, Operacao operacoes[], int *totalOperacoes) {
    (void)operacoes; 
    (void)totalOperacoes; 

    limparEcra();
    printf("\n=== DOAR LIVRO AO IPCA ===\n");
    printf("Escolha qual dos seus livros deseja doar a instituicao:\n");
    printf("%-3s | %-30s | %-20s\n", "N.", "TITULO", "AUTOR");
    printf("----------------------------------------------------------\n");

    int visualId = 1; 
    int temLivros = 0;

    for (int i = 0; i < totalBooks; i++) {
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

    int escolha = lerInteiro("\nIntroduza o N. do livro (0 para cancelar): ", 0, visualId - 1);

    if (escolha == 0) return;

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

    if (idxEncontrado != -1) {
        // === ALTERAÇÃO AQUI: Uso de 1/0 para coerência ===
        printf("\nConfirma a doacao do livro '%s'?\n", books[idxEncontrado].titulo);
        int confirmacao = lerInteiro("(1-Sim / 0-Nao): ", 0, 1);

        if (confirmacao == 1) {
            // Mudança de proprietário para o IPCA (ID 1)
            books[idxEncontrado].idProprietario = 1; 
            books[idxEncontrado].idDetentor = 1;
            books[idxEncontrado].estado = LIVRO_DISPONIVEL; 

            // Gravar as alterações no ficheiro
            guardarLivros(books, totalBooks); 

            printf("\n[Sucesso] O livro foi doado ao IPCA. Obrigado pela sua generosidade!\n");
        } else {
            printf("\n[Cancelado] A doacao foi cancelada.\n");
        }
    } else {
        printf("\n[Erro] Livro nao encontrado.\n");
    }
    
    esperarEnter();
}


/**
 * @brief Avalia um empréstimo concluído.
 * Lógica:
 * - Apenas permite avaliar se o empréstimo está ACEITE ou CONCLUIDO.
 * - Identifica quem é o avaliado com base em quem está logado.
 * - Evita avaliações duplicadas para o mesmo empréstimo pelo mesmo avaliador.
 */
void avaliarEmprestimo(Feedback feedbacks[], int *totalFeedbacks, Operacao *loan, int idLogado) {
    // Só avalia operações terminadas ou em curso (aceites)
    if (loan->estado != ESTADO_OP_ACEITE && loan->estado != ESTADO_OP_CONCLUIDO) {
        printf("Erro: A operacao tem de estar ativa ou concluida.\n");
        return;
    }

    // Identifica quem é o alvo da avaliação com base em quem está logado
    int idAvaliado = -1;
    if (idLogado == loan->idProprietario) idAvaliado = loan->idRequerente; // Quem pediu avalia quem deu
    else if (idLogado == loan->idRequerente) idAvaliado = loan->idProprietario; // Quem deu avalia quem pediu
    else {
        printf("Erro: Nao pertence a esta operacao.\n");
        return;
    }

    // Evita duplicados (spam de avaliações)
    for (int i = 0; i < *totalFeedbacks; i++) {
        if (feedbacks[i].idOperacao == loan->id && feedbacks[i].idAvaliador == idLogado) {
            printf("Ja avaliou esta operacao.\n");
            return;
        }
    }

    Feedback novo;
    novo.id = (*totalFeedbacks) + 1;
    novo.idOperacao = loan->id;
    novo.idAvaliador = idLogado;
    novo.idAvaliado = idAvaliado;
    novo.idResposta = 0;
    novo.dataAvaliacao = obterDataAtual();

    // Mostra contexto ao utilizador
    char acao[30];
    if (loan->tipo == OP_TIPO_DOACAO) strcpy(acao, "Doacao");
    else if (loan->tipo == OP_TIPO_TROCA) strcpy(acao, "Troca");
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
        if(feedbacks[i].idAvaliado == idAvaliado){
            printf("- Nota: %d (de User %d)\n", feedbacks[i].nota, feedbacks[i].idAvaliador);
            s += feedbacks[i].nota;
            c++;
        }
    }
    
    if(c > 0) printf(">> Media Global: %.1f / 5.0\n", (float)s/c);
    else printf(">> Sem avaliacoes.\n");
}

/**
 * @brief Lista os empréstimos e trocas associados ao utilizador logado.
 * @param operacoes Array de operações/empréstimos.
 * @param totalOperacoes Total de operações.
 * @param books Array de livros.
 * @param totalBooks Total de livros.
 * @param idLogado ID do utilizador logado.
 */
void listarEmprestimos(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, int idLogado) {
    printf("%-3s | %-25s | %-12s | %-12s | %s\n", "ID", "LIVRO", "DATA INICIO", "DATA FIM", "ESTADO");
    printf("--------------------------------------------------------------------------------\n");

    int encontrou = 0;
    char dataIni[12], dataFim[12];

    for (int i = 0; i < totalOperacoes; i++) {
        // Mostra se eu sou o Requerente ou o Dono/Detentor
        if (operacoes[i].idRequerente == idLogado || operacoes[i].idProprietario == idLogado) {
            
            // Só vale a pena mostrar se não for devolução nem doação (foco em empréstimos/trocas ativos ou recentes)
            if (operacoes[i].tipo == OP_TIPO_DEVOLUCAO || operacoes[i].tipo == OP_TIPO_DOACAO) continue;

            // Buscar Titulo
            char titulo[MAX_STRING] = "---";
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[i].idLivro) {
                    strcpy(titulo, books[b].titulo); break;
                }
            }

            // Formatar Datas
            formatarData(operacoes[i].dataEmprestimo, dataIni);
            
            if (operacoes[i].dias > 0) {
                // Se tem dias, calculamos o fim
                int dataFimInt = somarDias(operacoes[i].dataEmprestimo, operacoes[i].dias);
                formatarData(dataFimInt, dataFim);
            } else {
                // Troca não tem fim
                strcpy(dataFim, "Permanente");
            }

            // Traduzir Estado
            char estadoStr[20];
            switch(operacoes[i].estado) {
                case ESTADO_OP_PENDENTE: strcpy(estadoStr, "Pendente"); break;
                case ESTADO_OP_ACEITE:   strcpy(estadoStr, "A decorrer"); break;
                case ESTADO_OP_REJEITADO:strcpy(estadoStr, "Recusado"); break;
                case ESTADO_OP_CONCLUIDO:strcpy(estadoStr, "Concluido"); break;
                default: strcpy(estadoStr, "?");
            }

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

// Verifica se um utilizador já avaliou uma operação específica
int jaAvaliou(Feedback feeds[], int totalFeeds, int idOperacao, int idAvaliador) {
    // Proteção contra lixo de memória
    if (totalFeeds < 0 || totalFeeds > MAX_FEEDBACKS) return 0;

    for (int i = 0; i < totalFeeds; i++) {
        if (feeds[i].idOperacao == idOperacao && feeds[i].idAvaliador == idAvaliador) {
            return 1; // Já avaliou
        }
    }
    return 0; // Ainda não
}

// Adiciona uma nova operação ao array
void adicionarOperacao(Operacao operacoes[], int *totalOperacoes, int idRequerente, int idLivro, TipoOperacao tipo, int dias) {
    if (*totalOperacoes >= MAX_OPERACOES) {
        printf("[Erro] Limite de operacoes atingido.\n");
        return;
    }

    int i = *totalOperacoes;
    
    operacoes[i].id = i + 1; // Gera ID sequencial
    operacoes[i].idRequerente = idRequerente;
    operacoes[i].idLivro = idLivro;
    operacoes[i].tipo = tipo;
    operacoes[i].dias = dias;
    operacoes[i].estado = ESTADO_OP_PENDENTE;
    operacoes[i].dataPedido = obterDataAtual();
    
    // IMPORTANTE: Como não passamos o array 'books' aqui, não sabemos quem é o dono agora.
    // O ideal seria passar 'books[]' como argumento, ou preencher isto noutro lado.
    // Por agora fica a 0 para permitir compilar.
    operacoes[i].idProprietario = 0; 
    operacoes[i].dataEmprestimo = 0;
    operacoes[i].dataFecho = 0;

    (*totalOperacoes)++;
}

void registarDevolucao(Operacao operacoes[], int totalOperacoes, Livro livros[], int totalLivros, int idLivro, int idUtilizador) {
    int encontrouEmprestimo = 0;
    int idxOp = -1;

    // 1. Procurar o registo do empréstimo ATIVO
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].idLivro == idLivro && 
            operacoes[i].idRequerente == idUtilizador && 
            operacoes[i].estado == ESTADO_OP_ACEITE) {
            
            idxOp = i;
            encontrouEmprestimo = 1;
            break;
        }
    }

    if (!encontrouEmprestimo) {
        printf("[Erro] Nao existe emprestimo ativo deste livro para este utilizador.\n");
        return;
    }

    // 2. Atualizar a OPERAÇÃO (Histórico)
    // Não apagamos! Apenas fechamos o ciclo.
    operacoes[idxOp].estado = ESTADO_OP_CONCLUIDO;
    operacoes[idxOp].dataDevolucaoReal = obterDataAtual(); // Guarda o dia de hoje

    // Cálculo simples de multa/atraso (opcional)
    if (operacoes[idxOp].dataDevolucaoReal > operacoes[idxOp].dataDevolucaoPrevista) {
        printf("[Aviso] Livro entregue fora do prazo! (Previsto: %d)\n", operacoes[idxOp].dataDevolucaoPrevista);
    }

    // 3. Atualizar o LIVRO (Stock)
    int idxLivro = -1;
    for (int i = 0; i < totalLivros; i++) {
        if (livros[i].id == idLivro) {
            idxLivro = i;
            break;
        }
    }

    if (idxLivro != -1) {
        livros[idxLivro].estado = LIVRO_DISPONIVEL; // Volta a estar livre
        livros[idxLivro].idDetentor = livros[idxLivro].idProprietario; // O dono volta a ter o livro na mão
        printf("[Sucesso] O livro '%s' foi devolvido ao proprietario.\n", livros[idxLivro].titulo);
    }

    esperarEnter();
}

int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes) {
    for (int i = 0; i < totalOperacoes; i++) {
        // Verifica se o utilizador aparece como Requerente OU como Proprietário
        if (operacoes[i].idRequerente == idUser || operacoes[i].idProprietario == idUser) {
            return 1; // Verdadeiro: Encontrou registos
        }
    }
    return 0; // Falso: Utilizador limpo
}

