#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_USERS 50
#define MAX_BOOKS 100
#define MAX_STRING 100
#define MAX_LOANS 200
#define MAX_FEEDBACKS 200

/**
 * @brief Estados da conta do utilizador.
 */
typedef enum {INATIVO, ATIVO, PENDENTE_APROVACAO, PENDENTE_REATIVACAO} EstadoConta;

/** @brief Estados do livro. */
typedef enum { INDISPONIVEL, DISPONIVEL, EMPRESTADO, PENDENTE_CATEGORIA } EstadoLivro;

/** @brief Categorias de livros. */
typedef enum { FICCAO, NAO_FICCAO, CIENCIA, HISTORIA, BIOGRAFIA, TECNOLOGIA, OUTRO } CategoriaLivro;

/** @brief Tipos de operação. */
typedef enum { DOACAO, EMPRESTIMO, TROCA } TipoOperacao;

/** @brief Estados do empréstimo. */
typedef enum { PENDENTE_PROPOSTA, ACEITE, REJEITADO, CONCLUIDO } EstadoEmprestimo;

/** @brief Tipos de pesquisa. */
typedef enum { PESQUISA_TITULO, PESQUISA_AUTOR, PESQUISA_CATEGORIA } TipoPesquisa;

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
    EstadoConta ativo;
} Utilizador;

/** @brief Estrutura que representa um livro. */
typedef struct {
    int id;                 // ID único do livro
    char titulo[MAX_STRING];
    char autor[MAX_STRING];
    CategoriaLivro categoria;
    char categoriaManual[50];  // Categoria personalizada se OUTRO 
    char isbn[20];          // ISBN-13 com hífens
    
    int retido;             // 0 = Ativo, 1 = Eliminado (Soft Delete)
    EstadoLivro Disponivel;

    int userId;             // ID do DONO (Proprietário)
    int userIdEmprestimo;   // ID de quem tem a POSSE atual (Detentor)
    int numRequisicoes;   // Para relatório de mais procurados
} Livro;

/**
 * @brief Estrutura que representa um empréstimo ou troca de livro.
 */
typedef struct {
    int id;
    int userId;             // Quem pede (Requisitante)
    int userIdEmprestimo;   // Quem tem o livro atualmente (Dador)
    int bookId;
    int idOperacao;         // ID auxiliar (opcional)
    
    int dataEmprestimo;     // YYYYMMDD
    int dataDevolucao;      // YYYYMMDD (0 se não devolvido)
    
    EstadoEmprestimo estado;
    TipoOperacao tipoOperacao;
} Operacao;

/** @brief Estrutura de feedback mútuo. */
typedef struct {
    int id;
    int userId;             // Quem recebe o feedback ( redundantemente avaliadoId)
    int loanId;             // Ligação à transação
    int avaliadorId;        // Autor da crítica
    int avaliadoId;         // Alvo da crítica
    int dataAvaliacao;      // YYYYMMDD
    int feedbackId;         // 0 = Original, >0 = Resposta
    int nota;               // 1 a 5
} Feedback;

/** @brief Estrutura para logs de auditoria. */
typedef struct {
    int userId;
    int bookId;
    int idOperacao;
    char tipoOperacao[20];  // Texto descritivo da ação
    int dataOperacao;       // YYYYMMDD
} LogSistema;

#endif // STRUCTS_H