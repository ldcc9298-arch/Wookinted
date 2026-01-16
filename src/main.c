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
    int totalBooks = carregarLivros(books);
    int totalOperacoes = carregarOperacoes(operacoes);
    int totalFeedbacks = carregarFeedbacks(feedbacks);

    garantirAdminPadrao(users, &totalUsers);

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
                // Aqui tratamos APENAS da inicialização e Notificações
                
                indiceUser = resultado;          
                idLogado = users[resultado].id;  

                // 1. Notificações (Saudação já veio do login)
                if (strcmp(users[indiceUser].email, "admin@ipca.pt") == 0) {
                    verificarNotificacoesAdmin(users, totalUsers, books, totalBooks, operacoes, totalOperacoes);
                } else {
                    verificarNotificacoes(operacoes, totalOperacoes, idLogado);
                }

                esperarEnter();
            }
        } 
        
        // =======================================================
        // CASO 2: JÁ ESTÁ LOGADO (Mostra o Menu Principal)
        // =======================================================
        else { 
            
            // É AQUI QUE O SWITCH TEM DE ESTAR!
            // Assim, quando voltas do Mercado, ele cai aqui e mostra o menu de novo.

            // A. Se for ADMIN
            if (strcmp(users[indiceUser].email, "admin@ipca.pt") == 0) {
                menuAdministrador(users, totalUsers, books, totalBooks, operacoes, totalOperacoes, feedbacks, totalFeedbacks);
                
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
                        // VERIFICA SE TENS OS '&' NOS TOTAIS!
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
                        // Se apagou a conta:
                        if (idLogado == -1) { 
                            indiceUser = -1; 
                            printf("Conta eliminada.\n"); 
                            esperarEnter(); 
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