# 📚 Wookinted - Sistema de Gestão de Biblioteca Partilhada

**Wookinted** é uma aplicação em C desenvolvida para facilitar a troca, empréstimo e doação de livros dentro da comunidade académica do IPCA.

---

## 🚀 Funcionalidades Principais

### 👤 Gestão de Utilizadores
* **Registo e Login:** Autenticação segura com validação de emails institucionais (`@ipca.pt`, `@alunos.ipca.pt`).
* **Recuperação de Conta:** Sistema de segurança para redefinir passwords.
* **Perfis:** Edição de dados e histórico de reputação.

### 📖 Mercado de Livros
* **Catálogo Global:** Pesquisa avançada por Título, Autor ou Categoria.
* **Inventário Pessoal:** Os utilizadores podem registar os seus próprios livros para disponibilizar à comunidade.
* **Estado do Livro:** Gestão automática de estados (`Disponível`, `Emprestado`, `Reservado`).

### 🤝 Transações e Movimentos
* **Empréstimos:** Pedidos com datas de devolução definidas.
* **Trocas:** Mecanismo de troca direta de livros entre utilizadores.
* **Doações:** Possibilidade de doar livros ao espólio da Instituição.
* **Sistema de Aprovação:** Os donos dos livros aceitam ou recusam os pedidos recebidos.

### ⭐ Sistema de Reputação
* **Feedback:** Avaliação (1 a 5 estrelas) e comentários após cada transação.
* **Média:** Cálculo automático da reputação de cada utilizador.

### 🛡️ Painel de Administrador
* Validação de novas contas.
* Monitorização de logs de atividade.
* Relatórios estatísticos (Top Users, Top Livros).

---

## 🛠️ Como Compilar e Executar

### Pré-requisitos
* Compilador GCC.

### Compilação
Abra o terminal na pasta do projeto e execute:

```bash
gcc main.c users.c books.c transactions.c interface.c files.c utils.c -o wookinted