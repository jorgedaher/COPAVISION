#ifndef ARVOREMATAMATA_HPP
#define ARVOREMATAMATA_HPP

#include <iostream>
#include <iomanip> 
#include "NoArvore.hpp"

using namespace std;

class ArvoreMataMata {
public:
    void simularMataMata(NoArvore* no) {
        if (no == nullptr) return;

        simularMataMata(no->esquerda);
        simularMataMata(no->direita);

        if (no->esquerda != nullptr && no->direita != nullptr) {
            no->time1 = no->esquerda->vencedor;
            no->time2 = no->direita->vencedor;
        }

        if (no->time1 != nullptr && no->time2 != nullptr) {
            cout << "\n=============================================" << endl;
            cout << "              " << no->nomeFase << "              " << endl;
            cout << "=============================================" << endl;
            
            int rank1 = no->time1->getRankingFifa();
            int rank2 = no->time2->getRankingFifa();
            int somaRankings = rank1 + rank2;
            
            double chance1 = ((double)rank2 / somaRankings) * 100.0;
            double chance2 = ((double)rank1 / somaRankings) * 100.0;

            cout << "Odds de Vitoria (Baseado na FIFA): " << endl;
            cout << fixed << setprecision(1); 
            cout << no->time1->getNome() << " (" << chance1 << "%) x (" 
                 << chance2 << "%) " << no->time2->getNome() << endl;
            cout << "---------------------------------------------" << endl;

            int gols1, gols2;
            cout << "Digite os gols de " << no->time1->getNome() << ": ";
            cin >> gols1;
            cout << "Digite os gols de " << no->time2->getNome() << ": ";
            cin >> gols2;

            if (gols1 > gols2) {
                no->vencedor = no->time1;
                cout << "-> " << no->vencedor->getNome() << " venceu no tempo normal!" << endl;
            } 
            else if (gols2 > gols1) {
                no->vencedor = no->time2;
                cout << "-> " << no->vencedor->getNome() << " venceu no tempo normal!" << endl;
            } 
            else {
                int escolhaPenaltis;
                cout << "\n>>> EMPATE! DECISAO NOS PENALTIS <<<" << endl;
                cout << "Quem venceu a disputa de penaltis?" << endl;
                cout << "[1] " << no->time1->getNome() << endl;
                cout << "[2] " << no->time2->getNome() << endl;
                cout << "Sua escolha: ";
                cin >> escolhaPenaltis;

                while (escolhaPenaltis != 1 && escolhaPenaltis != 2) {
                    cout << "Opcao invalida. Digite 1 ou 2: ";
                    cin >> escolhaPenaltis;
                }

                if (escolhaPenaltis == 1) {
                    no->vencedor = no->time1;
                } else {
                    no->vencedor = no->time2;
                }
                cout << "-> " << no->vencedor->getNome() << " avanca nos penaltis!" << endl;
            }
        }
    }
};

#endif