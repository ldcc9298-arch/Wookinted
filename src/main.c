#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "files.h"
#include "users.h"
#include "books.h"
#include "transactions.h"
#include "interface.h"

// Definir tamanhos máximos para os arrays
#define MAX_BOOKS 100
#define MAX_LOANS 200
#define MAX_FEEDBACKS 200

int main() {
    // --- Inicialização de Dados ---
    Utilizador users[MAX_USERS];
    Livro books[MAX_BOOKS];
    Emprestimo loans[MAX_LOANS];
    Feedback feedbacks[MAX_FEEDBACKS];

    int totalUsers = carregarUtilizadores(users);
    int totalBooks = carregarLivros(books);
    int totalLoans = carregarEmprestimos(loans);
    int totalFeedbacks = carregarFeedbacks(feedbacks);

    limparEcra();
    printf("Sistema Wookinted Iniciado. Dados carregados.\n");
    printf("Users: %d | Livros: %d | Emprestimos: %d\n", totalUsers, totalBooks, totalLoans);
    esperarEnter();

    int idLogado = -1; // -1 significa ninguém logado
    int running = 1;

    while (running) {
        

        
        if (idLogado == -1) {
            // === MODO VISITANTE (Gerido pelo users.c) ===
            int resultado = menuModoVisitante(users, &totalUsers);
            
            if (resultado == -10) {
                running = 0; // O utilizador escolheu Sair

            } else if (resultado >= 0) {
                idLogado = resultado; // Login feito!
                
                // VERIFICAÇÃO DE TIPO DE UTILIZADOR
                char *email = users[idLogado].email;
                
                // Verifica se é Admin
                if (strcmp(email, "admin@ipca.pt") == 0) {
                    menuAdministrador(users, totalUsers);
                    // Opcional: idLogado = -1; se quiseres logout após admin
                }
                else {
                    // Verifica se é Aluno (procura "alunos.ipca.pt" no email)
                    if (strstr(email, "alunos.ipca.pt") != NULL) {
                        printf("\n[Login OK] Bem-vindo, Aluno %s!\n", users[idLogado].nome);
                    } 
                    // Se não é aluno nem admin, e passou a validação IPCA, é Docente/Staff
                    else {
                        printf("\n[Login OK] Bem-vindo, Docente %s!\n", users[idLogado].nome);
                    }

                    // Verificar notificações logo após o login
                    verificarNotificacoes(loans, totalLoans, idLogado);
                }
            }

        } else {
            // === MODO LOGADO (Maestro Main) ===
            int opcao = mostrarMenuPrincipal(users[idLogado].nome);

            switch (opcao) {
                case 1: 
                    menuMercadoLivros(books, &totalBooks, loans, &totalLoans, idLogado);
                    break;
                case 2:
                    menuMeusLivros(books, &totalBooks, idLogado);
                    break;
                case 3:
                    menuGestaoMovimentos(loans, &totalLoans, books, &totalBooks, feedbacks, &totalFeedbacks, idLogado);
                    break;
                case 4:
                    printf("--- MEU PERFIL ---\n");
                    menuGestaoPerfil(users, totalUsers, &idLogado);
                    listarFeedbacks(feedbacks, totalFeedbacks, idLogado);
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