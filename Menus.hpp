#ifndef MENUS_HPP
#define MENUS_HPP

#include <vector>

#include "DadosHistoricos.hpp"
#include "Jogador.hpp"
#include "Selecao.hpp"

using namespace std;

void exibirMenuPrincipal();
void menuSelecoes(vector<Selecao*>& catalogoSelecoes, const DadosHistoricosCopa& dadosHistoricos);
Jogador* barraDePesquisa(Selecao* todasSelecoes[], int totalSelecoes);
Jogador* escolherMelhorGoleiro(Selecao* todasSelecoes[], int totalSelecoes);

#endif