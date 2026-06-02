#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include "Selecao.hpp"
#include "ConsoleUtils.hpp"
#include "DadosHistoricos.hpp"
#include "Menus.hpp"
#include "SelecoesLoader.hpp"
#include "SimulacaoCopa.hpp"

using namespace std;

// Inicializa os dados do sistema, carrega os arquivos CSV e controla o loop principal do menu.
int main() {
    srand(time(0));

    vector<Selecao*> catalogoSelecoes;
    DadosHistoricosCopa dadosHistoricos;
    unordered_map<string, vector<Selecao*>> gruposDaCopa;
    vector<string> ordemGrupos;
    vector<string> caminhosCSV = {
        "./convocados_copa_2026_atualizado.csv",
        "/Users/jorgedaher23/Downloads/convocados_copa_2026_atualizado.csv"
    };
    vector<string> caminhosDadosHistoricos = {
        "./dados_historicos_copa.csv"
    };

    bool carregouCSV = carregarSelecoesDoCSV(caminhosCSV, catalogoSelecoes, gruposDaCopa, ordemGrupos);
    bool carregouDadosHistoricos = carregarDadosHistoricosCSV(caminhosDadosHistoricos, dadosHistoricos);

    if (!carregouDadosHistoricos) {
        cout << "[AVISO] CSV de dados historicos nao encontrado ou vazio.\n" << endl;
    }

    if (!carregouCSV) {
        cout << "[AVISO] CSV nao encontrado ou vazio. Carregando dados de teste.\n" << endl;
        carregarSelecoesTeste(catalogoSelecoes, gruposDaCopa, ordemGrupos);
    }

    bool executando = true;
    while (executando) {
        exibirMenuPrincipal();
        int opcao = lerOpcaoIntervalo(1, 5);

        switch (opcao) {
            case 1:
                executarSimulacaoCopa(catalogoSelecoes, gruposDaCopa, ordemGrupos);
                break;
            case 2:
                menuSelecoes(catalogoSelecoes, dadosHistoricos);
                break;
            case 3:
                exibirDadosSelecoesCampeas(dadosHistoricos);
                break;
            case 4:
                exibirCuriosidadesDaCopa(dadosHistoricos);
                break;
            case 5:
                executando = false;
                cout << "\nEncerrando o COPA VISION. Ate a proxima!\n" << endl;
                break;
        }
    }

    for (Selecao* selecao : catalogoSelecoes) {
        delete selecao;
    }

    return 0;
}