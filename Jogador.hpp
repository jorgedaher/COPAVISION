#ifndef JOGADOR_HPP
#define JOGADOR_HPP

#include <string>

using namespace std;

class Jogador {
private:
    string nome;
    string posicao;
    int numeroCamisa;
    int golsMarcados;

public:
    // Cria um jogador com dados basicos e contador de gols zerado.
    Jogador(string n, string p, int num) {
        nome = n;
        posicao = p;
        numeroCamisa = num;
        golsMarcados = 0;
    }

    // Retorna o nome do jogador.
    string getNome() { return nome; }
    // Retorna a posicao em campo.
    string getPosicao() { return posicao; }
    // Retorna o numero da camisa.
    int getNumeroCamisa() { return numeroCamisa; }
    // Retorna o total de gols marcados.
    int getGols() { return golsMarcados; }

    // Incrementa em uma unidade o contador de gols.
    void marcarGol() { golsMarcados++; }
};

#endif