#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Necessário para gerar datas automáticas

#include "books.h"
//#include "users.h"
#include "transactions.h"
#include "utils.h"
//#include "files.h"
//#include "interface.h"
#include "structs.h"

/**
 * @brief Solicita uma operação de empréstimo ou troca.
 * Lógica:
 * - Verifica se o utilizador já tem o livro.
 * - Cria um novo registo de empréstimo com estado PENDENTE.
 * - Define o tipo de operação com base na posse do livro.
 */
void solicitarEmprestimo(Operacao operacoes[], int *totalOperacoes, Livro *book, int idRequisitante) {
    // Validação básica: não pedir o que já se tem
    if (book->idDetentor == idRequisitante) {
        printf("Erro: Voce ja tem este livro na sua posse.\n");
        return;
    }

    // Prepara a estrutura do novo pedido
    Operacao novo;
    novo.id = (*totalOperacoes) + 1;
    novo.idLivro = book->id;
    novo.idProprietario = idRequisitante;           // Quem está a pedir
    novo.idRequerente = book->idDetentor; // Quem tem o livro agora (Dador)
    novo.estado = ESTADO_OP_PENDENTE;                 // Fica à espera de aprovação
    novo.dataPedido = obterDataAtual(); // Data de hoje
    novo.dataDevolucaoPrevista = 0;                 // Ainda não devolvido
    novo.id = 0;                    // ID reservado para uso futuro

    // Lógica de Decisão do Tipo de Operação
    if (book->idProprietario == 0) {
        // Cenário A: Livro da Biblioteca (Instituição)
        novo.tipo = OP_TIPO_EMPRESTIMO; 
        printf("Pedido de LEVANTAMENTO (Instituicao) registado.\n");
    } else {
        // Cenário B: Livro de outro Utilizador
        int escolha;
        printf("\nO livro pertence ao Utilizador %d.\n", book->idProprietario);
        printf("Qual a sua intencao?\n");
        printf("1 - Pedir Emprestado (Ler e Devolver)\n");
        printf("2 - Propor Troca (Ficar com o livro para sempre)\n");
        printf("Escolha: ");
        scanf("%d", &escolha);
        getchar(); // Limpar buffer do enter

        if (escolha == 2) {
            novo.tipo = OP_TIPO_TROCA; // Mudança de Dono
            printf("Proposta de TROCA enviada.\n");
        } else {
            novo.tipo = OP_TIPO_EMPRESTIMO; // Mudança de Posse apenas
            printf("Pedido de EMPRESTIMO enviado.\n");
        }
    }

    // Guarda no array e incrementa contador
    operacoes[*totalOperacoes] = novo;
    (*totalOperacoes)++;
    book->numRequisicoes++; // Incrementa o contador de requisições do livro
}

void gerirPedidosPendentes(Operacao operacoes[], int totalOperacoes, Livro books[], int totalBooks, Utilizador users[], int totalUsers, int idLogado) {
    limparEcra();
    printf("\n=== PEDIDOS PENDENTES (Meus Livros) ===\n");
    printf("%-3s | %-12s | %-30s | %-20s\n", "N.", "TIPO", "LIVRO", "REQUERENTE");
    printf("--------------------------------------------------------------------------------\n");

    int visualId = 1;
    int temPedidos = 0;

    // 1. Listar Pedidos
    for (int i = 0; i < totalOperacoes; i++) {
        // Só mostramos pedidos que ainda aguardam resposta
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            
            // Encontrar índice do livro solicitado (O MEU)
            int idxLivro = -1;
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) {
                    idxLivro = b;
                    break;
                }
            }

            // Se o livro existe E eu sou o dono (ou sou Admin e o livro é do sistema)
            if (idxLivro != -1 && (books[idxLivro].idProprietario == idLogado || (idLogado == 1 && books[idxLivro].idProprietario == 0))) {
                
                char nomeReq[MAX_STRING] = "Desconhecido";
                for (int u = 0; u < totalUsers; u++) {
                    if (users[u].id == operacoes[i].idRequerente) {
                        strcpy(nomeReq, users[u].nome); 
                        break;
                    }
                }

                char tipoStr[15];
                if (operacoes[i].tipo == OP_TIPO_EMPRESTIMO) strcpy(tipoStr, "EMPRESTIMO");
                else strcpy(tipoStr, "TROCA");

                printf("%-3d | %-12s | %-30.30s | %-20.20s\n", 
                       visualId, tipoStr, books[idxLivro].titulo, nomeReq);
                
                visualId++;
                temPedidos = 1;
            }
        }
    }
    printf("--------------------------------------------------------------------------------\n");

    if (!temPedidos) {
        printf("[Info] Nao tem pedidos pendentes de aprovacao.\n");
        esperarEnter();
        return;
    }

    // 2. Selecionar
    int escolha = lerInteiro("Escolha o pedido a tratar (0 para voltar): ", 0, visualId - 1);
    if (escolha == 0) return;

    // 3. Recuperar Índices Reais
    int cont = 1;
    int idxOp = -1;
    int idxLivro = -1; // O meu livro

    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].estado == ESTADO_OP_PENDENTE) {
            int tempLivroIdx = -1;
            for (int b = 0; b < totalBooks; b++) {
                if (books[b].id == operacoes[i].idLivro) { tempLivroIdx = b; break; }
            }

            if (tempLivroIdx != -1 && (books[tempLivroIdx].idProprietario == idLogado || (idLogado == 1 && books[tempLivroIdx].idProprietario == 0))) {
                if (cont == escolha) {
                    idxOp = i;
                    idxLivro = tempLivroIdx;
                    break;
                }
                cont++;
            }
        }
    }

    if (idxOp != -1 && idxLivro != -1) {
        printf("\n--- Detalhes da Decisao ---\n");
        printf("Livro Solicitado: %s\n", books[idxLivro].titulo);
        
        // Identificar o livro oferecido (caso seja troca)
        int idxLivroOferecido = -1;
        if (operacoes[idxOp].tipo == OP_TIPO_TROCA) {
            for(int b=0; b<totalBooks; b++) {
                if(books[b].id == operacoes[idxOp].idLivroTroca) {
                    idxLivroOferecido = b;
                    printf("Em troca recebe:  '%s' (%s)\n", books[b].titulo, books[b].autor);
                    break;
                }
            }
        } else {
            printf("Tipo: EMPRESTIMO (%d dias)\n", operacoes[idxOp].dias);
        }

        printf("\n1. Aceitar\n2. Recusar\n0. Cancelar\n");
        int acao = lerInteiro("Opcao: ", 0, 2);

        if (acao == 1) { // === ACEITAR ===
            operacoes[idxOp].dataEmprestimo = obterDataAtual(); // Define data de hoje

            // --- CASO A: EMPRÉSTIMO ---
            if (operacoes[idxOp].tipo == OP_TIPO_EMPRESTIMO) {
                // MUITO IMPORTANTE: Passa para ATIVO, não PENDENTE
                operacoes[idxOp].estado = ESTADO_OP_ACEITE; 
                
                // Define data prevista (ex: hoje + dias)
                // Nota: Certifica-te que a tua função obterDataAtual retorna um formato que permita soma simples
                // Se for string, precisas de lógica de datas. Vou assumir inteiro/timestamp aqui.
                // operacoes[idxOp].dataDevolucaoPrevista = somarDias(dataHoje, dias); 
                
                // Atualiza o livro
                books[idxLivro].estado = LIVRO_EMPRESTADO; 
                books[idxLivro].idDetentor = operacoes[idxOp].idRequerente; // Está com quem pediu
                books[idxLivro].numRequisicoes++;
                
                printf("\n[Sucesso] Emprestimo aprovado!\n");
            
            } 
            // --- CASO B: TROCA ---
            else if (operacoes[idxOp].tipo == OP_TIPO_TROCA) {
                operacoes[idxOp].estado = ESTADO_OP_CONCLUIDO; // Troca é final, fica concluído

                // 1. O meu livro passa para ele (e fica Privado/Indisponível na estante dele)
                transferirPosseLivro(books, totalBooks, books[idxLivro].id, operacoes[idxOp].idRequerente);
                books[idxLivro].numRequisicoes++;

                // 2. O livro dele passa para MIM (e fica Privado/Indisponível na minha estante)
                if (idxLivroOferecido != -1) {
                    transferirPosseLivro(books, totalBooks, books[idxLivroOferecido].id, idLogado);
                    books[idxLivroOferecido].numRequisicoes++;
                }

                printf("\n[Sucesso] Troca realizada! Verifique 'Meus Livros' para ver a nova aquisicao.\n");
            }
            
            // Gravar tudo para garantir persistência
            // guardarOperacoes(...);
            // guardarLivros(...);

        } else if (acao == 2) { // === RECUSAR ===
            operacoes[idxOp].estado = ESTADO_OP_REJEITADO;
            
            // O meu livro volta ao mercado
            books[idxLivro].estado = LIVRO_DISPONIVEL;
            
            // O livro dele (se for troca) também volta ao mercado (liberta o RESERVADO)
            if (operacoes[idxOp].tipo == OP_TIPO_TROCA && idxLivroOferecido != -1) {
                books[idxLivroOferecido].estado = LIVRO_DISPONIVEL;
            }
            
            printf("\n[Info] Pedido recusado. Os livros voltaram ao mercado.\n");
        }
        
        esperarEnter();
    }
}

void doarLivro(Livro books[], int totalBooks, int idLogado, Operacao operacoes[], int *totalOperacoes) {
    (void)operacoes; // Evita warning de variável não usada
    (void)totalOperacoes; // Evita warning de variável não usada

    limparEcra();
    printf("\n=== DOAR LIVRO AO IPCA ===\n");
    printf("Escolha qual dos seus livros deseja doar a instituicao:\n");
    printf("%-3s | %-30s | %-20s\n", "N.", "TITULO", "AUTOR");
    printf("----------------------------------------------------------\n");

    int visualId = 1; // O nosso contador sequencial (1, 2, 3...)
    int temLivros = 0;

    // 1. LISTAGEM COM NÚMERO SEQUENCIAL
    for (int i = 0; i < totalBooks; i++) {
        // Filtro: Livros que são meus, estão disponíveis e não eliminados
        if (books[i].idProprietario == idLogado && 
            books[i].estado == LIVRO_DISPONIVEL && 
            books[i].eliminado == 0) {
            
            printf("%-3d | %-30.30s | %-20.20s\n", 
                   visualId,  // Mostramos o 1, 2, 3...
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

    // 2. SELEÇÃO DO UTILIZADOR
    // O utilizador escolhe entre 1 e (visualId - 1)
    int escolha = lerInteiro("\nIntroduza o N. do livro (0 para cancelar): ", 0, visualId - 1);

    if (escolha == 0) return;

    // 3. TRADUÇÃO (Visual -> Real)
    // Temos de encontrar qual é o índice no array que corresponde ao nº escolhido
    int contador = 1;
    int idxEncontrado = -1;

    for (int i = 0; i < totalBooks; i++) {
        if (books[i].idProprietario == idLogado && 
            books[i].estado == LIVRO_DISPONIVEL && 
            books[i].eliminado == 0) {
            
            if (contador == escolha) {
                idxEncontrado = i; // Encontrei a posição real no array!
                break;
            }
            contador++;
        }
    }

    if (idxEncontrado != -1) {
        // Confirmação
        char confirmacao;
        printf("\nTem a certeza que deseja doar o livro '%s'? (S/N): ", books[idxEncontrado].titulo);
        scanf(" %c", &confirmacao); // O espaço antes do %c limpa o buffer

        if (confirmacao == 's' || confirmacao == 'S') {
            
            // === EFETUAR A DOAÇÃO ===
            // Passa a ser propriedade do IPCA (Assumindo ID 1 ou 0)
            books[idxEncontrado].idProprietario = 1; // 1 = Admin/IPCA
            books[idxEncontrado].idDetentor = 1;
            
            // Opcional: Se quiseres mudar o estado para algo específico ou manter DISPONIVEL
            books[idxEncontrado].estado = LIVRO_DISPONIVEL; 

            printf("\n[Sucesso] O livro foi doado ao IPCA. Obrigado pela sua generosidade!\n");
            
            // Gravar alterações
            // guardarLivros(books, totalBooks); // Descomenta se tiveres a função aqui
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

