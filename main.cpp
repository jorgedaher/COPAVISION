#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Jogador.hpp"
#include "Selecao.hpp"
#include "Grupo.hpp"
#include "NoArvore.hpp"
#include "ArvoreMataMata.hpp"

using namespace std;

// Funcao global de barra de pesquisa com tratamento de erros e espacos
Jogador* barraDePesquisa(Selecao* todasSelecoes[], int totalSelecoes) {
    struct OpcaoBusca {
        Jogador* jogador;
        Selecao* selecao;
    };

    string termo;

    while (true) {
        cout << "Digite o nome do jogador (ou parte do nome): ";
        // getline com ws limpa qualquer buffer anterior e aceita espacos ("Vini Jr")
        getline(cin >> ws, termo);

        if (termo.empty()) {
            cout << "[ERRO] Digite pelo menos 1 caractere para pesquisar.\n" << endl;
            continue;
        }

        // Se o usuario digitar o nome completo, retorna imediatamente.
        for (int i = 0; i < totalSelecoes; i++) {
            Jogador* exato = todasSelecoes[i]->buscarJogadorNomeExato(termo);
            if (exato != nullptr) {
                cout << "-> Encontrado com sucesso: " << exato->getNome()
                     << " (" << todasSelecoes[i]->getNome() << ")\n" << endl;
                return exato;
            }
        }

        vector<OpcaoBusca> opcoes;

        for (int i = 0; i < totalSelecoes; i++) {
            vector<Jogador*> encontrados = todasSelecoes[i]->buscarJogadoresPorPrefixo(termo);
            for (Jogador* jogador : encontrados) {
                opcoes.push_back({jogador, todasSelecoes[i]});
            }
        }

        if (opcoes.empty()) {
            cout << "[ERRO] Nenhum jogador encontrado com esse inicio de nome. Tente novamente.\n" << endl;
            continue;
        }

        if (opcoes.size() == 1) {
            cout << "-> Encontrado com sucesso: " << opcoes[0].jogador->getNome()
                 << " (" << opcoes[0].selecao->getNome() << ")\n" << endl;
            return opcoes[0].jogador;
        }

        cout << "\nOpcoes encontradas:\n";
        for (size_t i = 0; i < opcoes.size(); i++) {
            cout << (i + 1) << ". " << opcoes[i].jogador->getNome()
                 << " (" << opcoes[i].selecao->getNome() << ")\n";
        }

        cout << "Escolha o numero do jogador correto: ";
        int escolha = 0;
        cin >> escolha;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERRO] Entrada invalida! Digite apenas numeros.\n" << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        if (escolha < 1 || escolha > static_cast<int>(opcoes.size())) {
            cout << "[ERRO] Numero fora das opcoes! Tente novamente.\n" << endl;
            continue;
        }

        cout << "-> Selecionado: " << opcoes[escolha - 1].jogador->getNome()
             << " (" << opcoes[escolha - 1].selecao->getNome() << ")\n" << endl;
        return opcoes[escolha - 1].jogador;
    }
}

int main() {
    srand(time(0)); 

    // 1. Criando selecoes teste
    Selecao* brasil = new Selecao("Brasil", "America do Sul", 5, 5);
    Selecao* franca = new Selecao("Franca", "Europa", 2, 2);
    Selecao* marrocos = new Selecao("Marrocos", "Africa", 0, 13);
    Selecao* escocia = new Selecao("Escocia", "Europa", 0, 39);

    // Convocando jogadores para testar a busca
    brasil->convocarJogador(new Jogador("Vinicius Jr", "Atacante", 7));
    brasil->convocarJogador(new Jogador("Alisson", "Goleiro", 1));
    franca->convocarJogador(new Jogador("Kylian Mbappe", "Atacante", 10));

    // Array master para a busca no final
    Selecao* catalogoSelecoes[4] = {brasil, franca, marrocos, escocia};

    cout << "=============================================" << endl;
    cout << "        BEM-VINDO A COPA VISION 2026         " << endl;
    cout << "=============================================" << endl;
    
    // 2. Grupos Teste
    Grupo grupoA("Grupo A");
    grupoA.adicionarSelecao(brasil);
    grupoA.adicionarSelecao(marrocos);
    grupoA.adicionarSelecao(escocia);
    grupoA.adicionarSelecao(franca); // Usando Franca aqui só para fechar 4 times e testar

    grupoA.definirPosicoesManuais();
    grupoA.exibirTabela();

    // 3. Mata-Mata Teste
    Selecao* primeiroA = grupoA.getPrimeiro();
    Selecao* segundoA = grupoA.getSegundo();

    NoArvore* grandeFinal = new NoArvore("Grande Final", primeiroA, segundoA);
    
    cout << "\n--- INICIANDO A GRANDE FINAL ---" << endl;
    ArvoreMataMata gerenciador;
    gerenciador.simularMataMata(grandeFinal);

    if (grandeFinal->vencedor != nullptr) {
        cout << "\n=================================================" << endl;
        cout << " CAMPEAO DA COPA VISION: " << grandeFinal->vencedor->getNome() << "!!!" << endl;
        cout << "=================================================" << endl;
    }

    // 4. Premiacoes
    cout << "\n=============================================" << endl;
    cout << "             PREMIACOES DA COPA              " << endl;
    cout << "=============================================" << endl;

    cout << "\n--- JOGADORES DISPONIVEIS POR EQUIPE ---" << endl;
    for (int i = 0; i < 4; i++) {
        catalogoSelecoes[i]->exibirElenco();
    }

    cout << "\n--- ESCOLHA O MELHOR JOGADOR DA COPA ---\n";
    Jogador* melhorJogador = barraDePesquisa(catalogoSelecoes, 4);

    cout << "\n--- ESCOLHA A REVELACAO DA COPA ---\n";
    Jogador* revelacao = barraDePesquisa(catalogoSelecoes, 4);

    cout << "\n--- ESCOLHA O MELHOR GOLEIRO ---\n";
    Jogador* melhorGoleiro = barraDePesquisa(catalogoSelecoes, 4);

    cout << "\n=============================================" << endl;
    cout << "              QUADRO DE HONRA                " << endl;
    cout << "=============================================" << endl;
    if (melhorJogador) cout << "Melhor Jogador: " << melhorJogador->getNome() << endl;
    if (revelacao) cout << "Revelacao: " << revelacao->getNome() << endl;
    if (melhorGoleiro) cout << "Melhor Goleiro: " << melhorGoleiro->getNome() << endl;
    cout << "=============================================" << endl;

    // Limpeza (Nao esquecer na versao final)
    delete grandeFinal;
    delete brasil; delete franca; delete marrocos; delete escocia;

    return 0;
}