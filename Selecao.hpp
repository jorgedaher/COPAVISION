#ifndef SELECAO_HPP
#define SELECAO_HPP

#include <iostream>
#include <string>
#include <algorithm> 
#include <cctype>    
#include <vector>
#include "NoJogador.hpp"

using namespace std;

class Selecao {
private:
    string nome;
    string continente;
    int titulosMundiais;
    int rankingFifa;
    
    int pontos;
    int golsPro;
    int golsContra;
    int vitorias;

    NoJogador* cabecaElenco;

public:
    Selecao(string n, string cont, int titulos, int ranking) {
        nome = n;
        continente = cont;
        titulosMundiais = titulos;
        rankingFifa = ranking;
        pontos = 0;
        golsPro = 0;
        golsContra = 0;
        vitorias = 0;
        cabecaElenco = nullptr;
    }

    void convocarJogador(Jogador* novoJogador) {
        NoJogador* novoNo = new NoJogador(novoJogador);
        if (cabecaElenco == nullptr) {
            cabecaElenco = novoNo;
        } else {
            NoJogador* atual = cabecaElenco;
            while (atual->proximo != nullptr) {
                atual = atual->proximo;
            }
            atual->proximo = novoNo;
        }
    }

    void exibirElenco() {
        cout << "--- Elenco da Selecao: " << nome << " ---" << endl;
        NoJogador* atual = cabecaElenco;
        if (atual == nullptr) {
            cout << "Nenhum jogador convocado ainda." << endl;
            return;
        }
        while (atual != nullptr) {
            cout << "Camisa " << atual->atleta->getNumeroCamisa() << " | " 
                 << atual->atleta->getNome() << " (" << atual->atleta->getPosicao() << ")" << endl;
            atual = atual->proximo;
        }
        cout << "-----------------------------------" << endl;
    }

    // Busca inteligente de jogadores na Lista Encadeada
    Jogador* buscarJogadorPorNome(string termoBusca) {
        string termoLower = termoBusca;
        transform(termoLower.begin(), termoLower.end(), termoLower.begin(), ::tolower);

        NoJogador* atual = cabecaElenco;
        
        while (atual != nullptr) {
            string nomeJogador = atual->atleta->getNome();
            string nomeLower = nomeJogador;
            
            transform(nomeLower.begin(), nomeLower.end(), nomeLower.begin(), ::tolower);
            
            if (nomeLower.find(termoLower) != string::npos) {
                return atual->atleta;
            }
            atual = atual->proximo;
        }
        return nullptr;
    }

    // Busca por nome completo (case-insensitive)
    Jogador* buscarJogadorNomeExato(string nomeBusca) {
        string nomeBuscaLower = nomeBusca;
        transform(nomeBuscaLower.begin(), nomeBuscaLower.end(), nomeBuscaLower.begin(), 
                  [](unsigned char c) { return tolower(c); });

        NoJogador* atual = cabecaElenco;
        while (atual != nullptr) {
            string nomeLower = atual->atleta->getNome();
            transform(nomeLower.begin(), nomeLower.end(), nomeLower.begin(),
                      [](unsigned char c) { return tolower(c); });

            if (nomeLower == nomeBuscaLower) {
                return atual->atleta;
            }
            atual = atual->proximo;
        }
        return nullptr;
    }

    // Lista jogadores cujo nome comeca com o termo digitado (case-insensitive)
    vector<Jogador*> buscarJogadoresPorPrefixo(string termoBusca) {
        vector<Jogador*> encontrados;
        string termoLower = termoBusca;
        transform(termoLower.begin(), termoLower.end(), termoLower.begin(),
                  [](unsigned char c) { return tolower(c); });

        NoJogador* atual = cabecaElenco;
        while (atual != nullptr) {
            string nomeLower = atual->atleta->getNome();
            transform(nomeLower.begin(), nomeLower.end(), nomeLower.begin(),
                      [](unsigned char c) { return tolower(c); });

            if (!termoLower.empty() && nomeLower.rfind(termoLower, 0) == 0) {
                encontrados.push_back(atual->atleta);
            }
            atual = atual->proximo;
        }

        return encontrados;
    }

    string getNome() { return nome; }
    int getPontos() { return pontos; }
    int getSaldoGols() { return golsPro - golsContra; }
    int getGolsPro() { return golsPro; }
    int getVitorias() { return vitorias; }
    int getRankingFifa() { return rankingFifa; }

    void adicionarVitoria() { pontos += 3; vitorias++; }
    void adicionarEmpate() { pontos += 1; }
    void registrarGols(int marcados, int sofridos) {
        golsPro += marcados;
        golsContra += sofridos;
    }
};

#endif