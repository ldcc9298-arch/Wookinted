#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_UTILIZADORES 50
#define MAX_LIVROS 100
#define MAX_OPERACOES 200
#define MAX_FEEDBACKS 100
#define MAX_STRING 100
#define MAX_LOANS 200

/**
 * @brief Estados da conta do utilizador.
 * Recomendação: Usar 0 para inativo ou pendente faz com que contas recém-criadas (que iniciam a zero) não fiquem "Ativas" por engano.
 */
typedef enum {
    CONTA_INATIVA = 0, 
    CONTA_ATIVA = 1, 
    CONTA_PENDENTE_APROVACAO = 2, 
    CONTA_PENDENTE_REATIVACAO = 3
} EstadoConta;

/** @brief Estados do livro. */
typedef enum {
    LIVRO_INDISPONIVEL = 0,  // Ex: Danificado/Removido
    LIVRO_DISPONIVEL = 1,    // Visível no mercado
    LIVRO_EMPRESTADO = 2,    // Fisicamente fora
    LIVRO_RESERVADO = 3      // Tem uma proposta ativa
} EstadoLivro;

/** @brief Tipos de operação. */
typedef enum {
    OP_TIPO_DESCONHECIDO = 0, // Segurança (para inits a zero)
    OP_TIPO_EMPRESTIMO = 1,
    OP_TIPO_TROCA = 2,
    OP_TIPO_DEVOLUCAO = 3,
    OP_TIPO_DOACAO = 4
} TipoOperacao;

/** @brief Estados do empréstimo/troca. */
typedef enum {
    ESTADO_OP_PENDENTE = 0,  // Default (útil para novas operações)
    ESTADO_OP_ACEITE = 1,    // Aprovado
    ESTADO_OP_REJEITADO = 2, // Recusado
    ESTADO_OP_CONCLUIDO = 3,  // Devolvido/Finalizado
    ESTADO_OP_DEVOLUCAO_PENDENTE = 4 // Devolução aguardando confirmação do dono
} EstadoOperacao;

/** @brief Tipos de pesquisa. */
typedef enum {
    PESQUISA_TITULO = 1,    // Começar em 1 facilita menus
    PESQUISA_AUTOR = 2,
    PESQUISA_CATEGORIA = 3
} TipoPesquisa;

// =============================================================
// ESTRUTURAS
// =============================================================

/** @brief Estrutura que representa um utilizador. */
typedef struct {
    int id;
    char nome[MAX_STRING];
    char email[MAX_STRING];
    char password[50];
    char telemovel[15];
    char dataNascimento[11]; // DD-MM-YYYY

    EstadoConta estado;
} Utilizador;

/** @brief Estrutura que representa um livro. */
typedef struct {
    int id;                
    char titulo[MAX_STRING];
    char autor[MAX_STRING];
    char isbn[20];              // Sugestão: Guardar apenas números para facilitar busca

    char categoria[50];         // Alterado: Agora recebe o texto direto da BD ou input
    
    int eliminado;              // 0 = Visível, 1 = Soft Delete
    EstadoLivro estado;         // Enum (DISPONIVEL, EMPRESTADO, etc.)

    int idProprietario;         // Quem registou o livro
    int idDetentor;             // Com quem está fisicamente (útil para empréstimos)

    int numRequisicoes;         // Contador de popularidade
} Livro;

/**
 * @brief Estrutura que representa um empréstimo ou troca de livro.
 */
typedef struct {
    int id;                     // ID único da operação

    int idRequerente;           // Quem pede (Requisitante)
    int idProprietario;         // Quem tem o livro (Dono)
    int idLivro;                // Qual o livro (bookId)
    int idLivroTroca;          // Se for troca, qual o livro oferecido (0 se não for troca)
    
    TipoOperacao tipo;          // Enum (EMPRESTIMO, TROCA, DEVOLUCAO)
    EstadoOperacao estado;      // Enum (PENDENTE, ACEITE, CONCLUIDO)

    int dias;                   // Duração pedida (0 se for Troca)

    int dataPedido;             // Criação do pedido
    int dataEmprestimo;         // Quando foi aceite
    int dataDevolucaoPrevista;  // Calculado: dataEmprestimo + dias
    int dataDevolucaoReal;      // Quando foi realmente devolvido/concluído
    int dataFecho;              // Quando a operação foi fechada (rejeitada/concluída)
} Operacao;

/** @brief Estrutura de feedback mútuo. */
typedef struct {
    int id;
    int idOperacao;         // A qual empréstimo/troca isto se refere

    int idAvaliador;        // Autor da crítica
    int idAvaliado;         // Alvo da crítica

    float nota;               // 1 a 5
    char comentario[200];   // Texto opcional

    int dataAvaliacao;      // DD/MM/YYYY
    int idResposta;         // 0 = É original. >0 = É resposta a outro feedback
} Feedback;

/** @brief Estrutura para logs de auditoria. */
typedef struct {
    int id;
    int idUtilizador;       // Quem fez a ação
    int dataHora;           // Timestamp da ação

    char acao[50];          // Texto descritivo da ação
    char detalhes[100];     // Informação adicional
} LogSistema;

#endif // STRUCTS_H