#include "ConsoleUtils.hpp"

#include <iostream>

using namespace std;

int lerOpcaoIntervalo(int minimo, int maximo) {
    int opcao = 0;
    cin >> opcao;

    while (cin.fail() || opcao < minimo || opcao > maximo) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Opcao invalida. Digite um numero entre " << minimo << " e " << maximo << ": ";
        cin >> opcao;
    }

    cin.ignore(10000, '\n');
    return opcao;
}