# Wookinted - Sistema de Gestão de Livros e Transações Institucionais

**Wookinted** é uma aplicação de consola desenvolvida em linguagem C, focada na economia circular de livros dentro da comunidade académica do **IPCA**. O sistema permite que alunos e docentes registem, emprestem, troquem e doem livros, promovendo a partilha de conhecimento de forma organizada e auditada.

---

## 🚀 Funcionalidades Principais

### 👤 Gestão de Utilizadores
- **Autenticação Segura:** Login baseado em email institucional (@alunos.ipca.pt ou @ipca.pt).
- **Níveis de Acesso:** Distinção clara entre **Utilizador Comum** e **Administrador**.
- **Segurança de Conta:** Sistema de recuperação de password através de validação de telemóvel.
- **Reputação:** Ranking de utilizadores baseado em feedbacks (1 a 5 estrelas) após transações.

### 📚 Catálogo de Livros
- **Gestão de Acervo:** Registo completo com validação de ISBN-13.
- **Estados Dinâmicos:** Controlo em tempo real (Disponível, Emprestado, Reservado, Indisponível).
- **Pesquisa Avançada:** Filtros por título, autor e categoria.

### 🔄 Transações e Movimentos
- **Empréstimos:** Pedidos temporários com definição de prazos e alertas de atraso.
- **Trocas:** Propostas de troca direta entre utilizadores.
- **Doações:** Entrega de livros para o acervo institucional (Admin).
- **Workflow de Aprovação:** O proprietário tem total controlo sobre quem aceita para manusear os seus livros.

### 🛡️ Administração e Auditoria
- **Painel de Controlo:** Aprovação de novos registos e reativação de contas.
- **Logs de Sistema:** Registo técnico de todas as ações críticas em ficheiro TXT.
- **Estatísticas:** Listagens de popularidade de livros e atividade de utilizadores.

---

## 🛠️ Arquitetura Técnica

O projeto foi desenhado seguindo princípios de modularidade para facilitar a manutenção:

- **`structs.h`**: Definição dos modelos de dados e estados (Enums).
- **`transactions.c/h`**: Lógica de negócio e ciclo de vida das operações.
- **`users.c/h`**: Motores de autenticação e ranking de reputação.
- **`files.c/h`**: Camada de persistência em ficheiros binários (`.dat`).
- **`utils.c/h`**: Biblioteca de funções utilitárias (validações, datas e limpeza de buffer).
- **`interface.c/h`**: Gestão de menus e interação com o utilizador.



---

## 📋 Pré-requisitos e Instalação

### Compilação
O projeto inclui um **Makefile** para facilitar a compilação no macOS/Linux.

1. Clone o repositório ou extraia os ficheiros.
2. No terminal, navegue até à pasta raiz.
3. Execute o comando para compilar: make

### Execução

Para iniciar o sistema após a compilação: ./wookinted
Ou, alternativamente, use o comando de atalho: make run