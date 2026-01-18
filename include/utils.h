/**
 * @file utils.h
 * @brief Protótipos das funções utilitárias e de validação.
 * @details Este módulo contém funções transversais ao sistema, como manipulação de strings,
 * cálculos de datas, validações de input e gestão de logs.
 * @author Leandro Castro
 * @date 2026
 */

#ifndef UTILS_H
#define UTILS_H

#include "structs.h"

/** @defgroup INPUT_OUTPUT Gestão de Entrada e Saída
 * @brief Funções para leitura segura de dados e manipulação de terminal.
 * @{ 
 */

/** @brief Limpa o buffer do teclado (stdin). */
void limparBuffer();

/**
 * @brief Lê uma string do stdin com proteção contra overflow.
 * @param texto Buffer de destino.
 * @param tamanho Tamanho máximo a ler.
 */
void lerString(char *texto, int tamanho);

/**
 * @brief Lê um número inteiro com validação de intervalo.
 * @param mensagem Texto a exibir.
 * @param min Valor mínimo aceite.
 * @param max Valor máximo aceite.
 * @return int O valor validado.
 */
int lerInteiro(char *mensagem, int min, int max);

/**
 * @brief Lê um valor decimal (float) com validação de intervalo.
 * @param mensagem Texto a exibir.
 * @param min Valor mínimo.
 * @param max Valor máximo.
 * @return float O valor validado.
 */
float lerFloat(char* mensagem, float min, float max);

/** @brief Limpa o ecrã do terminal (compatível com Windows/Linux). */
void limparEcra();

/** @brief Pausa a execução até que a tecla ENTER seja pressionada. */
void esperarEnter();

/** @} */

/** @defgroup VALIDACOES Validações de Dados e Formatos
 * @brief Funções que garantem a integridade e qualidade da informação introduzida.
 * @{ 
 */

/** @brief Verifica se a string contém caracteres visíveis. @return 1 se válido. */
int stringNaoVazia(char *str);

/** @brief Valida se a string contém apenas caracteres alfabéticos. */
int validarApenasLetras(char *nome, int tamanho);

/** @brief Verifica se a string é composta apenas por dígitos numéricos. */
int validarApenasNumeros(const char *str);

/** @brief Valida o formato e dígito de controlo de um ISBN-13. */
int validarISBN(const char *isbn);

/** @brief Verifica se um telemóvel segue o padrão português (9 dígitos, começa por 9). */
int validarTelemovelPortugues(char *tel);

/** @brief Valida se a string é alfanumérica (sem caracteres especiais). */
int validarAlfanumerico(char *str);

/** @brief Valida o formato de data "DD-MM-YYYY". */
int validarData(char *data);

/** @} */

/** @defgroup DATA_TEMPO Manipulação de Datas e Cronologia
 * @brief Lógica para cálculos temporais e formatação de datas.
 * @{ 
 */

/** @brief Obtém a data do sistema no formato inteiro YYYYMMDD. */
int obterDataAtual();

/** @brief Converte data YYYYMMDD para string legível "DD/MM/AAAA". */
void formatarData(int dataInt, char *buffer);

/** @brief Adiciona um intervalo de dias a uma data YYYYMMDD. */
int somarDias(int dataInicio, int dias);

/** @brief Calcula a idade em anos a partir de uma data "DD-MM-YYYY". */
int calcularIdade(char *dataNascimento);

/** @} */

/** @defgroup DIVERSOS Funções de Sistema e Auxiliares
 * @{ 
 */

/** @brief Verifica a existência de um ISBN no catálogo atual. */
int existeISBN(Livro books[], int totalBooks, char *isbn);

/** @brief Normaliza uma string para comparação (Case-insensitive). */
void paraMinusculas(const char *origem, char *destino);

/** @brief Verifica se um utilizador tem operações pendentes ou concluídas. */
int verificarHistoricoUtilizador(int idUser, Operacao operacoes[], int totalOperacoes);

/**
 * @brief Regista uma ocorrência no ficheiro de auditoria (log.bin).
 * @param idUser ID do utilizador responsável.
 * @param acao Código da ação.
 * @param detalhes Descrição do evento.
 */
void registarLog(int idUser, char *acao, char *detalhes);

/** @} */

#endif // UTILS_H