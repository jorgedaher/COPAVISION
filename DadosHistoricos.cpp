#include "DadosHistoricos.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

string trimHistorico(const string& texto) {
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

vector<string> separarCamposCSVHistorico(const string& linha) {
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
            campos.push_back(trimHistorico(atual));
            atual.clear();
        } else {
            atual += c;
        }
    }

    campos.push_back(trimHistorico(atual));
    return campos;
}

bool tentarConverterParaInteiroHistorico(const string& texto, int& valor) {
    if (texto.empty()) {
        return false;
    }

    stringstream ss(texto);
    ss >> valor;
    return !ss.fail() && ss.eof();
}

}

bool carregarDadosHistoricosCSV(const vector<string>& caminhos, DadosHistoricosCopa& dados) {
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

        vector<string> campos = separarCamposCSVHistorico(linha);
        if (campos.size() < 4) {
            continue;
        }

        string tipo = trimHistorico(campos[0]);
        string nomeSelecao = trimHistorico(campos[1]);
        string texto = trimHistorico(campos[2]);
        string numeroTexto = trimHistorico(campos[3]);
        int numero = 0;

        if (tipo == "artilheiro") {
            if (!nomeSelecao.empty() && !texto.empty()) {
                dados.artilheiros[nomeSelecao] = texto;
            }
            continue;
        }

        if (tipo == "curiosidade") {
            if (!texto.empty()) {
                dados.curiosidades.push_back(texto);
            }
            continue;
        }

        if (!tentarConverterParaInteiroHistorico(numeroTexto, numero) || nomeSelecao.empty()) {
            continue;
        }

        if (tipo == "titulo") {
            dados.titulos[nomeSelecao] = numero;
        } else if (tipo == "participacao") {
            dados.participacoes[nomeSelecao] = numero;
        } else if (tipo == "campea") {
            dados.campeas.push_back({nomeSelecao, numero});
        }
    }

    arquivo.close();

    bool carregouAlgumDado = !dados.artilheiros.empty() || !dados.titulos.empty() ||
                             !dados.participacoes.empty() || !dados.campeas.empty() ||
                             !dados.curiosidades.empty();

    if (carregouAlgumDado) {
        cout << "[OK] Dados historicos carregados: " << caminhoUsado << "\n" << endl;
    }

    return carregouAlgumDado;
}

string obterMaiorArtilheiroHistorico(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos) {
    auto it = dadosHistoricos.artilheiros.find(nomeSelecao);
    if (it != dadosHistoricos.artilheiros.end()) {
        return it->second;
    }

    return "Dados historicos nao cadastrados para esta selecao";
}

int obterTitulosCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos) {
    auto it = dadosHistoricos.titulos.find(nomeSelecao);
    if (it != dadosHistoricos.titulos.end()) {
        return it->second;
    }

    return 0;
}

int obterParticipacoesCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos) {
    auto it = dadosHistoricos.participacoes.find(nomeSelecao);
    if (it != dadosHistoricos.participacoes.end()) {
        return it->second;
    }

    return -1;
}

void exibirDadosSelecoesCampeas(const DadosHistoricosCopa& dadosHistoricos) {
    cout << "\n=============================================" << endl;
    cout << "         DADOS DAS SELECOES CAMPEAS         " << endl;
    cout << "=============================================" << endl;

    if (dadosHistoricos.campeas.empty()) {
        cout << "Nenhum dado de selecoes campeas foi carregado." << endl;
    }

    for (const auto& item : dadosHistoricos.campeas) {
        cout << item.first << " - " << item.second << " titulo(s)" << endl;
    }

    cout << "=============================================\n" << endl;
}

void exibirCuriosidadesDaCopa(const DadosHistoricosCopa& dadosHistoricos) {
    cout << "\n=============================================" << endl;
    cout << "             CURIOSIDADES DA COPA           " << endl;
    cout << "=============================================" << endl;

    if (dadosHistoricos.curiosidades.empty()) {
        cout << "Nenhuma curiosidade foi carregada." << endl;
    }

    for (size_t i = 0; i < dadosHistoricos.curiosidades.size(); i++) {
        cout << i + 1 << ". " << dadosHistoricos.curiosidades[i] << endl;
    }

    cout << "=============================================\n" << endl;
}