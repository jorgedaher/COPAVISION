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
    Jogador(string n, string p, int num) {
        nome = n;
        posicao = p;
        numeroCamisa = num;
        golsMarcados = 0;
    }

    string getNome() { return nome; }
    string getPosicao() { return posicao; }
    int getNumeroCamisa() { return numeroCamisa; }
    int getGols() { return golsMarcados; }

    void marcarGol() { golsMarcados++; }
};

#endif