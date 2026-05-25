#ifndef PARTIDA_HPP
#define PARTIDA_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Selecao.hpp"

using namespace std;

class Partida {
private:
    Selecao* timeCasa;
    Selecao* timeFora;
    int golsCasa;
    int golsFora;
    bool partidaJogada;

public:
    Partida(Selecao* casa, Selecao* fora) {
        timeCasa = casa;
        timeFora = fora;
        golsCasa = 0;
        golsFora = 0;
        partidaJogada = false;
    }

    void jogarFaseGrupos(int golsC, int golsF) {
        golsCasa = golsC;
        golsFora = golsF;
        partidaJogada = true;

        timeCasa->registrarGols(golsCasa, golsFora);
        timeFora->registrarGols(golsFora, golsCasa);

        if (golsCasa > golsFora) {
            timeCasa->adicionarVitoria();
            cout << "Resultado: Vitoria de " << timeCasa->getNome() << "!" << endl;
        } 
        else if (golsFora > golsCasa) {
            timeFora->adicionarVitoria();
            cout << "Resultado: Vitoria de " << timeFora->getNome() << "!" << endl;
        } 
        else {
            timeCasa->adicionarEmpate();
            timeFora->adicionarEmpate();
            cout << "Resultado: Empate!" << endl;
        }
    }

    void simularJogoAutomatizado() {
        int golsC = rand() % 4; 
        int golsF = rand() % 4; 

        cout << "Simulando: " << timeCasa->getNome() << " x " << timeFora->getNome() << "..." << endl;
        jogarFaseGrupos(golsC, golsF);
        exibirPlacar();
        cout << endl;
    }

    void exibirPlacar() {
        if (partidaJogada) {
            cout << "Placar Final: " << timeCasa->getNome() << " " << golsCasa << " x " 
                 << golsFora << " " << timeFora->getNome() << endl;
        } else {
            cout << "A partida entre " << timeCasa->getNome() << " e " 
                 << timeFora->getNome() << " ainda nao foi jogada." << endl;
        }
    }
};

#endif