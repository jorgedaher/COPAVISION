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
    
    int pontos;
    int golsPro;
    int golsContra;
    int vitorias;

    NoJogador* cabecaElenco;

public:
    // Inicializa a selecao com dados basicos e estatisticas zeradas.
    Selecao(string n, string cont, int titulos) {
        nome = n;
        continente = cont;
        titulosMundiais = titulos;
        pontos = 0;
        golsPro = 0;
        golsContra = 0;
        vitorias = 0;
        cabecaElenco = nullptr;
    }

    // Libera toda a lista encadeada de jogadores convocados.
    ~Selecao() {
        NoJogador* atual = cabecaElenco;
        while (atual != nullptr) {
            NoJogador* proximo = atual->proximo;
            delete atual->atleta;
            delete atual;
            atual = proximo;
        }
        cabecaElenco = nullptr;
    }

    // Adiciona um novo jogador ao fim da lista encadeada do elenco.
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

    // Exibe todos os jogadores convocados da selecao.
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

    // Busca um jogador por trecho do nome (case-insensitive).
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

    // Busca por nome completo (case-insensitive).
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

    // Lista jogadores cujo nome ou sobrenome inicia com o termo digitado.
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

            bool achouPrefixo = false;

            // Prefixo no nome completo (ex.: "vin" -> "vinicius jr")
            if (!termoLower.empty() && nomeLower.rfind(termoLower, 0) == 0) {
                achouPrefixo = true;
            }

            // Prefixo em qualquer palavra do nome (ex.: "mba" -> "kylian mbappe")
            if (!achouPrefixo && !termoLower.empty()) {
                size_t inicio = 0;
                while (inicio < nomeLower.size()) {
                    while (inicio < nomeLower.size() && nomeLower[inicio] == ' ') {
                        inicio++;
                    }
                    if (inicio >= nomeLower.size()) {
                        break;
                    }

                    size_t fim = nomeLower.find(' ', inicio);
                    string palavra;
                    if (fim == string::npos) {
                        palavra = nomeLower.substr(inicio);
                        inicio = nomeLower.size();
                    } else {
                        palavra = nomeLower.substr(inicio, fim - inicio);
                        inicio = fim + 1;
                    }

                    if (palavra.rfind(termoLower, 0) == 0) {
                        achouPrefixo = true;
                        break;
                    }
                }
            }

            if (achouPrefixo) {
                encontrados.push_back(atual->atleta);
            }
            atual = atual->proximo;
        }

        return encontrados;
    }

    // Retorna o nome da selecao.
    string getNome() { return nome; }
    // Retorna os pontos acumulados na competicao.
    int getPontos() { return pontos; }
    // Retorna o saldo de gols da selecao.
    int getSaldoGols() { return golsPro - golsContra; }
    // Retorna a quantidade de gols marcados.
    int getGolsPro() { return golsPro; }
    // Retorna a quantidade de vitorias.
    int getVitorias() { return vitorias; }
    // Soma 3 pontos e incrementa o numero de vitorias.
    void adicionarVitoria() { pontos += 3; vitorias++; }
    // Soma 1 ponto por empate.
    void adicionarEmpate() { pontos += 1; }
    // Atualiza gols marcados e sofridos da selecao.
    void registrarGols(int marcados, int sofridos) {
        golsPro += marcados;
        golsContra += sofridos;
    }
};

#endif