#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"
#include "utils.h"
#include "loans.h"
#include "files.h"

/**
 * @brief Regista um novo livro no sistema.
 * Lógica:
 * - Gera um ID único baseado no total atual.
 * - Associa o livro ao utilizador que o está a registar.
 * - Define o estado inicial como DISPONIVEL e retido como 0 (não retido).
 */
void registarLivro(Livro books[], int total, int idLogado) {
    // Usamos uma variável auxiliar ou escrevemos diretamente no array.
    // Escrever diretamente poupa memória.

    //ALERTA : Simples, mas pode causar colisões se livros forem eliminados.
    books[total].id = total + 1; // 1. Geração do ID
    books[total].userId = idLogado; // 2. Associação ao Utilizador (Dono)
    books[total].userIdEmprestimo = idLogado; // Se userIdEmprestimo serve para dizer "com quem está o livro agora", começa com o dono.

    // 3. Dados do Livro
    printf("Titulo do Livro: ");
    lerString(books[total].titulo, MAX_STRING); 

    printf("Autor: ");
    lerString(books[total].autor, MAX_STRING);

    printf("Ano de Publicacao: ");
    while (scanf("%d", &books[total].anoPublicacao) != 1) {
        printf("Ano invalido. Insira um numero: ");
        limparBuffer(); // Limpa o enter inválido
    }
    limparBuffer(); // Limpa o enter final do scanf

    // 4. Estados Iniciais
    books[total].retido = 0; // 0 = Não retido (Disponível para troca)
    books[total].Disponivel = DISPONIVEL; 

    printf("\n[Sucesso] Livro '%s' (ID: %d) registado no inventario!\n", books[total].titulo, books[total].id);
}

/**
 * @brief Lista todos os livros do sistema (Catálogo Global).
 * Mostra se pertence ao Instituto ou a um Utilizador.
 */
void listarLivros(Livro books[], int total) {
    printf("\n--- Catalogo Geral de Livros ---\n");
    printf("%-4s | %-20s | %-15s | %-10s | %-12s\n", "ID", "Titulo", "Autor", "Dono", "Estado");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0) {
            char estado[20];
            
            // Traduz o estado do Enum para texto legível
            if (books[i].Disponivel == INDISPONIVEL || books[i].Disponivel == EMPRESTADO) {
                strcpy(estado, "EMPRESTADO");
            } else {
                strcpy(estado, "DISPONIVEL");
            }

            // Identifica se o dono é a Instituição (ID 0) ou um User
            char donoStr[15];
            if (books[i].userId == 0) strcpy(donoStr, "IPCA");
            else sprintf(donoStr, "User %d", books[i].userId);

            printf("%-4d | %-20.20s | %-15.15s | %-10s | %-12s\n",
                   books[i].id,
                   books[i].titulo,
                   books[i].autor,
                   donoStr,
                   estado);
        }
    }
}


//Pesquisa através da lista

/**
 * @brief Pesquisa livros por título (parcial).
 * @param books Array de livros.
 * @param total Número total de livros.
 * @param titulo Título ou parte do título a pesquisar.
 */
void pesquisarLivroPorTitulo(Livro books[], int total, const char *titulo) {
    printf("\nResultados da pesquisa por '%s':\n", titulo);
    int found = 0;
    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0 && strstr(books[i].titulo, titulo) != NULL) {
            char donoStr[15];
            if (books[i].userId == 0) strcpy(donoStr, "INSTITUTO");
            else sprintf(donoStr, "User %d", books[i].userId);

            printf("- ID: %d | Titulo: %s | Dono: %s\n", 
                   books[i].id, books[i].titulo, donoStr);
            found = 1;
        }
    }
    if (!found) printf("Nenhum livro encontrado.\n");
}

/**
 * @brief Pesquisa livros por autor (parcial).
 * @param books Array de livros.
 * @param total Número total de livros.
 * @param autor Autor ou parte do nome do autor a pesquisar.
 */
void pesquisarLivroPorAutor(Livro books[], int total, const char *autor) {
    int encontrados = 0;
    printf("\nResultados da pesquisa por autor '%s':\n", autor);
    for (int i = 0; i < total; i++) {
        if (books[i].retido == 0 && strstr(books[i].autor, autor) != NULL) {
             printf("- ID: %d | Titulo: %s | Dono: %d\n", 
                   books[i].id, books[i].titulo, books[i].userId);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum livro encontrado para o autor: %s\n", autor);
    }
}

/**
 * @brief Edita um livro.
 * Segurança: Impede edição se o livro não estiver na posse do dono.
 */
void editarLivro(Livro *book, int userId) {
    // 1. Verificar propriedade
    if (book->userId != userId) {
        printf("Erro: Apenas o dono (User %d) pode editar este livro.\n", book->userId);
        return;
    }

    // 2. Verificar posse (integração com empréstimos)
    if (book->userId != book->userIdEmprestimo) {
        printf("Erro: O livro esta emprestado ao User %d. Recupere-o antes de editar.\n", book->userIdEmprestimo);
        return;
    }

    printf("\n--- Editar Livro ID: %d ---\n", book->id);
    
    printf("Novo Titulo (Atual: %s): ", book->titulo);
    char buffer[MAX_STRING];
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') { // Só altera se o utilizador escrever algo
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->titulo, buffer);
    }

    printf("Novo Autor (Atual: %s): ", book->autor);
    fgets(buffer, MAX_STRING, stdin);
    if (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(book->autor, buffer);
    }

    printf("Novo Ano (Atual: %d): ", book->anoPublicacao);
    int novoAno;
    // Tenta ler int, se falhar consome o buffer
    char check;
    if (scanf("%d%c", &novoAno, &check) != 2 || check != '\n') {
        // Se o user der apenas Enter ou input inválido, mantemos o ano atual (ou ignoramos)
        // Neste caso simples, assumimos que ele não quis mudar se deu enter
    } else {
        book->anoPublicacao = novoAno;
    }
    
    printf("Livro atualizado com sucesso!\n");
}

/**
 * @brief Elimina (Soft Delete) um livro.
 * Segurança: Impede eliminação se o livro estiver emprestado.
 */
int eliminarLivro(Livro *book, int userId) {   
    // 1. Verificar propriedade
    if (book->userId != userId) {
        printf("Erro: Apenas o dono pode eliminar este livro.\n");
        return 0;
    }

    // 2. Verificar posse
    if (book->userId != book->userIdEmprestimo) {
        printf("Erro: Impossivel eliminar! O livro esta atualmente com o Utilizador %d.\n", book->userIdEmprestimo);
        printf("Deve solicitar a devolucao antes de o remover do sistema.\n");
        return 0;
    }

    char confirmacao[15];
    printf("\n!!! ZONA DE PERIGO !!!\n");
    printf("Vai remover '%s' permanentemente.\n", book->titulo);
    printf("Escreva 'ELIMINAR' para confirmar: ");
    
    scanf("%s", confirmacao);
    limparBuffer(); 
    
    if (strcmp(confirmacao, "ELIMINAR") == 0) {
        book->retido = 1; // Soft Delete
        printf("[Sucesso] Livro removido do catalogo.\n");
        return 1; 
    } else {
        printf("[Cancelado] O livro mantem-se ativo.\n");
        return 0; 
    }
}

/**
 * @brief Lista apenas os livros do utilizador logado.
 * Mostra claramente se o livro está "Consigo" ou "Com Outro User".
 */
void listarMeusLivros(Livro books[], int total, int idLogado) {
    printf("\n--- O MEU INVENTARIO ---\n");
    printf("%-5s | %-30s | %-20s | %s\n", "ID", "TITULO", "AUTOR", "ESTADO");
    printf("----------------------------------------------------------------------\n");
    
    int cont = 0;
    for (int i = 0; i < total; i++) {
        // CORREÇÃO: idDono
        if (books[i].userId == idLogado) {
            printf("%-5d | %-30s | %-20s | %s\n", 
                   books[i].id, 
                   books[i].titulo, 
                   books[i].autor, 
                   (books[i].retido == 0 ? "Disponivel" : "Emprestado/Retido"));
            cont++;
        }
    }
    
    if (cont == 0) printf("    (Ainda nao registou livros)\n");
    printf("----------------------------------------------------------------------\n");
}

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















