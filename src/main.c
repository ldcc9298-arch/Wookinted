#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "users.h"
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
    garantirAdminPadrao(users, &totalUsers);
    int totalBooks = carregarLivros(books);
    int totalOperacoes = carregarOperacoes(operacoes);
    int totalFeedbacks = carregarFeedbacks(feedbacks);

    

    limparEcra();
    printf("Sistema Wookinted Iniciado. Dados carregados.\n");
    printf("Users: %d | Livros: %d | Operacoes: %d\n", totalUsers, totalBooks, totalOperacoes);
    esperarEnter();

    int idLogado = -1; // -1 significa ninguém logado
    int running = 1;
    int indiceUser = -1;

    while (running) {
        
        // =======================================================
        // CASO 1: NÃO ESTÁ LOGADO (Faz Login)
        // =======================================================
        if (idLogado == -1) {
            
            // Tenta fazer login ou visita
            int resultado = menuModoVisitante(users, &totalUsers, operacoes, totalOperacoes);
            // Nota: Se usaste a minha sugestão anterior, o menuModoVisitante chama o loginUtilizador

            if (resultado == -10) {
                running = 0; // Sair do programa
            } 
            else if (resultado >= 0) {
                // === LOGIN COM SUCESSO ===
                // resultado aqui é o ID retornado pelo loginUtilizador
                
                int idRetornado = resultado;
                indiceUser = -1;

                // Procurar qual é a posição no array que tem este ID
                for (int i = 0; i < totalUsers; i++) {
                    if (users[i].id == idRetornado) {
                        indiceUser = i;
                        break;
                    }
                }

                if (indiceUser != -1) {
                    idLogado = users[indiceUser].id;

                    // 1. Notificações
                    if (strcmp(users[indiceUser].email, "admin@ipca.pt") == 0) {
                        verificarNotificacoesAdmin(users, totalUsers, books, totalBooks, operacoes, totalOperacoes, feedbacks, totalFeedbacks, 1);
                    } else {
                        verificarNotificacoes(operacoes, totalOperacoes, feedbacks, totalFeedbacks, idLogado);
                    }
                    esperarEnter();
                } else {
                    printf("[Erro] Utilizador nao encontrado no sistema.\n");
                    idLogado = -1;
                    esperarEnter();
                }
            }
        
        // =======================================================
        // CASO 2: JÁ ESTÁ LOGADO (Mostra o Menu Principal)
        // =======================================================
        } else { 
            
            // A. Se for ADMIN
            if (strcmp(users[indiceUser].email, "admin@ipca.pt") == 0) {
                menuAdministrador(users, &totalUsers, books, &totalBooks, operacoes, &totalOperacoes, feedbacks, &totalFeedbacks);
                
                // Quando o Admin sai do seu menu, fazemos logout
                idLogado = -1;
                indiceUser = -1;
                printf("\nSessao de Administrador terminada.\n");
                esperarEnter();
            } 
            
            // B. Se for UTILIZADOR NORMAL
            else {
                int opcao = mostrarMenuPrincipal(users[indiceUser].nome);
                
                switch (opcao) {
                    case 1: 
                        menuMercadoLivros(users, totalUsers, books, &totalBooks, operacoes, &totalOperacoes, idLogado, feedbacks, totalFeedbacks);
                        break;
                    case 2:
                        menuMeusLivros(books, &totalBooks, idLogado);
                        break;
                    case 3:
                        menuGestaoMovimentos(users, totalUsers, books, &totalBooks, operacoes, &totalOperacoes, feedbacks, &totalFeedbacks, idLogado);
                        break;
                    case 4:
                        if (menuGestaoPerfil(users, totalUsers, feedbacks, totalFeedbacks, idLogado) == 1) { 
                            idLogado = -1; 
                            indiceUser = -1; 
                        }
                        break;
                    case 0:
                        idLogado = -1;
                        indiceUser = -1;
                        printf("Logout efetuado.\n");
                        esperarEnter();
                        break;
                    default:
                        printf("Opcao invalida.\n");
                        esperarEnter();
                }
            }
        }
    } // Fim do while

    // Antes de sair, garante que tudo está salvo (redundância de segurança)
    guardarUtilizadores(users, totalUsers);
    printf("A encerrar sistema... Ate a proxima!\n");
        
    return 0;
}