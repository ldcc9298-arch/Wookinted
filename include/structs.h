/**
 * @file structs.h
 * @brief Definições de constantes, enums e estruturas de dados globais.
 * @details Este ficheiro centraliza todos os modelos de dados utilizados no sistema,
 * estabelecendo a base para a persistência e comunicação entre módulos.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef STRUCTS_H
#define STRUCTS_H

/* =============================================================
   CONSTANTES DE DIMENSIONAMENTO
   ============================================================= */
#define MAX_UTILIZADORES 50    /**< Limite máximo de utilizadores no sistema. */
#define MAX_LIVROS 100         /**< Limite máximo de livros no catálogo. */
#define MAX_OPERACOES 200      /**< Capacidade do histórico de transações. */
#define MAX_FEEDBACKS 100      /**< Capacidade do registo de avaliações. */
#define MAX_STRING 100         /**< Tamanho padrão para cadeias de caracteres. */
#define MAX_LOANS 200          /**< Alias para limite de operações. */

/** @defgroup ENUMS Enumerações de Estado e Tipo
 * @brief Definições de estados lógicos para controlo de fluxo.
 * @{ 
 */

/** @brief Estados da conta do utilizador para controlo de acesso. */
typedef enum {
    CONTA_INATIVA = 0,             /**< Conta desativada ou banida. */
    CONTA_ATIVA = 1,               /**< Utilizador com acesso total. */
    CONTA_PENDENTE_APROVACAO = 2,  /**< Novo registo aguardando Admin. */
    CONTA_PENDENTE_REATIVACAO = 3  /**< Pedido de recuperação de conta. */
} EstadoConta;

/** @brief Estados físicos e lógicos do livro no catálogo. */
typedef enum {
    LIVRO_INDISPONIVEL = 0,  /**< Fora de circulação (danificado/removido). */
    LIVRO_DISPONIVEL = 1,    /**< Disponível para novas propostas. */
    LIVRO_EMPRESTADO = 2,    /**< Atualmente com um requerente. */
    LIVRO_RESERVADO = 3      /**< Possui proposta aceite aguardando entrega. */
} EstadoLivro;

/** @brief Categorias de transações possíveis no sistema. */
typedef enum {
    OP_TIPO_DESCONHECIDO = 0, /**< Fallback de segurança. */
    OP_TIPO_EMPRESTIMO = 1,   /**< Cedência temporária. */
    OP_TIPO_TROCA = 2,        /**< Troca definitiva de propriedade. */
    OP_TIPO_DEVOLUCAO = 3,    /**< Encerramento de empréstimo. */
    OP_TIPO_DOACAO = 4        /**< Entrega de livro à instituição (IPCA). */
} TipoOperacao;

/** @brief Ciclo de vida de uma proposta de transação. */
typedef enum {
    ESTADO_OP_PENDENTE = 0,           /**< Aguardando decisão do proprietário. */
    ESTADO_OP_ACEITE = 1,             /**< Proposta aprovada. */
    ESTADO_OP_REJEITADO = 2,          /**< Proposta recusada. */
    ESTADO_OP_CONCLUIDO = 3,          /**< Processo finalizado com sucesso. */
    ESTADO_OP_DEVOLUCAO_PENDENTE = 4  /**< Requerente devolveu, dono deve confirmar. */
} EstadoOperacao;

/** @brief Filtros disponíveis para o motor de busca. */
typedef enum {
    PESQUISA_TITULO = 1,
    PESQUISA_AUTOR = 2,
    PESQUISA_CATEGORIA = 3
} TipoPesquisa;

/** @} */

/** @defgroup MODELOS Estruturas de Dados Principais
 * @brief Definição dos objetos do sistema.
 * @{ 
 */

/** @brief Representação de um Utilizador no sistema. */
typedef struct {
    int id;                         /**< Identificador único. */
    char nome[MAX_STRING];          /**< Nome completo ou alcunha. */
    char email[MAX_STRING];         /**< Email institucional (Chave de Login). */
    char password[50];              /**< Credencial de acesso. */
    char telemovel[15];             /**< Contacto para notificações SMS. */
    char dataNascimento[11];        /**< Formato DD-MM-YYYY. */
    EstadoConta estado;             /**< Estado atual da conta. */
} Utilizador;

/** @brief Representação de um Livro e o seu histórico de posse. */
typedef struct {
    int id;                         /**< Identificador único do exemplar. */
    char titulo[MAX_STRING];        /**< Título da obra. */
    char autor[MAX_STRING];         /**< Autor da obra. */
    char isbn[20];                  /**< Código internacional (apenas números). */
    char categoria[50];             /**< Género ou categoria literária. */
    int eliminado;                  /**< Flag de Soft Delete (0: Ativo, 1: Removido). */
    EstadoLivro estado;             /**< Disponibilidade atual. */
    int idProprietario;             /**< ID do utilizador que detém a posse legal. */
    int idDetentor;                 /**< ID de quem tem o livro fisicamente. */
    int numRequisicoes;             /**< Contador para relatórios de popularidade. */
} Livro;

/** @brief Registo de uma transação entre dois utilizadores. */
typedef struct {
    int id;                         /**< Identificador único da operação. */
    int idRequerente;               /**< ID de quem solicita o livro. */
    int idProprietario;             /**< ID de quem cede o livro. */
    int idLivro;                    /**< ID do livro principal. */
    int idLivroTroca;               /**< ID do livro oferecido em troca (0 se n/a). */
    TipoOperacao tipo;              /**< Empréstimo, Troca, etc. */
    EstadoOperacao estado;          /**< Pendente, Aceite, Concluído, etc. */
    int dias;                       /**< Duração do empréstimo. */
    int dataPedido;                 /**< Data de criação (YYYYMMDD). */
    int dataEmprestimo;             /**< Data de início efetivo. */
    int dataDevolucaoPrevista;      /**< Data limite para entrega. */
    int dataDevolucaoReal;          /**< Data de fecho da devolução. */
    int dataFecho;                  /**< Data de finalização do processo. */
} Operacao;

/** @brief Sistema de avaliação e comentários (Peer-to-peer). */
typedef struct {
    int id;                         /**< ID da avaliação. */
    int idOperacao;                 /**< Operação que originou o feedback. */
    int idAvaliador;                /**< Quem escreve a nota. */
    int idAvaliado;                 /**< Quem recebe a nota. */
    float nota;                     /**< Pontuação de 1.0 a 5.0. */
    char comentario[200];           /**< Observações sobre a experiência. */
    int dataAvaliacao;              /**< Data do registo. */
    int idResposta;                 /**< ID de feedback pai (se for réplica). */
} Feedback;

/** @brief Registo técnico de eventos para auditoria. */
typedef struct {
    int id;                         /**< ID do log. */
    int idUtilizador;               /**< Autor do evento. */
    int dataHora;                   /**< Timestamp (YYYYMMDD). */
    char acao[50];                  /**< Código da ação (ex: "LOGIN"). */
    char detalhes[100];             /**< Descrição textual do evento. */
} LogSistema;

/** @} */

/** @defgroup AUXILIARES Estruturas para Relatórios
 * @brief Estruturas temporárias para processamento estatístico.
 * @{ 
 */

/** @brief Auxiliar para cálculo de rankings de reputação. */
typedef struct { 
    int indexUser;                  /**< Índice no array original. */
    float media;                    /**< Média de notas calculada. */
    int qtd;                        /**< Volume de avaliações recebidas. */
} UserStats;

/** @brief Auxiliar para cálculo de rankings de atividade. */
typedef struct {
    int indexUser;                  /**< Índice no array original. */
    int qtd;                        /**< Total de operações concluídas. */
} UserActivityRanking;

/** @} */

#endif // STRUCTS_H