#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
//#include "users.h"
//#include "books.h"
#include "transactions.h"
#include "files.h"
#include "interface.h"
#include "structs.h"


int main() {
    // --- Inicialização de Dados ---
    Utilizador users[MAX_UTILIZADORES];
    Livro books[MAX_LIVROS];
    Operacao operacoes[MAX_OPERACOES];
    Feedback feedbacks[MAX_FEEDBACKS];

    int totalUsers = carregarUtilizadores(users);
    int totalBooks = carregarLivros(books);
    int totalOperacoes = carregarOperacoes(operacoes);
    int totalFeedbacks = carregarFeedbacks(feedbacks);

    limparEcra();
    printf("Sistema Wookinted Iniciado. Dados carregados.\n");
    printf("Users: %d | Livros: %d | Emprestimos: %d\n", totalUsers, totalBooks, totalOperacoes);
    esperarEnter();

    int idLogado = -1; // -1 significa ninguém logado
    int running = 1;

    while (running) {
        
        if (idLogado == -1) {
            // === MODO VISITANTE ===
            int resultado = menuModoVisitante(users, &totalUsers, operacoes, totalOperacoes);
            
            if (resultado == -10) {
                running = 0; // O utilizador escolheu Sair

            } else if (resultado >= 0) {
                idLogado = resultado; // Login feito!
                
                // VERIFICAÇÃO DE ADMIN (O Admin não tem saudações no login, por isso tratamos aqui)
                if (strcmp(users[idLogado].email, "admin@ipca.pt") == 0) {
                    
                    verificarNotificacoesAdmin(users, totalUsers, books, totalBooks, operacoes, totalOperacoes);
                    
                    menuAdministrador(users, totalUsers, books, totalBooks, operacoes, totalOperacoes, feedbacks, totalFeedbacks);
                    idLogado = -1; 
                    printf("\nSessao de Administrador terminada.\n");
                    esperarEnter();
                }
            }
        } else {
            // === MODO LOGADO ===
            int opcao = mostrarMenuPrincipal(users[idLogado].nome);

            switch (opcao) {
                case 1: 
                    menuMercadoLivros(users, totalUsers, books, &totalBooks, operacoes, &totalOperacoes, idLogado);
                    break;
                case 2:
                    menuMeusLivros(books, &totalBooks, idLogado);
                    break;
                case 3:
                    menuGestaoMovimentos(users, totalUsers, books, &totalBooks, operacoes, &totalOperacoes, feedbacks, &totalFeedbacks, idLogado);
                    break;
                case 4:
                    menuGestaoPerfil(users, totalUsers, feedbacks, totalFeedbacks, idLogado);

                    // Se a conta for apagada dentro do menu, idLogado volta como -1
                    if (idLogado == -1) {
                        printf("Sessao terminada (Conta eliminada).\n");
                        esperarEnter();
                    }
                    break;
                case 0:
                    idLogado = -1;
                    printf("Logout efetuado com sucesso.\n");
                    esperarEnter();
                    break;
                default:
                    printf("Opcao invalida.\n");
                    esperarEnter();
            }
        }
    }

    // Antes de sair, garante que tudo está salvo (redundância de segurança)
    guardarUtilizadores(users, totalUsers);
    printf("A encerrar sistema... Ate a proxima!\n");
        
    return 0;
}