#ifndef SIMULACAO_COPA_HPP
#define SIMULACAO_COPA_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "Selecao.hpp"

using namespace std;

void executarSimulacaoCopa(vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos);

#endif