#ifndef NOARVORE_HPP
#define NOARVORE_HPP

#include <iostream>
#include "Selecao.hpp"

using namespace std;

class NoArvore {
public:
    string nomeFase;       
    Selecao* time1;        
    Selecao* time2;        
    Selecao* vencedor;     

    NoArvore* esquerda;
    NoArvore* direita;

    // Cria um no de confronto com os dois times ja definidos.
    NoArvore(string fase, Selecao* t1, Selecao* t2) {
        nomeFase = fase;
        time1 = t1;
        time2 = t2;
        vencedor = nullptr;
        esquerda = nullptr;
        direita = nullptr;
    }

    // Cria um no apenas com o nome da fase para preencher depois.
    NoArvore(string fase) {
        nomeFase = fase;
        time1 = nullptr;
        time2 = nullptr;
        vencedor = nullptr;
        esquerda = nullptr;
        direita = nullptr;
    }
};

#endif