#ifndef NOJOGADOR_HPP
#define NOJOGADOR_HPP

#include "Jogador.hpp"

using namespace std;

class NoJogador {
public:
    Jogador* atleta;
    NoJogador* proximo;

    // Cria um no da lista encadeada apontando para um jogador.
    NoJogador(Jogador* j) {
        atleta = j;
        proximo = nullptr;
    }
};

#endif