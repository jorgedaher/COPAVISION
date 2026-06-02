#include "Menus.hpp"

#include <iostream>
#include <string>

#include "ConsoleUtils.hpp"

using namespace std;

namespace {

// Exibe as selecoes disponiveis e retorna o indice escolhido pelo usuario.
int escolherSelecao(const vector<Selecao*>& catalogoSelecoes) {
    cout << "\nSelecoes disponiveis:" << endl;
    for (size_t i = 0; i < catalogoSelecoes.size(); i++) {
        cout << "[" << i + 1 << "] " << catalogoSelecoes[i]->getNome() << endl;
    }
    cout << "Escolha uma selecao: ";
    return lerOpcaoIntervalo(1, static_cast<int>(catalogoSelecoes.size())) - 1;
}

}

// Mostra o menu principal da aplicacao.
void exibirMenuPrincipal() {
    cout << "\n=============================================" << endl;
    cout << "            MENU PRINCIPAL - COPA VISION    " << endl;
    cout << "=============================================" << endl;
    cout << "[1] Simulacao de Copa" << endl;
    cout << "[2] Selecoes" << endl;
    cout << "[3] Dados das selecoes campeas" << endl;
    cout << "[4] Curiosidades da Copa" << endl;
    cout << "[5] Sair" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Escolha uma opcao: ";
}

// Controla o menu de informacoes das selecoes.
void menuSelecoes(vector<Selecao*>& catalogoSelecoes, const DadosHistoricosCopa& dadosHistoricos) {
    if (catalogoSelecoes.empty()) {
        cout << "[ERRO] Nao ha selecoes carregadas." << endl;
        return;
    }

    while (true) {
        int indice = escolherSelecao(catalogoSelecoes);
        string nomeSelecao = catalogoSelecoes[indice]->getNome();

        bool menuDaSelecaoAtivo = true;
        while (menuDaSelecaoAtivo) {
            cout << "\n=============================================" << endl;
            cout << "        MENU DA SELECAO: " << nomeSelecao << endl;
            cout << "=============================================" << endl;
            cout << "[1] Jogadores" << endl;
            cout << "[2] Maior artilheiro" << endl;
            cout << "[3] Numero de titulos" << endl;
            cout << "[4] Participacoes em copas" << endl;
            cout << "[5] Trocar selecao" << endl;
            cout << "[6] Voltar ao menu principal" << endl;
            cout << "---------------------------------------------" << endl;
            cout << "Escolha uma opcao: ";

            int opcao = lerOpcaoIntervalo(1, 6);

            switch (opcao) {
                case 1:
                    cout << "\nOpcao escolhida: Jogadores" << endl;
                    catalogoSelecoes[indice]->exibirElenco();
                    break;
                case 2:
                    cout << "\nOpcao escolhida: Maior artilheiro" << endl;
                    cout << "Maior artilheiro historico de " << nomeSelecao << ": "
                         << obterMaiorArtilheiroHistorico(nomeSelecao, dadosHistoricos) << "\n" << endl;
                    break;
                case 3:
                    cout << "\nOpcao escolhida: Numero de titulos" << endl;
                    cout << "Titulos de Copa de " << nomeSelecao << ": "
                         << obterTitulosCopa(nomeSelecao, dadosHistoricos) << "\n" << endl;
                    break;
                case 4: {
                    cout << "\nOpcao escolhida: Participacoes em copas" << endl;
                    int participacoes = obterParticipacoesCopa(nomeSelecao, dadosHistoricos);
                    if (participacoes >= 0) {
                        cout << "Participacoes em Copas de " << nomeSelecao << ": "
                             << participacoes << "\n" << endl;
                    } else {
                        cout << "Dados de participacoes em Copas nao cadastrados para "
                             << nomeSelecao << ".\n" << endl;
                    }
                    break;
                }
                case 5:
                    menuDaSelecaoAtivo = false;
                    break;
                case 6:
                    return;
            }
        }
    }
}

// Busca um jogador por nome exato ou prefixo entre todas as selecoes.
Jogador* barraDePesquisa(Selecao* todasSelecoes[], int totalSelecoes) {
    struct OpcaoBusca {
        Jogador* jogador;
        Selecao* selecao;
    };

    string termo;

    while (true) {
        cout << "Digite o nome do jogador (ou parte do nome): ";
        getline(cin >> ws, termo);

        if (termo.empty()) {
            cout << "[ERRO] Digite pelo menos 1 caractere para pesquisar.\n" << endl;
            continue;
        }

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
            cout << "[ERRO] Nenhum jogador encontrado com esse inicio de nome ou sobrenome. Tente novamente.\n" << endl;
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

// Reutiliza a busca e valida que o jogador escolhido seja goleiro.
Jogador* escolherMelhorGoleiro(Selecao* todasSelecoes[], int totalSelecoes) {
    while (true) {
        Jogador* escolhido = barraDePesquisa(todasSelecoes, totalSelecoes);

        if (escolhido != nullptr && escolhido->getPosicao() == "Goleiro") {
            return escolhido;
        }

        cout << "[ERRO] O premio de Melhor Goleiro aceita apenas jogadores da posicao Goleiro. Tente novamente.\n" << endl;
    }
}