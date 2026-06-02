#include "SelecoesLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "Jogador.hpp"

namespace {

string trimCSV(const string& texto) {
    size_t inicio = 0;
    while (inicio < texto.size() && isspace(static_cast<unsigned char>(texto[inicio]))) {
        inicio++;
    }

    size_t fim = texto.size();
    while (fim > inicio && isspace(static_cast<unsigned char>(texto[fim - 1]))) {
        fim--;
    }

    return texto.substr(inicio, fim - inicio);
}

bool tentarExtrairNumeroCamisa(const string& observacao, int& numeroCamisa) {
    const string marcador = "camisa=";
    size_t posicao = observacao.find(marcador);
    if (posicao == string::npos) {
        return false;
    }

    posicao += marcador.size();
    if (posicao >= observacao.size() || !isdigit(static_cast<unsigned char>(observacao[posicao]))) {
        return false;
    }

    size_t fim = posicao;
    while (fim < observacao.size() && isdigit(static_cast<unsigned char>(observacao[fim]))) {
        fim++;
    }

    numeroCamisa = stoi(observacao.substr(posicao, fim - posicao));
    return numeroCamisa > 0;
}

vector<string> separarCamposCSV(const string& linha) {
    vector<string> campos;
    string atual;
    bool dentroDeAspas = false;

    for (size_t i = 0; i < linha.size(); i++) {
        char c = linha[i];

        if (c == '"') {
            if (dentroDeAspas && i + 1 < linha.size() && linha[i + 1] == '"') {
                atual += '"';
                i++;
            } else {
                dentroDeAspas = !dentroDeAspas;
            }
        } else if (c == ',' && !dentroDeAspas) {
            campos.push_back(trimCSV(atual));
            atual.clear();
        } else {
            atual += c;
        }
    }

    campos.push_back(trimCSV(atual));
    return campos;
}

}

bool carregarSelecoesDoCSV(const vector<string>& caminhos,
                          vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos) {
    ifstream arquivo;
    string caminhoUsado;

    for (const string& caminho : caminhos) {
        arquivo.open(caminho);
        if (arquivo.is_open()) {
            caminhoUsado = caminho;
            break;
        }
        arquivo.clear();
    }

    if (!arquivo.is_open()) {
        return false;
    }

    unordered_map<string, Selecao*> mapaSelecoes;
    unordered_map<string, int> proximoNumeroCamisa;
    unordered_set<string> gruposRegistrados;

    string linha;
    bool cabecalhoPulou = false;

    while (getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        if (!cabecalhoPulou) {
            cabecalhoPulou = true;
            continue;
        }

        vector<string> campos = separarCamposCSV(linha);
        if (campos.size() < 7) {
            continue;
        }

        string grupo = trimCSV(campos[0]);
        string nomeSelecao = trimCSV(campos[1]);
        string posicao = trimCSV(campos[5]);
        string nomeJogador = trimCSV(campos[6]);

        if (nomeSelecao.empty() || nomeJogador.empty() || posicao.empty()) {
            continue;
        }

        if (mapaSelecoes.find(nomeSelecao) == mapaSelecoes.end()) {
            string continente = grupo.empty() ? "Desconhecido" : "Grupo " + grupo;
            Selecao* novaSelecao = new Selecao(nomeSelecao, continente, 0);
            mapaSelecoes[nomeSelecao] = novaSelecao;
            proximoNumeroCamisa[nomeSelecao] = 1;
            catalogoSelecoes.push_back(novaSelecao);

            if (!grupo.empty()) {
                gruposDaCopa[grupo].push_back(novaSelecao);
                if (gruposRegistrados.find(grupo) == gruposRegistrados.end()) {
                    ordemGrupos.push_back(grupo);
                    gruposRegistrados.insert(grupo);
                }
            }
        }

        int numeroCamisa = 0;
        bool numeroCamisaDefinido = campos.size() >= 11 &&
                                    tentarExtrairNumeroCamisa(trimCSV(campos[10]), numeroCamisa);

        if (!numeroCamisaDefinido) {
            numeroCamisa = proximoNumeroCamisa[nomeSelecao]++;
        } else if (proximoNumeroCamisa[nomeSelecao] <= numeroCamisa) {
            proximoNumeroCamisa[nomeSelecao] = numeroCamisa + 1;
        }

        mapaSelecoes[nomeSelecao]->convocarJogador(new Jogador(nomeJogador, posicao, numeroCamisa));
    }

    arquivo.close();

    if (catalogoSelecoes.empty()) {
        return false;
    }

    cout << "\n[OK] CSV carregado: " << caminhoUsado << endl;
    cout << "[OK] Selecoes carregadas: " << catalogoSelecoes.size() << "\n" << endl;
    return true;
}

void carregarSelecoesTeste(vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos) {
    Selecao* brasil = new Selecao("Brasil", "America do Sul", 5);
    Selecao* franca = new Selecao("Franca", "Europa", 2);
    Selecao* marrocos = new Selecao("Marrocos", "Africa", 0);
    Selecao* escocia = new Selecao("Escocia", "Europa", 0);

    brasil->convocarJogador(new Jogador("Vinicius Jr", "Atacante", 7));
    brasil->convocarJogador(new Jogador("Alisson", "Goleiro", 1));
    franca->convocarJogador(new Jogador("Kylian Mbappe", "Atacante", 10));

    catalogoSelecoes.push_back(brasil);
    catalogoSelecoes.push_back(franca);
    catalogoSelecoes.push_back(marrocos);
    catalogoSelecoes.push_back(escocia);

    gruposDaCopa["A"] = {brasil, franca, marrocos, escocia};
    ordemGrupos.push_back("A");
}