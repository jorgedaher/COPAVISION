#ifndef MENUS_HPP
#define MENUS_HPP

#include <vector>

#include "DadosHistoricos.hpp"
#include "Jogador.hpp"
#include "Selecao.hpp"

using namespace std;

// Exibe o menu principal com as opcoes do sistema.
void exibirMenuPrincipal();
// Exibe e controla o menu de consultas por selecao.
void menuSelecoes(vector<Selecao*>& catalogoSelecoes, const DadosHistoricosCopa& dadosHistoricos);
// Busca um jogador pelo nome entre todas as selecoes carregadas.
Jogador* barraDePesquisa(Selecao* todasSelecoes[], int totalSelecoes);
// Garante a selecao de um jogador da posicao goleiro.
Jogador* escolherMelhorGoleiro(Selecao* todasSelecoes[], int totalSelecoes);

#endif