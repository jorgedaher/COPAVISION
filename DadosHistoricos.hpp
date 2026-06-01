#ifndef DADOS_HISTORICOS_HPP
#define DADOS_HISTORICOS_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

struct DadosHistoricosCopa {
    unordered_map<string, string> artilheiros;
    unordered_map<string, int> titulos;
    unordered_map<string, int> participacoes;
    vector<pair<string, int>> campeas;
    vector<string> curiosidades;
};

bool carregarDadosHistoricosCSV(const vector<string>& caminhos, DadosHistoricosCopa& dados);
string obterMaiorArtilheiroHistorico(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
int obterTitulosCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
int obterParticipacoesCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
void exibirDadosSelecoesCampeas(const DadosHistoricosCopa& dadosHistoricos);
void exibirCuriosidadesDaCopa(const DadosHistoricosCopa& dadosHistoricos);

#endif