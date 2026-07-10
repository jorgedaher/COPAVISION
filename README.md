# 🏆 COPA VISION 2026

Um simulador completo da Copa do Mundo 2026! O sistema possui um **backend em C++** super rápido para simular o torneio, gerenciar dados das 48 seleções e histórico, e um **frontend web em React** moderno com design imersivo para a melhor experiência.

---

## 🚀 Como rodar o projeto na sua máquina

Para rodar este projeto, você precisará iniciar dois servidores simultaneamente em terminais separados: o Servidor C++ (Backend) e o Servidor React (Frontend).

### Pré-requisitos
Antes de começar, certifique-se de ter os seguintes programas instalados no Windows:
1. **Compilador C++ (g++)**: Necessário para rodar o backend. (Instalado geralmente via MinGW).
2. **Node.js**: Necessário para rodar o painel Web em React. (Recomendado versão 18+).
3. **Git**: Para clonar este repositório.

---

### Passo 1: Clonar o Repositório

Abra seu terminal (PowerShell ou Prompt de Comando) e digite:
```bash
git clone https://github.com/jorgedaher/COPAVISION.git
cd COPAVISION
```

---

### Passo 2: Iniciar o Servidor Backend (C++)

O backend C++ é responsável por toda a lógica matemática, sorteios e gerenciamento dos dados do torneio via uma API REST.

1. Abra um terminal dentro da pasta `COPAVISION`
2. Execute o script de inicialização do servidor:
   ```powershell
   .\build_server.ps1
   ```
3. O script vai automaticamente compilar o arquivo `server.cpp` e iniciar o executável.
4. Você deverá ver a mensagem indicando que o servidor está rodando em `http://localhost:8080`.
*(Mantenha esse terminal aberto enquanto estiver usando o programa).*

---

### Passo 3: Iniciar o Frontend Web (React)

A interface gráfica vai se comunicar com o seu servidor C++ local para exibir tudo de forma visual.

1. Abra um **novo terminal** (separado do terminal do C++) e entre na pasta do frontend:
   ```bash
   cd copa-vision-web
   ```
2. Instale as dependências do projeto (isso só é necessário na primeira vez que rodar):
   ```bash
   npm install
   ```
3. Inicie o servidor de desenvolvimento:
   ```bash
   npm run dev
   ```
4. O terminal vai exibir uma URL, geralmente **`http://localhost:5173`**. Clique nela segurando `Ctrl` para abrir a simulação no seu navegador!

---

## 🛠️ Resolução de Problemas Comuns (Troubleshooting)

**1. Erro ao rodar `npm install` ou `npm run dev` no PowerShell (Texto vermelho)**
Isso acontece porque o Windows bloqueia a execução de scripts `.ps1` por padrão.
* **Solução Rápida:** Sempre que for rodar comandos do npm no Windows, digite `npm.cmd` em vez de apenas `npm`. (Exemplo: `npm.cmd install` e depois `npm.cmd run dev`).
* **Solução Definitiva:** No PowerShell, execute o comando abaixo para autorizar scripts na sua janela atual:
  `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass`

**2. Erro de porta ocupada (Port already in use)**
Se a porta 8080 ou 5173 já estiverem sendo usadas por outro aplicativo na sua máquina, encerre o aplicativo conflitante, ou verifique no Gerenciador de Tarefas se já não há um `server.exe` ou `node.exe` rodando silenciosamente no fundo.

**3. Falta do compilador `g++`**
Se o passo 2 falhar dizendo que `g++` não foi reconhecido, significa que você não tem o MinGW/GCC instalado no seu "PATH" do Windows. Você precisará baixar o MinGW e configurá-lo no seu Windows antes de prosseguir.

---

### 🎨 Sobre o Projeto
* **Backend:** C++ (Puro 17), `cpp-httplib` (API REST). Sem dependências pesadas, sem necessidade de banco de dados (os dados vêm de arquivos CSV ou ficam em memória RAM garantindo velocidade instantânea).
* **Frontend:** React + Vite, 100% Vanilla CSS responsivo e inspirado na temática oficial da Copa.
