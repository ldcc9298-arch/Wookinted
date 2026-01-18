#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "books.h"
#include "transactions.h"
#include "utils.h"
#include "files.h"
#include "structs.h"

/* =============================================================
   LÓGICA DE PROCESSAMENTO DE PEDIDOS (ACEITAR/RECUSAR)
   ============================================================= */

void processarAceitacao(Operacao *op, Livro books[], int totalBooks, Operacao operacoes[], int *totalOperacoes, int idLogado) {
    int dataHoje = obterDataAtual(); // Obtém a data do sistema para o fecho ou início da posse
    op->dataEmprestimo = dataHoje; // Marca o momento em que o livro muda de mãos

    // Localiza os índices reais dos livros envolvidos para alterar os seus estados
    int idxLivro = -1, idxLivroOferecido = -1;
    for(int b=0; b<totalBooks; b++) {
        if(books[b].id == op->idLivro) idxLivro = b;
        if(op->tipo == OP_TIPO_TROCA && books[b].id == op->idLivroTroca) idxLivroOferecido = b;
    }

    if (idxLivro == -1) return; // Aborta por segurança se o livro não for encontrado

    char detalheLog[250]; 
    int dia = dataHoje % 100, mes = (dataHoje % 10000) / 100, ano = dataHoje / 10000;

    if (op->tipo == OP_TIPO_EMPRESTIMO) {
        op->estado = ESTADO_OP_ACEITE; // Define operação como ativa
        books[idxLivro].estado = LIVRO_EMPRESTADO; // Bloqueia o livro no catálogo global
        books[idxLivro].idDetentor = op->idRequerente; // Transfere a posse física
        books[idxLivro].numRequisicoes++; // Incrementa popularidade do título
        
        // Calcula a data limite somando os dias acordados à data atual
        int dtLimite = somarDias(dataHoje, op->dias);
        op->dataDevolucaoPrevista = dtLimite;

        sprintf(detalheLog, "EMPRESTIMO ACEITE: %s ao ID %d em %02d/%02d/%04d", 
                books[idxLivro].titulo, op->idRequerente, dia, mes, ano);
        registarLog(idLogado, "EMPRESTIMO_APROVADO", detalheLog);

        printf("\n[Sucesso] Emprestimo aprovado!\n");
    } 
    else if (op->tipo == OP_TIPO_TROCA) {
        op->estado = ESTADO_OP_CONCLUIDO; // Troca conclui-se imediatamente na aceitação
        op->dataFecho = dataHoje;

        // Cria uma operação "espelho" para que ambos os users fiquem com o registo de troca no histórico
        if (*totalOperacoes < MAX_OPERACOES) {
            int newIdx = *totalOperacoes;
            operacoes[newIdx].id = (*totalOperacoes > 0) ? operacoes[*totalOperacoes-1].id + 1 : 1;
            operacoes[newIdx].tipo = OP_TIPO_TROCA;
            operacoes[newIdx].estado = ESTADO_OP_CONCLUIDO;
            operacoes[newIdx].idLivro = op->idLivroTroca;
            operacoes[newIdx].idProprietario = op->idRequerente;
            operacoes[newIdx].idRequerente = idLogado;
            operacoes[newIdx].dataPedido = op->dataPedido;
            operacoes[newIdx].dataFecho = dataHoje;
            (*totalOperacoes)++;
        }

        // Alteração definitiva de propriedade e posse física para os dois livros
        books[idxLivro].idProprietario = op->idRequerente;
        books[idxLivro].idDetentor = op->idRequerente;
        books[idxLivro].estado = LIVRO_DISPONIVEL; // Disponível na estante do novo dono
        books[idxLivro].numRequisicoes++;

        if (idxLivroOferecido != -1) {
            books[idxLivroOferecido].idProprietario = idLogado;
            books[idxLivroOferecido].idDetentor = idLogado;
            books[idxLivroOferecido].estado = LIVRO_DISPONIVEL;
            books[idxLivroOferecido].numRequisicoes++;

            sprintf(detalheLog, "Livro %s trocado por %s com Utilizador ID %d", 
                    books[idxLivro].titulo, books[idxLivroOferecido].titulo, op->idRequerente);
            registarLog(idLogado, "TROCA_CONCLUIDA", detalheLog);
        }
        printf("\n[Sucesso] Troca concluida com sucesso!\n");
    }
}

void processarRejeicao(Operacao *op, Livro books[], int totalBooks, int idLogado) {
    op->estado = ESTADO_OP_REJEITADO; // Marca o pedido como recusado
    op->dataFecho = obterDataAtual();
    
    // Devolve os livros ao estado DISPONÍVEL para que outros possam requisitar
    for(int b=0; b<totalBooks; b++) {
        if(books[b].id == op->idLivro) books[b].estado = LIVRO_DISPONIVEL;
        if(op->tipo == OP_TIPO_TROCA && books[b].id == op->idLivroTroca) books[b].estado = LIVRO_DISPONIVEL;
    }

    registarLog(idLogado, "TRANSACAO_REJEITADA", "Pedido recusado pelo proprietario.");
    printf("\n[Info] Pedido recusado e livros libertados para o mercado.\n");
}

/* =============================================================
   GESTÃO DE DEVOLUÇÕES E HISTÓRICO
   ============================================================= */

void executarDevolucao(Livro *livro, Operacao operacoes[], int totalOperacoes, int idLogado) {
    int dataHoje = obterDataAtual();
    int idDoDono = livro->idProprietario;

    // Encerra a operação de empréstimo ativa ligada a este livro e utilizador
    for (int k = 0; k < totalOperacoes; k++) {
        if (operacoes[k].idLivro == livro->id && operacoes[k].idRequerente == idLogado && operacoes[k].estado == ESTADO_OP_ACEITE) {
            operacoes[k].estado = ESTADO_OP_CONCLUIDO;
            operacoes[k].dataDevolucaoReal = dataHoje;
            operacoes[k].dataFecho = dataHoje;
            break;
        }
    }

    livro->estado = LIVRO_DISPONIVEL; // Livro volta a ficar livre para o mercado
    livro->idDetentor = idDoDono; // A posse física regressa ao proprietário legal

    char detalhe[150];
    sprintf(detalhe, "Livro %s devolvido ID %d", livro->titulo, idDoDono);
    registarLog(idLogado, "LIVRO_DEVOLVIDO", detalhe); // Log de quem devolveu
    registarLog(idDoDono, "LIVRO_RECEBIDO", detalhe); // Log de quem recebeu
}

void registarDevolucao(Operacao operacoes[], int totalOperacoes, Livro livros[], int totalLivros, int idLivro, int idUtilizador) {
    int idxOp = -1;
    // Procura o registo de empréstimo ativo correspondente
    for (int i = 0; i < totalOperacoes; i++) {
        if (operacoes[i].idLivro == idLivro && operacoes[i].idRequerente == idUtilizador && operacoes[i].estado == ESTADO_OP_ACEITE) {
            idxOp = i;
            break;
        }
    }

    if (idxOp == -1) {
        printf("[Erro] Nao existe emprestimo ativo deste livro para este utilizador.\n");
        return;
    }

    operacoes[idxOp].estado = ESTADO_OP_CONCLUIDO;
    operacoes[idxOp].dataDevolucaoReal = obterDataAtual();

    // Atualiza o estado do livro no catálogo
    for (int i = 0; i < totalLivros; i++) {
        if (livros[i].id == idLivro) {
            livros[i].estado = LIVRO_DISPONIVEL;
            livros[i].idDetentor = livros[i].idProprietario;
            printf("[Sucesso] O livro '%s' foi devolvido ao proprietario.\n", livros[i].titulo);
            break;
        }
    }
    esperarEnter();
}

void adicionarOperacao(Operacao operacoes[], int *totalOperacoes, int idRequerente, int idLivro, TipoOperacao tipo, int dias) {
    if (*totalOperacoes >= MAX_OPERACOES) {
        printf("[Erro] Limite de operacoes atingido.\n");
        return;
    }

    int i = *totalOperacoes;
    operacoes[i].id = i + 1; // Gera ID incremental
    operacoes[i].idRequerente = idRequerente;
    operacoes[i].idLivro = idLivro;
    operacoes[i].tipo = tipo;
    operacoes[i].dias = dias;
    operacoes[i].estado = ESTADO_OP_PENDENTE;
    operacoes[i].dataPedido = obterDataAtual();
    operacoes[i].idProprietario = 0; 
    operacoes[i].dataEmprestimo = 0;
    operacoes[i].dataFecho = 0;
    (*totalOperacoes)++;
}

/* =============================================================
   GESTÃO DE FEEDBACKS E AVALIAÇÕES
   ============================================================= */

void processarNovoFeedback(Feedback feeds[], int *totalFeeds, Operacao *op, int idLogado, float nota, char *comentario) {
    if (*totalFeeds >= MAX_FEEDBACKS) {
        printf("\n[Erro] Base de dados de feedback cheia.\n");
        return;
    }

    feeds[*totalFeeds].id = (*totalFeeds > 0) ? feeds[*totalFeeds - 1].id + 1 : 1;
    feeds[*totalFeeds].idOperacao = op->id;
    feeds[*totalFeeds].idAvaliador = idLogado;
    
    // Determina quem é o alvo (idAvaliado) com base em quem está a escrever o feedback
    feeds[*totalFeeds].idAvaliado = (idLogado == op->idRequerente) ? op->idProprietario : op->idRequerente;
    
    feeds[*totalFeeds].nota = nota;
    strncpy(feeds[*totalFeeds].comentario, comentario, 199);
    feeds[*totalFeeds].comentario[199] = '\0';
    feeds[*totalFeeds].dataAvaliacao = obterDataAtual();

    char msgLog[250]; 
    sprintf(msgLog, "NOTA %.1f ao ID %d na Operacao ID %d", nota, feeds[*totalFeeds].idAvaliado, op->id);
    registarLog(idLogado, "FEEDBACK_ADD", msgLog);

    (*totalFeeds)++;
    printf("\n[Sucesso] Obrigado! A sua avaliacao ajuda a comunidade.\n");
}

int jaAvaliou(Feedback feeds[], int totalFeeds, int idOperacao, int idAvaliador) {
    // Evita duplicados: percorre os feedbacks para ver se este user já avaliou esta transação
    for (int i = 0; i < totalFeeds; i++) {
        if (feeds[i].idOperacao == idOperacao && feeds[i].idAvaliador == idAvaliador) {
            return 1; // Já existe avaliação
        }
    }
    return 0; // Avaliação disponível
}

/* =============================================================
   CRIAÇÃO DE REGISTOS (MÓDULOS DE TRANSAÇÃO)
   ============================================================= */

void criarRegistoEmprestimo(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, int idxLivro, int idLogado, int dias) {
    int i = *totalOperacoes;
    operacoes[i].id = (i > 0) ? operacoes[i - 1].id + 1 : 1;
    operacoes[i].idRequerente = idLogado;
    operacoes[i].idProprietario = books[idxLivro].idProprietario;
    operacoes[i].idLivro = books[idxLivro].id;
    operacoes[i].idLivroTroca = 0;
    operacoes[i].tipo = OP_TIPO_EMPRESTIMO;
    operacoes[i].dias = dias;
    operacoes[i].estado = ESTADO_OP_PENDENTE;
    operacoes[i].dataPedido = obterDataAtual();
    (*totalOperacoes)++;

    books[idxLivro].estado = LIVRO_RESERVADO; // Bloqueia o livro para evitar pedidos simultâneos
    
    guardarOperacoes(operacoes, *totalOperacoes);
    guardarLivros(books, totalBooks);

    char detalheOperacao[200];
    sprintf(detalheOperacao, "Pedido de EMPRESTIMO, Livro %s por %d dias a ID %d", 
            books[idxLivro].titulo, dias, books[idxLivro].idProprietario);
    registarLog(idLogado, "EMPRESTIMO_PEDIDO", detalheOperacao);
}

void criarRegistoTroca(Operacao operacoes[], int *totalOperacoes, Livro books[], int totalBooks, int idxLivroAlvo, int idxMeuLivro, int idLogado) {
    int i = *totalOperacoes;
    operacoes[i].id = (i > 0) ? operacoes[i - 1].id + 1 : 1;
    operacoes[i].idRequerente = idLogado;
    operacoes[i].idProprietario = books[idxLivroAlvo].idProprietario;
    operacoes[i].idLivro = books[idxLivroAlvo].id;         
    operacoes[i].idLivroTroca = books[idxMeuLivro].id; 
    operacoes[i].tipo = OP_TIPO_TROCA;
    operacoes[i].estado = ESTADO_OP_PENDENTE;
    operacoes[i].dataPedido = obterDataAtual();
    (*totalOperacoes)++;

    // Bloqueia ambos os títulos envolvidos na troca
    books[idxLivroAlvo].estado = LIVRO_RESERVADO;
    books[idxMeuLivro].estado = LIVRO_RESERVADO;

    guardarOperacoes(operacoes, *totalOperacoes);
    guardarLivros(books, totalBooks);
    
    char detalheOperacao[250];
    sprintf(detalheOperacao, "Livro %s trocado por %s com Utilizador ID %d", 
            books[idxMeuLivro].titulo, books[idxLivroAlvo].titulo, books[idxLivroAlvo].idProprietario);
    registarLog(idLogado, "TROCA_PROPOSTA", detalheOperacao);
}