#ifndef SELECOES_LOADER_HPP
#define SELECOES_LOADER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "Selecao.hpp"

using namespace std;

// Carrega selecoes reais a partir de caminhos candidatos de arquivo CSV.
bool carregarSelecoesDoCSV(const vector<string>& caminhos,
                          vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos);

// Carrega dados sinteticos quando o CSV real nao estiver disponivel.
void carregarSelecoesTeste(vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos);

#endif