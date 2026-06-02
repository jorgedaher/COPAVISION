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

// Carrega os dados historicos da Copa a partir de um arquivo CSV.
bool carregarDadosHistoricosCSV(const vector<string>& caminhos, DadosHistoricosCopa& dados);
// Consulta o maior artilheiro historico de uma selecao.
string obterMaiorArtilheiroHistorico(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
// Consulta o numero de titulos da selecao em Copas.
int obterTitulosCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
// Consulta o numero de participacoes da selecao em Copas.
int obterParticipacoesCopa(const string& nomeSelecao, const DadosHistoricosCopa& dadosHistoricos);
// Mostra o quadro de selecoes campeas.
void exibirDadosSelecoesCampeas(const DadosHistoricosCopa& dadosHistoricos);
// Mostra as curiosidades historicas cadastradas.
void exibirCuriosidadesDaCopa(const DadosHistoricosCopa& dadosHistoricos);

#endif