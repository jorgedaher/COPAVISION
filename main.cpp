#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include "Jogador.hpp"
#include "Selecao.hpp"
#include "Grupo.hpp"
#include "NoArvore.hpp"
#include "ArvoreMataMata.hpp"

using namespace std;

struct ClassificacaoGrupo {
    Selecao* primeiro = nullptr;
    Selecao* segundo = nullptr;
    Selecao* terceiro = nullptr;
    Selecao* quarto = nullptr;
};

string trim(const string& texto) {
    size_t inicio = 0;
    while (inicio < texto.size() && isspace(static_cast<unsigned char>(texto[inicio]))) {
        inicio++;
    }

    size_t fim = texto.size();
    while (fim > inicio && isspace(static_cast<unsigned char>(texto[fim - 1]))) {
        fim--;
    }

    return texto.substr(inicio, fim - inicio);
}

vector<string> separarCamposCSV(const string& linha) {
    vector<string> campos;
    string atual;
    bool dentroDeAspas = false;

    for (size_t i = 0; i < linha.size(); i++) {
        char c = linha[i];

        if (c == '"') {
            if (dentroDeAspas && i + 1 < linha.size() && linha[i + 1] == '"') {
                atual += '"';
                i++;
            } else {
                dentroDeAspas = !dentroDeAspas;
            }
        } else if (c == ',' && !dentroDeAspas) {
            campos.push_back(trim(atual));
            atual.clear();
        } else {
            atual += c;
        }
    }

    campos.push_back(trim(atual));
    return campos;
}

bool carregarSelecoesDoCSV(const vector<string>& caminhos,
                          vector<Selecao*>& catalogoSelecoes,
                          unordered_map<string, vector<Selecao*>>& gruposDaCopa,
                          vector<string>& ordemGrupos) {
    ifstream arquivo;
    string caminhoUsado;

    for (const string& caminho : caminhos) {
        arquivo.open(caminho);
        if (arquivo.is_open()) {
            caminhoUsado = caminho;
            break;
        }
        arquivo.clear();
    }

    if (!arquivo.is_open()) {
        return false;
    }

    unordered_map<string, Selecao*> mapaSelecoes;
    unordered_map<string, int> proximoNumeroCamisa;
    unordered_set<string> gruposRegistrados;

    string linha;
    bool cabecalhoPulou = false;

    while (getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        if (!cabecalhoPulou) {
            cabecalhoPulou = true;
            continue;
        }

        vector<string> campos = separarCamposCSV(linha);
        if (campos.size() < 7) {
            continue;
        }

        string grupo = trim(campos[0]);
        string nomeSelecao = trim(campos[1]);
        string posicao = trim(campos[5]);
        string nomeJogador = trim(campos[6]);

        if (nomeSelecao.empty() || nomeJogador.empty() || posicao.empty()) {
            continue;
        }

        if (mapaSelecoes.find(nomeSelecao) == mapaSelecoes.end()) {
            string continente = grupo.empty() ? "Desconhecido" : "Grupo " + grupo;
            Selecao* novaSelecao = new Selecao(nomeSelecao, continente, 0);
            mapaSelecoes[nomeSelecao] = novaSelecao;
            proximoNumeroCamisa[nomeSelecao] = 1;
            catalogoSelecoes.push_back(novaSelecao);

            if (!grupo.empty()) {
                gruposDaCopa[grupo].push_back(novaSelecao);
                if (gruposRegistrados.find(grupo) == gruposRegistrados.end()) {
                    ordemGrupos.push_back(grupo);
                    gruposRegistrados.insert(grupo);
                }
            }
        }

        int numeroCamisa = proximoNumeroCamisa[nomeSelecao]++;
        mapaSelecoes[nomeSelecao]->convocarJogador(new Jogador(nomeJogador, posicao, numeroCamisa));
    }

    arquivo.close();

    if (catalogoSelecoes.empty()) {
        return false;
    }

    cout << "\n[OK] CSV carregado: " << caminhoUsado << endl;
    cout << "[OK] Selecoes carregadas: " << catalogoSelecoes.size() << "\n" << endl;
    return true;
}

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

bool existeDistribuicaoValida(const vector<vector<string>>& confrontosRestantes,
                             size_t indice,
                             unordered_set<string> gruposDisponiveis) {
    if (indice >= confrontosRestantes.size()) {
        return true;
    }

    for (const string& grupo : confrontosRestantes[indice]) {
        if (gruposDisponiveis.find(grupo) == gruposDisponiveis.end()) {
            continue;
        }

        unordered_set<string> proximosDisponiveis = gruposDisponiveis;
        proximosDisponiveis.erase(grupo);

        if (existeDistribuicaoValida(confrontosRestantes, indice + 1, proximosDisponiveis)) {
            return true;
        }
    }

    return false;
}

unordered_map<string, Selecao*> selecionarMelhoresTerceiros(
    const unordered_map<string, ClassificacaoGrupo>& classificacaoPorGrupo,
    const vector<string>& ordemGrupos,
    const vector<vector<string>>& confrontosComTerceiros) {

    vector<pair<string, Selecao*>> terceiros;
    for (const string& idGrupo : ordemGrupos) {
        auto it = classificacaoPorGrupo.find(idGrupo);
        if (it != classificacaoPorGrupo.end() && it->second.terceiro != nullptr) {
            terceiros.push_back({idGrupo, it->second.terceiro});
        }
    }

    while (true) {
        unordered_map<string, Selecao*> melhoresTerceiros;

        cout << "\n=============================================\n";
        cout << "        ESCOLHA OS 8 MELHORES TERCEIROS      \n";
        cout << "=============================================\n";

        for (size_t i = 0; i < terceiros.size(); i++) {
            cout << "[" << i + 1 << "] Grupo " << terceiros[i].first
                 << " - " << terceiros[i].second->getNome() << endl;
        }
        cout << "---------------------------------------------" << endl;

        vector<bool> escolhido(terceiros.size(), false);
        while (melhoresTerceiros.size() < 8 && melhoresTerceiros.size() < terceiros.size()) {
            cout << "Escolha o " << melhoresTerceiros.size() + 1 << "o melhor terceiro: ";
            int opcao = lerOpcaoIntervalo(1, static_cast<int>(terceiros.size()));

            while (escolhido[opcao - 1]) {
                cout << "Esse terceiro ja foi escolhido. Digite outro numero: ";
                opcao = lerOpcaoIntervalo(1, static_cast<int>(terceiros.size()));
            }

            escolhido[opcao - 1] = true;
            melhoresTerceiros[terceiros[opcao - 1].first] = terceiros[opcao - 1].second;
        }

        unordered_set<string> gruposDisponiveis;
        for (const auto& item : melhoresTerceiros) {
            gruposDisponiveis.insert(item.first);
        }

        if (existeDistribuicaoValida(confrontosComTerceiros, 0, gruposDisponiveis)) {
            return melhoresTerceiros;
        }

        cout << "\n[ERRO] Essa combinacao de terceiros nao permite preencher todos os confrontos oficiais dos 16-avos." << endl;
        cout << "Escolha outra combinacao de 8 terceiros.\n" << endl;
    }
}

Selecao* escolherTerceiroElegivel(const string& nomeJogo,
                                  Selecao* liderDoGrupo,
                                  const vector<string>& gruposElegiveis,
                                  unordered_map<string, Selecao*>& terceirosDisponiveis,
                                  const vector<vector<string>>& confrontosRestantes) {
    vector<pair<string, Selecao*>> opcoes;

    for (const string& grupo : gruposElegiveis) {
        auto it = terceirosDisponiveis.find(grupo);
        if (it != terceirosDisponiveis.end()) {
            opcoes.push_back(*it);
        }
    }

    vector<pair<string, Selecao*>> opcoesValidas;
    for (const auto& opcao : opcoes) {
        unordered_set<string> gruposDisponiveis;
        for (const auto& item : terceirosDisponiveis) {
            if (item.first != opcao.first) {
                gruposDisponiveis.insert(item.first);
            }
        }

        if (existeDistribuicaoValida(confrontosRestantes, 0, gruposDisponiveis)) {
            opcoesValidas.push_back(opcao);
        }
    }

    if (opcoesValidas.empty()) {
        cout << "[ERRO] Nao ha terceiro colocado disponivel para " << nomeJogo << "." << endl;
        return nullptr;
    }

    cout << "\n---------------------------------------------" << endl;
    cout << nomeJogo << ": " << liderDoGrupo->getNome() << " enfrenta um dos terceiros elegiveis." << endl;
    for (size_t i = 0; i < opcoesValidas.size(); i++) {
        cout << "[" << i + 1 << "] Grupo " << opcoesValidas[i].first << " - " << opcoesValidas[i].second->getNome() << endl;
    }
    cout << "Escolha o terceiro classificado deste confronto: ";

    int opcao = lerOpcaoIntervalo(1, static_cast<int>(opcoesValidas.size()));
    Selecao* escolhido = opcoesValidas[opcao - 1].second;
    terceirosDisponiveis.erase(opcoesValidas[opcao - 1].first);
    return escolhido;
}

NoArvore* criarFolha(const string& fase, Selecao* time1, Selecao* time2) {
    return new NoArvore(fase, time1, time2);
}

void liberarArvore(NoArvore* no) {
    if (no == nullptr) {
        return;
    }

    liberarArvore(no->esquerda);
    liberarArvore(no->direita);
    delete no;
}

Selecao* simularConfrontoMataMata(const string& nomeFase, const string& nomeJogo, Selecao* time1, Selecao* time2) {
    cout << "\n=============================================" << endl;
    cout << "              " << nomeFase << "              " << endl;
    cout << "=============================================" << endl;
    
    // 2. Simulacao de todos os grupos

    cout << nomeJogo << endl;
    cout << time1->getNome() << " x " << time2->getNome() << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Escolha o resultado do jogo:" << endl;
    cout << "[1] Vitoria de " << time1->getNome() << endl;
    cout << "[2] Vitoria de " << time2->getNome() << endl;
    cout << "[3] Empate" << endl;
    cout << "Sua escolha: ";

    int resultado = lerOpcaoIntervalo(1, 3);

    if (resultado == 1) {
        cout << "-> " << time1->getNome() << " CLASSIFICADO!" << endl;
        return time1;
    }

    if (resultado == 2) {
        cout << "-> " << time2->getNome() << " CLASSIFICADO!" << endl;
        return time2;
    }

    cout << "\n>>> EMPATE! DECISAO NOS PENALTIS <<<" << endl;
    cout << "Quem venceu a disputa de penaltis?" << endl;
    cout << "[1] " << time1->getNome() << endl;
    cout << "[2] " << time2->getNome() << endl;
    cout << "Sua escolha: ";

    int penaltis = lerOpcaoIntervalo(1, 2);
    Selecao* vencedor = (penaltis == 1) ? time1 : time2;
    cout << "-> " << vencedor->getNome() << " CLASSIFICADO!" << endl;
    return vencedor;
}

vector<Selecao*> simularRodadaMataMata(const string& nomeFase, const vector<pair<Selecao*, Selecao*>>& jogos) {
    vector<Selecao*> classificados;
    classificados.reserve(jogos.size());

    for (size_t i = 0; i < jogos.size(); i++) {
        string nomeJogo = "Jogo " + to_string(73 + static_cast<int>(i));
        Selecao* vencedor = simularConfrontoMataMata(nomeFase, nomeJogo, jogos[i].first, jogos[i].second);
        classificados.push_back(vencedor);
    }

    return classificados;
}

vector<Selecao*> simularRodadaMataMata(const string& nomeFase,
                                       int numeroInicialJogo,
                                       const vector<pair<Selecao*, Selecao*>>& jogos) {
    vector<Selecao*> classificados;
    classificados.reserve(jogos.size());

    for (size_t i = 0; i < jogos.size(); i++) {
        string nomeJogo = "Jogo " + to_string(numeroInicialJogo + static_cast<int>(i));
        Selecao* vencedor = simularConfrontoMataMata(nomeFase, nomeJogo, jogos[i].first, jogos[i].second);
        classificados.push_back(vencedor);
    }

    return classificados;
}

bool atribuirTerceirosAosJogos(const vector<vector<string>>& slots,
                               size_t indice,
                               unordered_map<string, Selecao*> terceirosDisponiveis,
                               vector<pair<string, Selecao*>>& atribuicoes) {
    if (indice >= slots.size()) {
        return true;
    }

    for (const string& grupo : slots[indice]) {
        auto it = terceirosDisponiveis.find(grupo);
        if (it == terceirosDisponiveis.end()) {
            continue;
        }

        Selecao* terceiroEscolhido = it->second;
        terceirosDisponiveis.erase(it);
        atribuicoes.push_back({grupo, terceiroEscolhido});

        if (atribuirTerceirosAosJogos(slots, indice + 1, terceirosDisponiveis, atribuicoes)) {
            return true;
        }

        atribuicoes.pop_back();
        terceirosDisponiveis[grupo] = terceiroEscolhido;
    }

    return false;
}

// Funcao global de barra de pesquisa com tratamento de erros e espacos
Jogador* barraDePesquisa(Selecao* todasSelecoes[], int totalSelecoes) {
    struct OpcaoBusca {
        Jogador* jogador;
        Selecao* selecao;
    };

    string termo;

    while (true) {
        cout << "Digite o nome do jogador (ou parte do nome): ";
        // getline com ws limpa qualquer buffer anterior e aceita espacos ("Vini Jr")
        getline(cin >> ws, termo);

        if (termo.empty()) {
            cout << "[ERRO] Digite pelo menos 1 caractere para pesquisar.\n" << endl;
            continue;
        }

        // Se o usuario digitar o nome completo, retorna imediatamente.
        for (int i = 0; i < totalSelecoes; i++) {
            Jogador* exato = todasSelecoes[i]->buscarJogadorNomeExato(termo);
            if (exato != nullptr) {
                cout << "-> Encontrado com sucesso: " << exato->getNome()
                     << " (" << todasSelecoes[i]->getNome() << ")\n" << endl;
                return exato;
            }
        }

        vector<OpcaoBusca> opcoes;

        for (int i = 0; i < totalSelecoes; i++) {
            vector<Jogador*> encontrados = todasSelecoes[i]->buscarJogadoresPorPrefixo(termo);
            for (Jogador* jogador : encontrados) {
                opcoes.push_back({jogador, todasSelecoes[i]});
            }
        }

        if (opcoes.empty()) {
            cout << "[ERRO] Nenhum jogador encontrado com esse inicio de nome ou sobrenome. Tente novamente.\n" << endl;
            continue;
        }

        if (opcoes.size() == 1) {
            cout << "-> Encontrado com sucesso: " << opcoes[0].jogador->getNome()
                 << " (" << opcoes[0].selecao->getNome() << ")\n" << endl;
            return opcoes[0].jogador;
        }

        cout << "\nOpcoes encontradas:\n";
        for (size_t i = 0; i < opcoes.size(); i++) {
            cout << (i + 1) << ". " << opcoes[i].jogador->getNome()
                 << " (" << opcoes[i].selecao->getNome() << ")\n";
        }

        cout << "Escolha o numero do jogador correto: ";
        int escolha = 0;
        cin >> escolha;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERRO] Entrada invalida! Digite apenas numeros.\n" << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        if (escolha < 1 || escolha > static_cast<int>(opcoes.size())) {
            cout << "[ERRO] Numero fora das opcoes! Tente novamente.\n" << endl;
            continue;
        }

        cout << "-> Selecionado: " << opcoes[escolha - 1].jogador->getNome()
             << " (" << opcoes[escolha - 1].selecao->getNome() << ")\n" << endl;
        return opcoes[escolha - 1].jogador;
    }
}

int main() {
    srand(time(0)); 

    vector<Selecao*> catalogoSelecoes;
    unordered_map<string, vector<Selecao*>> gruposDaCopa;
    vector<string> ordemGrupos;
    vector<string> caminhosCSV = {
        "./convocados_copa_2026_atualizado.csv",
        "/Users/jorgedaher23/Downloads/convocados_copa_2026_atualizado.csv"
    };

    bool carregouCSV = carregarSelecoesDoCSV(caminhosCSV, catalogoSelecoes, gruposDaCopa, ordemGrupos);

    if (!carregouCSV) {
        cout << "[AVISO] CSV nao encontrado ou vazio. Carregando dados de teste.\n" << endl;

        Selecao* brasil = new Selecao("Brasil", "America do Sul", 5);
        Selecao* franca = new Selecao("Franca", "Europa", 2);
        Selecao* marrocos = new Selecao("Marrocos", "Africa", 0);
        Selecao* escocia = new Selecao("Escocia", "Europa", 0);

        brasil->convocarJogador(new Jogador("Vinicius Jr", "Atacante", 7));
        brasil->convocarJogador(new Jogador("Alisson", "Goleiro", 1));
        franca->convocarJogador(new Jogador("Kylian Mbappe", "Atacante", 10));

        catalogoSelecoes.push_back(brasil);
        catalogoSelecoes.push_back(franca);
        catalogoSelecoes.push_back(marrocos);
        catalogoSelecoes.push_back(escocia);

        gruposDaCopa["A"] = {brasil, franca, marrocos, escocia};
        ordemGrupos.push_back("A");
    }

    if (catalogoSelecoes.size() < 4) {
        cout << "[ERRO] Sao necessarias pelo menos 4 selecoes para simular o torneio." << endl;
        for (Selecao* selecao : catalogoSelecoes) {
            delete selecao;
        }
        return 1;
    }

    cout << "=============================================" << endl;
    cout << "        BEM-VINDO A COPA VISION 2026         " << endl;
    cout << "=============================================" << endl;
    
    // 2. Simulacao de todos os grupos
    unordered_map<string, ClassificacaoGrupo> classificacaoPorGrupo;

    for (const string& idGrupo : ordemGrupos) {
        vector<Selecao*>& timesDoGrupo = gruposDaCopa[idGrupo];

        if (timesDoGrupo.size() < 4) {
            cout << "[AVISO] Grupo " << idGrupo << " ignorado: menos de 4 selecoes cadastradas." << endl;
            continue;
        }

        Grupo grupoAtual("Grupo " + idGrupo);
        grupoAtual.adicionarSelecao(timesDoGrupo[0]);
        grupoAtual.adicionarSelecao(timesDoGrupo[1]);
        grupoAtual.adicionarSelecao(timesDoGrupo[2]);
        grupoAtual.adicionarSelecao(timesDoGrupo[3]);

        grupoAtual.definirPosicoesManuais();
        grupoAtual.exibirTabela();

        classificacaoPorGrupo[idGrupo] = {
            grupoAtual.getPrimeiro(),
            grupoAtual.getSegundo(),
            grupoAtual.getTerceiro(),
            nullptr
        };
    }

    if (classificacaoPorGrupo.size() < 12) {
        cout << "[ERRO] Sao necessarios os 12 grupos completos para montar o mata-mata oficial." << endl;
        for (Selecao* selecao : catalogoSelecoes) {
            delete selecao;
        }
        return 1;
    }

    vector<vector<string>> slotsTerceiros = {
        {"A", "B", "C", "D", "F"},
        {"C", "D", "F", "G", "H"},
        {"C", "E", "F", "H", "I"},
        {"E", "H", "I", "J", "K"},
        {"B", "E", "F", "I", "J"},
        {"A", "E", "H", "I", "J"},
        {"E", "F", "G", "I", "J"},
        {"D", "E", "I", "J", "L"}
    };

    unordered_map<string, Selecao*> melhoresTerceiros = selecionarMelhoresTerceiros(
        classificacaoPorGrupo,
        ordemGrupos,
        slotsTerceiros);

    vector<pair<string, Selecao*>> atribuicoesTerceiros;
    if (!atribuirTerceirosAosJogos(slotsTerceiros, 0, melhoresTerceiros, atribuicoesTerceiros) || atribuicoesTerceiros.size() != 8) {
        cout << "[ERRO] Nao foi possivel montar os 16-avos com os terceiros escolhidos." << endl;
        for (Selecao* selecao : catalogoSelecoes) {
            delete selecao;
        }
        return 1;
    }

    vector<pair<Selecao*, Selecao*>> jogos16avos = {
        {classificacaoPorGrupo.at("A").segundo, classificacaoPorGrupo.at("B").segundo},
        {classificacaoPorGrupo.at("E").primeiro, atribuicoesTerceiros[0].second},
        {classificacaoPorGrupo.at("F").primeiro, classificacaoPorGrupo.at("C").segundo},
        {classificacaoPorGrupo.at("C").primeiro, classificacaoPorGrupo.at("F").segundo},
        {classificacaoPorGrupo.at("I").primeiro, atribuicoesTerceiros[1].second},
        {classificacaoPorGrupo.at("E").segundo, classificacaoPorGrupo.at("I").segundo},
        {classificacaoPorGrupo.at("A").primeiro, atribuicoesTerceiros[2].second},
        {classificacaoPorGrupo.at("L").primeiro, atribuicoesTerceiros[3].second},
        {classificacaoPorGrupo.at("D").primeiro, atribuicoesTerceiros[4].second},
        {classificacaoPorGrupo.at("G").primeiro, atribuicoesTerceiros[5].second},
        {classificacaoPorGrupo.at("K").segundo, classificacaoPorGrupo.at("L").segundo},
        {classificacaoPorGrupo.at("H").primeiro, classificacaoPorGrupo.at("J").segundo},
        {classificacaoPorGrupo.at("B").primeiro, atribuicoesTerceiros[6].second},
        {classificacaoPorGrupo.at("J").primeiro, classificacaoPorGrupo.at("H").segundo},
        {classificacaoPorGrupo.at("K").primeiro, atribuicoesTerceiros[7].second},
        {classificacaoPorGrupo.at("D").segundo, classificacaoPorGrupo.at("G").segundo}
    };

    vector<Selecao*> classificados16avos = simularRodadaMataMata("16-avos de Final", 73, jogos16avos);

    vector<pair<Selecao*, Selecao*>> jogosOitavas = {
        {classificados16avos[0], classificados16avos[1]},
        {classificados16avos[2], classificados16avos[3]},
        {classificados16avos[4], classificados16avos[5]},
        {classificados16avos[6], classificados16avos[7]},
        {classificados16avos[8], classificados16avos[9]},
        {classificados16avos[10], classificados16avos[11]},
        {classificados16avos[12], classificados16avos[13]},
        {classificados16avos[14], classificados16avos[15]}
    };

    vector<Selecao*> classificadosOitavas = simularRodadaMataMata("Oitavas de Final", 89, jogosOitavas);

    vector<pair<Selecao*, Selecao*>> jogosQuartas = {
        {classificadosOitavas[0], classificadosOitavas[1]},
        {classificadosOitavas[2], classificadosOitavas[3]},
        {classificadosOitavas[4], classificadosOitavas[5]},
        {classificadosOitavas[6], classificadosOitavas[7]}
    };

    vector<Selecao*> classificadosQuartas = simularRodadaMataMata("Quartas de Final", 97, jogosQuartas);

    vector<pair<Selecao*, Selecao*>> jogosSemis = {
        {classificadosQuartas[0], classificadosQuartas[1]},
        {classificadosQuartas[2], classificadosQuartas[3]}
    };

    vector<Selecao*> classificadosSemis = simularRodadaMataMata("Semifinal", 101, jogosSemis);

    vector<pair<Selecao*, Selecao*>> jogoFinal = {
        {classificadosSemis[0], classificadosSemis[1]}
    };

    vector<Selecao*> campeao = simularRodadaMataMata("Final", 103, jogoFinal);

    if (!campeao.empty() && campeao[0] != nullptr) {
        cout << "\n=================================================" << endl;
        cout << " CAMPEAO DA COPA VISION: " << campeao[0]->getNome() << "!!!" << endl;
        cout << "=================================================" << endl;
    }

    // 4. Premiacoes
    cout << "\n=============================================" << endl;
    cout << "             PREMIACOES DA COPA              " << endl;
    cout << "=============================================" << endl;

    cout << "\n--- JOGADORES DISPONIVEIS POR EQUIPE ---" << endl;
    for (size_t i = 0; i < catalogoSelecoes.size(); i++) {
        catalogoSelecoes[i]->exibirElenco();
    }

    cout << "\n--- ESCOLHA O MELHOR JOGADOR DA COPA ---\n";
    Jogador* melhorJogador = barraDePesquisa(catalogoSelecoes.data(), static_cast<int>(catalogoSelecoes.size()));

    cout << "\n--- ESCOLHA A REVELACAO DA COPA ---\n";
    Jogador* revelacao = barraDePesquisa(catalogoSelecoes.data(), static_cast<int>(catalogoSelecoes.size()));

    cout << "\n--- ESCOLHA O MELHOR GOLEIRO ---\n";
    Jogador* melhorGoleiro = barraDePesquisa(catalogoSelecoes.data(), static_cast<int>(catalogoSelecoes.size()));

    cout << "\n=============================================" << endl;
    cout << "              QUADRO DE HONRA                " << endl;
    cout << "=============================================" << endl;
    if (melhorJogador) cout << "Melhor Jogador: " << melhorJogador->getNome() << endl;
    if (revelacao) cout << "Revelacao: " << revelacao->getNome() << endl;
    if (melhorGoleiro) cout << "Melhor Goleiro: " << melhorGoleiro->getNome() << endl;
    cout << "=============================================" << endl;

    // Limpeza (Nao esquecer na versao final)
    // Limpeza (Nao esquecer na versao final)
    for (Selecao* selecao : catalogoSelecoes) {
        delete selecao;
    }

    return 0;
}