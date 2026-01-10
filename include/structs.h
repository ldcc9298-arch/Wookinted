#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_USERS 50
#define MAX_STRING 100

/**
 * @brief Estados da conta do utilizador.
 */
typedef enum {INATIVO, ATIVO} EstadoConta;

/**
 * @brief Estrutura que representa um utilizador.
 */
typedef struct {
    int id;
    char nome[MAX_STRING];
    char email[MAX_STRING];
    char password[50];
    char telemovel[15];
    char dataNascimento[11];
    EstadoConta ativo;
} Utilizador;

/**
 * @brief Estados do livro.
 */
typedef enum {INDISPONIVEL, DISPONIVEL, EMPRESTADO} EstadoLivro;

/**
 * @brief Categorias de livros.
 */
typedef enum { FICCAO, NAO_FICCAO, CIENCIA, HISTORIA, BIOGRAFIA, TECNOLOGIA, OUTRO } CategoriaLivro;

/**
 * @brief Estrutura que representa um livro.
 */
typedef struct {
    int id;// ID do livro
    char titulo[MAX_STRING];// Titulo do livro
    char autor[MAX_STRING];// Autor do livro
    int anoPublicacao;// Ano de publicação
    CategoriaLivro categoria; // Categoria do livro
    char isbn[20];// ISBN do livro
    int retido; // 0 = Nao, 1 = Sim
    int userId; // ID do utilizador que detem o livro (se retido)
    int userIdEmprestimo; // ID do utilizador que fez o emprestimo
    EstadoLivro Disponivel;// Disponibilidade do livro
} Livro;

/**
 * @brief Tipos de operação.
 */
typedef enum {DOACAO, EMPRESTIMO, TROCA} TipoOperacao;

/**
 * @brief Estados do empréstimo.
 */
typedef enum {PENDENTE, ACEITE, REJEITADO, CONCLUIDO} EstadoEmprestimo;


/**
 * @brief Estrutura que representa um empréstimo ou troca de livro.
 */
typedef struct {
    int id; // ID do emprestimo
    int userId;// Quem pediu o livro
    int userIdEmprestimo;// Quem tem o livro (dador)
    int bookId;// ID do livro
    int idOperacao; // ID da operacao de emprestimo/devolucao
    int dataEmprestimo; // Formato YYYYMMDD
    int dataDevolucao; // Formato YYYYMMDD
    EstadoEmprestimo estado;// Estado do emprestimo
    TipoOperacao tipoOperacao; // Tipo de operacao
} Emprestimo;

/**
 * @brief Estrutura que representa um feedback de empréstimo.
 */
typedef struct {
    int id;// ID do feedback
    int userId;// Quem recebeu o feedback (Dador)
    int loanId;// NOVO: ID do empréstimo ao qual este feedback se refere
    int avaliadorId;// Quem está a avaliar (Recetor)
    int avaliadoId;// Quem está a ser avaliado (Dador)
    int dataAvaliacao; // Formato YYYYMMDD
    int feedbackId;// ID do feedback ao qual este é resposta (0 se for original)
    int nota;// Nota da experiência (1-5)
} Feedback;

/**
 * @brief Estrutura para logs do sistema.
 */
typedef struct {
    int userId;// ID do utilizador
    int bookId;// ID do livro
    int idOperacao;// ID da operacao de emprestimo/devolucao
    char tipoOperacao[10]; // "emprestimo" ou "devolucao" ou "doacao"
    int dataOperacao; // Formato YYYYMMDD
} LogSistema;


typedef enum { PESQUISA_TITULO, PESQUISA_AUTOR } TipoPesquisa;

#endif // STRUCTS_H