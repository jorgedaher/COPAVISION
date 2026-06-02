#ifndef GRUPO_HPP
#define GRUPO_HPP

#include <iostream>
#include <string>
#include "Selecao.hpp"

using namespace std;

class Grupo {
private:
    string nomeGrupo;
    Selecao* times[4];
    int quantidadeAtual;

public:
    // Inicializa um grupo vazio com nome e espaco para 4 selecoes.
    Grupo(string nome) {
        nomeGrupo = nome;
        quantidadeAtual = 0;
        for (int i = 0; i < 4; i++) {
            times[i] = nullptr;
        }
    }

    // Adiciona uma selecao ao grupo na proxima posicao disponivel.
    void adicionarSelecao(Selecao* s) {
        if (quantidadeAtual < 4) {
            times[quantidadeAtual] = s;
            quantidadeAtual++;
        }
    }

    // Permite definir manualmente a classificacao final do grupo.
    void definirPosicoesManuais() {
        cout << "\n=============================================" << endl;
        cout << "      DEFINA AS POSICOES DO " << nomeGrupo << "      " << endl;
        cout << "=============================================" << endl;
        
        for (int i = 0; i < 4; i++) {
            cout << "[" << i + 1 << "] " << times[i]->getNome() << endl;
        }
        cout << "---------------------------------------------" << endl;

        Selecao* novoOrdenado[4];
        bool escolhido[4] = {false, false, false, false};

        for (int pos = 0; pos < 4; pos++) {
            int opcao;
            cout << "Escolha quem ficou em " << pos + 1 << "o lugar (digite o numero de 1 a 4): ";
            cin >> opcao;

            while (opcao < 1 || opcao > 4 || escolhido[opcao - 1]) {
                cout << "Opcao invalida ou equipa ja escolhida. Digite novamente: ";
                cin >> opcao;
            }

            novoOrdenado[pos] = times[opcao - 1];
            escolhido[opcao - 1] = true; 
        }

        for (int i = 0; i < 4; i++) {
            times[i] = novoOrdenado[i];
        }

        cout << "\n-> " << nomeGrupo << " definido com sucesso!" << endl;
    }

    // Retorna o identificador textual do grupo.
    string getNomeGrupo() { return nomeGrupo; }
    // Retorna a selecao classificada em primeiro lugar.
    Selecao* getPrimeiro() { return times[0]; }
    // Retorna a selecao classificada em segundo lugar.
    Selecao* getSegundo() { return times[1]; }
    // Retorna a selecao classificada em terceiro lugar.
    Selecao* getTerceiro() { return times[2]; }

    // Exibe a tabela final de classificacao do grupo.
    void exibirTabela() {
        cout << "\n=== CLASSIFICACAO FINAL: " << nomeGrupo << " ===" << endl;
        cout << "Pos | Selecao" << endl;
        for (int i = 0; i < 4; i++) {
            if (times[i] != nullptr) {
                cout << i + 1 << "o  | " << times[i]->getNome() << endl;
            }
        }
        cout << "===================================\n" << endl;
    }
};

#endif