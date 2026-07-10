// =============================================================
// server.cpp — COPA VISION 2026  REST API Server
// Requer Windows 10+ (exigido pelo cpp-httplib)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00  // Windows 10
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif
// NOVO ARQUIVO: nenhum .hpp/.cpp existente é modificado.
// Inclui os headers originais do projeto e adiciona uma camada
// HTTP usando cpp-httplib (header-only, sem dependencias extras).
//
// Compilar (g++ MinGW, Windows):
//   g++ -std=c++17 -O2 server.cpp SelecoesLoader.cpp DadosHistoricos.cpp ConsoleUtils.cpp -o server.exe -lws2_32
// Rodar:
//   ./server.exe          →  http://localhost:8080
// =============================================================

#define WIN32_LEAN_AND_MEAN
#include "httplib.h"          // cpp-httplib header-only

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <mutex>

// ── Headers existentes do projeto (não modificados) ──────────
#include "Selecao.hpp"
#include "Jogador.hpp"
#include "DadosHistoricos.hpp"
#include "SelecoesLoader.hpp"

using namespace std;

// =============================================================
// JSON helpers — sem biblioteca externa
// =============================================================
namespace J {
    string str(const string& s) {
        string r = "\"";
        for (unsigned char c : s) {
            if      (c == '"')  r += "\\\"";
            else if (c == '\\') r += "\\\\";
            else if (c == '\n') r += "\\n";
            else if (c == '\r') r += "\\r";
            else if (c == '\t') r += "\\t";
            else                r += c;
        }
        r += "\"";
        return r;
    }
    string num(int i)      { return to_string(i); }
    string boolean(bool b) { return b ? "true" : "false"; }
}

// =============================================================
// Dados globais (carregados 1× no startup, read-only depois)
// =============================================================
static vector<Selecao*>                         catalogoSelecoes;
static DadosHistoricosCopa                      dadosHistoricos;
static unordered_map<string, vector<Selecao*>>  gruposDaCopa;
static vector<string>                           ordemGrupos;

// =============================================================
// Estado do torneio
// =============================================================
struct MatchInfo {
    Selecao* team1    = nullptr;
    Selecao* team2    = nullptr;
    int      goals1   = 0;
    int      goals2   = 0;
    bool     played   = false;
    bool     penalties= false;
    bool     penW1    = false;   // true = team1 venceu pênaltis
    Selecao* winner   = nullptr;
};

enum Phase {
    PHASE_GROUPS            = 0,
    PHASE_THIRDS_SELECTION  = 1,
    PHASE_R16               = 2,
    PHASE_R8                = 3,
    PHASE_QF                = 4,
    PHASE_SF                = 5,
    PHASE_FINAL             = 6,
    PHASE_DONE              = 7
};

struct TournamentState {
    Phase phase = PHASE_GROUPS;

    // Grupos: id → [1º,2º,3º,4º]
    unordered_map<string, array<Selecao*,4>> standings;
    unordered_set<string>                    classifiedGroups;

    // 3ºs selecionados (8 grupos)
    unordered_map<string, Selecao*> selectedThirds;

    // Fases mata-mata
    vector<MatchInfo> r16, r8, qf, sf;
    MatchInfo         thirdPlace, final_;

    // Premiações
    Selecao* champion         = nullptr;
    Selecao* thirdPlaceWinner = nullptr;
    string   bestPlayer, revelation, bestGoalkeeper;
};

static TournamentState ts;
static mutex           tsMtx;

// =============================================================
// Slots oficiais para 3ºs colocados (de SimulacaoCopa.cpp)
// =============================================================
static const vector<vector<string>> SLOTS = {
    {"A","B","C","D","F"},
    {"C","D","F","G","H"},
    {"C","E","F","H","I"},
    {"E","H","I","J","K"},
    {"B","E","F","I","J"},
    {"A","E","H","I","J"},
    {"E","F","G","I","J"},
    {"D","E","I","J","L"}
};

// Backtracking para atribuir 3ºs aos slots
static bool assign3rds(size_t idx,
                        unordered_map<string,Selecao*>& avail,
                        vector<pair<string,Selecao*>>& out) {
    if (idx >= SLOTS.size()) return true;
    for (const string& g : SLOTS[idx]) {
        auto it = avail.find(g);
        if (it == avail.end()) continue;
        Selecao* s = it->second;
        avail.erase(it);
        out.push_back({g, s});
        if (assign3rds(idx+1, avail, out)) return true;
        out.pop_back();
        avail[g] = s;
    }
    return false;
}

// =============================================================
// Utilitários de jogo
// =============================================================
static bool processResult(vector<MatchInfo>& stage, int idx,
                           int g1, int g2, bool pen, bool penW1) {
    if (idx < 0 || idx >= (int)stage.size()) return false;
    MatchInfo& m = stage[idx];
    m.goals1 = g1; m.goals2 = g2;
    m.played    = true;
    m.penalties = (g1 == g2);
    m.penW1     = penW1;
    if      (g1 > g2) m.winner = m.team1;
    else if (g2 > g1) m.winner = m.team2;
    else              m.winner = penW1 ? m.team1 : m.team2;
    return true;
}

static bool stageComplete(const vector<MatchInfo>& s) {
    return !s.empty() && all_of(s.begin(), s.end(), [](const MatchInfo& m){ return m.played; });
}

static void buildNextStage(const vector<MatchInfo>& prev, vector<MatchInfo>& next) {
    next.clear();
    for (size_t i = 0; i + 1 < prev.size(); i += 2) {
        MatchInfo m;
        m.team1 = prev[i].winner;
        m.team2 = prev[i+1].winner;
        next.push_back(m);
    }
}

static void setupR16(const vector<pair<string,Selecao*>>& thirds) {
    auto& st = ts.standings;
    ts.r16.resize(16);

    // Bracket oficial (de SimulacaoCopa.cpp, ordem exata)
    ts.r16[0]  = { st["A"][1], st["B"][1]          };
    ts.r16[1]  = { st["E"][0], thirds[0].second     };
    ts.r16[2]  = { st["F"][0], st["C"][1]           };
    ts.r16[3]  = { st["C"][0], st["F"][1]           };
    ts.r16[4]  = { st["I"][0], thirds[1].second     };
    ts.r16[5]  = { st["E"][1], st["I"][1]           };
    ts.r16[6]  = { st["A"][0], thirds[2].second     };
    ts.r16[7]  = { st["L"][0], thirds[3].second     };
    ts.r16[8]  = { st["D"][0], thirds[4].second     };
    ts.r16[9]  = { st["G"][0], thirds[5].second     };
    ts.r16[10] = { st["K"][1], st["L"][1]           };
    ts.r16[11] = { st["H"][0], st["J"][1]           };
    ts.r16[12] = { st["B"][0], thirds[6].second     };
    ts.r16[13] = { st["J"][0], st["H"][1]           };
    ts.r16[14] = { st["K"][0], thirds[7].second     };
    ts.r16[15] = { st["D"][1], st["G"][1]           };
}

static void initStandings() {
    for (const string& g : ordemGrupos) {
        array<Selecao*,4> arr = {};
        auto& v = gruposDaCopa[g];
        for (int i = 0; i < 4 && i < (int)v.size(); i++) arr[i] = v[i];
        ts.standings[g] = arr;
    }
}

// =============================================================
// Serialização JSON
// =============================================================
static string matchToJson(const MatchInfo& m) {
    string j = "{";
    j += "\"team1\":"    + (m.team1  ? J::str(m.team1->getNome())  : "null");
    j += ",\"team2\":"   + (m.team2  ? J::str(m.team2->getNome())  : "null");
    j += ",\"goals1\":"  + J::num(m.goals1);
    j += ",\"goals2\":"  + J::num(m.goals2);
    j += ",\"played\":"  + J::boolean(m.played);
    j += ",\"penalties\":" + J::boolean(m.penalties);
    j += ",\"penW1\":"   + J::boolean(m.penW1);
    j += ",\"winner\":"  + (m.winner ? J::str(m.winner->getNome()) : "null");
    j += "}";
    return j;
}

static string stageJson(const vector<MatchInfo>& v) {
    string j = "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i) j += ",";
        j += matchToJson(v[i]);
    }
    return j + "]";
}

static string phaseName(Phase p) {
    switch(p) {
        case PHASE_GROUPS:           return "Fase de Grupos";
        case PHASE_THIRDS_SELECTION: return "Selecao de Terceiros";
        case PHASE_R16:              return "16-avos de Final";
        case PHASE_R8:               return "Oitavas de Final";
        case PHASE_QF:               return "Quartas de Final";
        case PHASE_SF:               return "Semifinal";
        case PHASE_FINAL:            return "Final";
        case PHASE_DONE:             return "Encerrado";
        default:                     return "";
    }
}

// Retorna todos os jogadores de uma Selecao buscando com cada letra a-z
static vector<Jogador*> getAllPlayers(Selecao* sel) {
    vector<Jogador*> all;
    unordered_set<string> seen;
    for (char c = 'a'; c <= 'z'; c++) {
        string term(1, c);
        for (Jogador* j : sel->buscarJogadoresPorPrefixo(term))
            if (seen.insert(j->getNome()).second) all.push_back(j);
    }
    sort(all.begin(), all.end(), [](Jogador* a, Jogador* b){
        return a->getNumeroCamisa() < b->getNumeroCamisa();
    });
    return all;
}

// =============================================================
// Parsing de JSON da requisição (sem biblioteca externa)
// =============================================================
static string getStr(const string& body, const string& field) {
    auto p = body.find("\"" + field + "\"");
    if (p == string::npos) return "";
    auto c = body.find(':', p); if (c == string::npos) return "";
    auto q1 = body.find('"', c+1); if (q1 == string::npos) return "";
    auto q2 = body.find('"', q1+1); if (q2 == string::npos) return "";
    return body.substr(q1+1, q2-q1-1);
}
static int getInt(const string& body, const string& field) {
    auto p = body.find("\"" + field + "\"");
    if (p == string::npos) return -1;
    auto c = body.find(':', p); if (c == string::npos) return -1;
    size_t i = c+1;
    while (i < body.size() && !isdigit((unsigned char)body[i]) && body[i] != '-') i++;
    string n; while (i < body.size() && (isdigit((unsigned char)body[i]) || body[i]=='-')) n += body[i++];
    try { return stoi(n); } catch(...) { return -1; }
}
static bool getBool(const string& body, const string& field) {
    auto p = body.find("\"" + field + "\"");
    if (p == string::npos) return false;
    auto c = body.find(':', p); if (c == string::npos) return false;
    size_t i = c+1;
    while (i < body.size() && (body[i]==' '||body[i]=='\t')) i++;
    return body.substr(i,4) == "true";
}
static vector<int> getIntArr(const string& body, const string& field) {
    vector<int> r;
    auto p = body.find("\"" + field + "\""); if (p==string::npos) return r;
    auto s = body.find('[', p); auto e = body.find(']', s);
    if (s==string::npos||e==string::npos) return r;
    stringstream ss(body.substr(s+1, e-s-1)); string tok;
    while (getline(ss, tok, ',')) { try { r.push_back(stoi(tok)); } catch(...){} }
    return r;
}
static vector<string> getStrArr(const string& body, const string& field) {
    vector<string> r;
    auto p = body.find("\"" + field + "\""); if (p==string::npos) return r;
    auto s = body.find('[', p); auto e = body.find(']', s);
    if (s==string::npos||e==string::npos) return r;
    stringstream ss(body.substr(s+1, e-s-1)); string tok;
    while (getline(ss, tok, ',')) {
        string v; for (char c : tok) if (c!='"'&&c!=' '&&c!='\t') v+=c;
        if (!v.empty()) r.push_back(v);
    }
    return r;
}

// =============================================================
// CORS
// =============================================================
static void cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin",  "*");
    res.set_header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

// =============================================================
// main()
// =============================================================
int main() {
    srand((unsigned)time(nullptr));

    // ── Carrega CSV ───────────────────────────────────────────
    vector<string> csvPaths  = { "./convocados_copa_2026_atualizado.csv" };
    vector<string> histPaths = { "./dados_historicos_copa.csv" };

    bool loadedCSV = carregarSelecoesDoCSV(csvPaths, catalogoSelecoes, gruposDaCopa, ordemGrupos);
    if (!loadedCSV) {
        cerr << "[AVISO] CSV nao encontrado. Carregando dados de teste.\n";
        carregarSelecoesTeste(catalogoSelecoes, gruposDaCopa, ordemGrupos);
    }
    carregarDadosHistoricosCSV(histPaths, dadosHistoricos);
    initStandings();

    httplib::Server svr;

    // ── CORS pre-flight ────────────────────────────────────────
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res){
        cors(res); res.status = 204;
    });

    // ===========================================================
    // GET /api/status
    // ===========================================================
    svr.Get("/api/status", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        res.set_content(R"({"status":"ok","copa":"COPA VISION 2026"})", "application/json");
    });

    // ===========================================================
    // GET /api/selecoes  — lista todas as 48 seleções
    // ===========================================================
    svr.Get("/api/selecoes", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        string j = "["; bool first = true;
        for (const string& g : ordemGrupos) {
            for (Selecao* s : gruposDaCopa[g]) {
                if (!first) j += ","; first = false;
                j += "{\"nome\":" + J::str(s->getNome())
                   + ",\"grupo\":" + J::str(g) + "}";
            }
        }
        j += "]";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // GET /api/grupos  — estado atual dos grupos
    // ===========================================================
    svr.Get("/api/grupos", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        string j = "["; bool firstG = true;
        for (const string& g : ordemGrupos) {
            if (!firstG) j += ","; firstG = false;
            j += "{\"id\":" + J::str(g)
               + ",\"completo\":" + J::boolean(ts.classifiedGroups.count(g)>0)
               + ",\"timesOriginais\":[";
            auto& orig = gruposDaCopa[g];
            for (size_t i = 0; i < orig.size(); i++) {
                if (i) j += ",";
                j += "{\"idx\":" + J::num((int)i)
                   + ",\"nome\":" + J::str(orig[i]->getNome()) + "}";
            }
            j += "],\"classificados\":[";
            auto it = ts.standings.find(g);
            if (it != ts.standings.end()) {
                for (int i = 0; i < 4; i++) {
                    if (i) j += ",";
                    j += it->second[i]
                        ? "{\"pos\":" + J::num(i+1) + ",\"nome\":" + J::str(it->second[i]->getNome()) + "}"
                        : "null";
                }
            }
            j += "]}";
        }
        j += "]";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // POST /api/grupos/:id/classificar
    // Body: {"ordem":[0,2,1,3]}  (índices 0-based)
    // ===========================================================
    svr.Post("/api/grupos/([A-Z]+)/classificar", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        if (ts.phase != PHASE_GROUPS) {
            res.status = 400;
            res.set_content(R"({"error":"Fase de grupos ja encerrada"})", "application/json");
            return;
        }
        string gid = req.matches[1].str();
        if (gruposDaCopa.find(gid) == gruposDaCopa.end()) {
            res.status = 404;
            res.set_content(R"({"error":"Grupo nao encontrado"})", "application/json");
            return;
        }
        vector<int> ordem = getIntArr(req.body, "ordem");
        if (ordem.size() != 4) {
            res.status = 400;
            res.set_content(R"({"error":"ordem deve ter exatamente 4 indices"})", "application/json");
            return;
        }
        auto& times = gruposDaCopa[gid];
        array<Selecao*,4> novo; unordered_set<int> usados;
        for (int i = 0; i < 4; i++) {
            int idx = ordem[i];
            if (idx < 0 || idx >= (int)times.size() || !usados.insert(idx).second) {
                res.status = 400;
                res.set_content(R"({"error":"Indice invalido ou duplicado"})", "application/json");
                return;
            }
            novo[i] = times[idx];
        }
        ts.standings[gid] = novo;
        ts.classifiedGroups.insert(gid);
        if (ts.classifiedGroups.size() >= ordemGrupos.size())
            ts.phase = PHASE_THIRDS_SELECTION;
        res.set_content(R"({"success":true})", "application/json");
    });

    // ===========================================================
    // POST /api/grupos/simular  — classifica todos os grupos aleatoriamente
    // ===========================================================
    svr.Post("/api/grupos/simular", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        for (const string& g : ordemGrupos) {
            auto& v = gruposDaCopa[g];
            if ((int)v.size() < 4) continue;
            vector<int> idx = {0,1,2,3};
            for (int i = 3; i > 0; i--) swap(idx[i], idx[rand()%(i+1)]);
            array<Selecao*,4> novo;
            for (int i = 0; i < 4; i++) novo[i] = v[idx[i]];
            ts.standings[g] = novo;
            ts.classifiedGroups.insert(g);
        }
        ts.phase = PHASE_THIRDS_SELECTION;
        res.set_content(R"({"success":true})", "application/json");
    });

    // ===========================================================
    // GET /api/terceiros  — retorna 3ºs colocados de cada grupo
    // ===========================================================
    svr.Get("/api/terceiros", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        string j = "["; bool first = true;
        for (const string& g : ordemGrupos) {
            auto it = ts.standings.find(g);
            if (it == ts.standings.end() || !it->second[2]) continue;
            if (!first) j += ","; first = false;
            j += "{\"grupo\":" + J::str(g)
               + ",\"nome\":" + J::str(it->second[2]->getNome()) + "}";
        }
        j += "]";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // POST /api/terceiros  — confirma seleção dos 8 melhores 3ºs
    // Body: {"grupos":["A","B","C","D","E","F","G","H"]}
    // ===========================================================
    svr.Post("/api/terceiros", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        if (ts.phase != PHASE_THIRDS_SELECTION) {
            res.status = 400;
            res.set_content(R"({"error":"Nao e o momento para selecionar terceiros"})", "application/json");
            return;
        }
        vector<string> grupos = getStrArr(req.body, "grupos");
        if (grupos.size() != 8) {
            res.status = 400;
            res.set_content(R"({"error":"Selecione exatamente 8 grupos"})", "application/json");
            return;
        }
        unordered_map<string,Selecao*> thirds;
        for (const string& g : grupos) {
            auto it = ts.standings.find(g);
            if (it == ts.standings.end() || !it->second[2]) {
                res.status = 400;
                res.set_content("{\"error\":\"Grupo invalido: " + g + "\"}", "application/json");
                return;
            }
            thirds[g] = it->second[2];
        }
        // Valida e atribui aos slots
        auto avail = thirds;
        vector<pair<string,Selecao*>> assignments;
        if (!assign3rds(0, avail, assignments) || assignments.size() != 8) {
            res.status = 400;
            res.set_content(R"({"error":"Combinacao invalida: os terceiros escolhidos nao preenchem os slots oficiais"})", "application/json");
            return;
        }
        ts.selectedThirds = thirds;
        setupR16(assignments);
        ts.phase = PHASE_R16;
        res.set_content(R"({"success":true})", "application/json");
    });

    // ===========================================================
    // GET /api/matamata  — estado completo do bracket
    // ===========================================================
    svr.Get("/api/matamata", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        string j = "{";
        j += "\"fase\":"     + J::num((int)ts.phase);
        j += ",\"faseNome\":" + J::str(phaseName(ts.phase));
        j += ",\"r16\":"     + stageJson(ts.r16);
        j += ",\"r8\":"      + stageJson(ts.r8);
        j += ",\"qf\":"      + stageJson(ts.qf);
        j += ",\"sf\":"      + stageJson(ts.sf);
        j += ",\"terceiroLugar\":" + matchToJson(ts.thirdPlace);
        j += ",\"final\":"   + matchToJson(ts.final_);
        j += ",\"campeao\":" + (ts.champion ? J::str(ts.champion->getNome()) : "null");
        j += ",\"terceiroColocado\":" + (ts.thirdPlaceWinner ? J::str(ts.thirdPlaceWinner->getNome()) : "null");
        j += "}";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // POST /api/matamata/resultado
    // Body: {"fase":"r16","jogo":0,"gols1":2,"gols2":1,
    //        "penalties":false,"penWinner1":false}
    // fase: "r16" | "r8" | "qf" | "sf" | "terceiro" | "final"
    // ===========================================================
    svr.Post("/api/matamata/resultado", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);

        string fase = getStr(req.body, "fase");
        int jogo    = getInt(req.body, "jogo");
        int g1      = getInt(req.body, "gols1");
        int g2      = getInt(req.body, "gols2");
        bool pen    = getBool(req.body, "penalties");
        bool penW1  = getBool(req.body, "penWinner1");

        if (g1 < 0 || g2 < 0 || jogo < 0) {
            res.status = 400;
            res.set_content(R"({"error":"Dados invalidos"})", "application/json");
            return;
        }

        // Macro para processar fase e avançar automaticamente
        auto advancePhase = [&](vector<MatchInfo>& stage,
                                Phase expect, Phase nextPhase) -> bool {
            if (ts.phase != expect) return false;
            if (!processResult(stage, jogo, g1, g2, pen, penW1)) return false;
            if (stageComplete(stage)) {
                if (nextPhase == PHASE_R8)  buildNextStage(stage, ts.r8);
                if (nextPhase == PHASE_QF)  buildNextStage(stage, ts.qf);
                if (nextPhase == PHASE_SF)  buildNextStage(stage, ts.sf);
                if (nextPhase == PHASE_FINAL) {
                    // Monta final e disputa de 3º lugar
                    vector<MatchInfo> finalists;
                    buildNextStage(stage, finalists);
                    ts.final_.team1 = finalists[0].team1;
                    ts.final_.team2 = finalists[0].team2;
                    // Perdedores das semis disputam o 3º lugar
                    auto loser = [](const MatchInfo& m) -> Selecao* {
                        return m.winner == m.team1 ? m.team2 : m.team1;
                    };
                    ts.thirdPlace.team1 = loser(stage[0]);
                    ts.thirdPlace.team2 = loser(stage[1]);
                }
                ts.phase = nextPhase;
            }
            return true;
        };

        bool ok = false;
        if      (fase == "r16")    ok = advancePhase(ts.r16, PHASE_R16, PHASE_R8);
        else if (fase == "r8")     ok = advancePhase(ts.r8,  PHASE_R8,  PHASE_QF);
        else if (fase == "qf")     ok = advancePhase(ts.qf,  PHASE_QF,  PHASE_SF);
        else if (fase == "sf")     ok = advancePhase(ts.sf,  PHASE_SF,  PHASE_FINAL);
        else if (fase == "terceiro" && ts.phase == PHASE_FINAL) {
            // thirdPlace e final_ sao MatchInfo simples, nao vector
            auto applyResult = [&](MatchInfo& m) {
                m.goals1 = g1; m.goals2 = g2;
                m.played = true; m.penalties = (g1==g2); m.penW1 = penW1;
                if      (g1>g2) m.winner = m.team1;
                else if (g2>g1) m.winner = m.team2;
                else            m.winner = penW1 ? m.team1 : m.team2;
            };
            applyResult(ts.thirdPlace);
            ts.thirdPlaceWinner = ts.thirdPlace.winner;
            ok = true;
        } else if (fase == "final" && ts.phase == PHASE_FINAL) {
            auto applyResult = [&](MatchInfo& m) {
                m.goals1 = g1; m.goals2 = g2;
                m.played = true; m.penalties = (g1==g2); m.penW1 = penW1;
                if      (g1>g2) m.winner = m.team1;
                else if (g2>g1) m.winner = m.team2;
                else            m.winner = penW1 ? m.team1 : m.team2;
            };
            applyResult(ts.final_);
            ts.champion = ts.final_.winner;
            ts.phase    = PHASE_DONE;
            ok = true;
        }

        if (!ok) {
            res.status = 400;
            res.set_content(R"({"error":"Acao invalida para a fase atual do torneio"})", "application/json");
            return;
        }
        res.set_content(R"({"success":true})", "application/json");
    });

    // ===========================================================
    // GET /api/historico  — dados históricos completos
    // ===========================================================
    svr.Get("/api/historico", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        string j = "{\"campeas\":[";
        for (size_t i = 0; i < dadosHistoricos.campeas.size(); i++) {
            if (i) j += ",";
            j += "{\"selecao\":" + J::str(dadosHistoricos.campeas[i].first)
               + ",\"titulos\":" + J::num(dadosHistoricos.campeas[i].second) + "}";
        }
        j += "],\"titulos\":{";
        bool first = true;
        for (auto& p : dadosHistoricos.titulos) {
            if (!first) j += ","; first = false;
            j += J::str(p.first) + ":" + J::num(p.second);
        }
        j += "},\"participacoes\":{";
        first = true;
        for (auto& p : dadosHistoricos.participacoes) {
            if (!first) j += ","; first = false;
            j += J::str(p.first) + ":" + J::num(p.second);
        }
        j += "},\"artilheiros\":{";
        first = true;
        for (auto& p : dadosHistoricos.artilheiros) {
            if (!first) j += ","; first = false;
            j += J::str(p.first) + ":" + J::str(p.second);
        }
        j += "}}";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // GET /api/curiosidades
    // ===========================================================
    svr.Get("/api/curiosidades", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        string j = "[";
        for (size_t i = 0; i < dadosHistoricos.curiosidades.size(); i++) {
            if (i) j += ",";
            j += J::str(dadosHistoricos.curiosidades[i]);
        }
        j += "]";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // GET /api/jogadores/busca?q=...
    // ===========================================================
    svr.Get("/api/jogadores/busca", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        string termo = req.get_param_value("q");
        if (termo.empty()) { res.set_content("[]", "application/json"); return; }
        string j = "["; bool first = true;
        for (const string& g : ordemGrupos) {
            for (Selecao* s : gruposDaCopa[g]) {
                for (Jogador* jog : s->buscarJogadoresPorPrefixo(termo)) {
                    if (!first) j += ","; first = false;
                    j += "{\"nome\":"    + J::str(jog->getNome())
                       + ",\"posicao\":" + J::str(jog->getPosicao())
                       + ",\"camisa\":"  + J::num(jog->getNumeroCamisa())
                       + ",\"selecao\":" + J::str(s->getNome())
                       + ",\"grupo\":"   + J::str(g) + "}";
                }
            }
        }
        j += "]";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // GET /api/selecoes/:pais/jogadores  — elenco completo de um país
    // ===========================================================
    svr.Get("/api/selecoes/(.+)/jogadores", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        // URL-decode do parâmetro
        string raw = req.matches[1].str();
        string pais;
        for (size_t i = 0; i < raw.size(); i++) {
            if (raw[i]=='%' && i+2<raw.size()) {
                pais += (char)stoi(raw.substr(i+1,2), nullptr, 16); i += 2;
            } else if (raw[i]=='+') { pais += ' ';
            } else { pais += raw[i]; }
        }
        Selecao* found = nullptr; string foundGroup;
        for (const string& g : ordemGrupos) {
            for (Selecao* s : gruposDaCopa[g]) {
                if (s->getNome() == pais) { found = s; foundGroup = g; break; }
            }
            if (found) break;
        }
        if (!found) {
            res.status = 404;
            res.set_content(R"({"error":"Selecao nao encontrada"})", "application/json");
            return;
        }
        auto jogadores = getAllPlayers(found);
        string j = "{\"selecao\":" + J::str(found->getNome())
                 + ",\"grupo\":" + J::str(foundGroup)
                 + ",\"jogadores\":[";
        for (size_t i = 0; i < jogadores.size(); i++) {
            if (i) j += ",";
            j += "{\"nome\":"    + J::str(jogadores[i]->getNome())
               + ",\"posicao\":" + J::str(jogadores[i]->getPosicao())
               + ",\"camisa\":"  + J::num(jogadores[i]->getNumeroCamisa()) + "}";
        }
        j += "]}";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // POST /api/premiacao  — salva premiações finais
    // Body: {"melhorJogador":"...","revelacao":"...","melhorGoleiro":"..."}
    // ===========================================================
    svr.Post("/api/premiacao", [](const httplib::Request& req, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        ts.bestPlayer     = getStr(req.body, "melhorJogador");
        ts.revelation     = getStr(req.body, "revelacao");
        ts.bestGoalkeeper = getStr(req.body, "melhorGoleiro");
        res.set_content(R"({"success":true})", "application/json");
    });

    // ===========================================================
    // GET /api/premiacao
    // ===========================================================
    svr.Get("/api/premiacao", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        string j = "{";
        j += "\"campeao\":"       + (ts.champion ? J::str(ts.champion->getNome()) : "null");
        j += ",\"terceiro\":"     + (ts.thirdPlaceWinner ? J::str(ts.thirdPlaceWinner->getNome()) : "null");
        j += ",\"melhorJogador\":" + J::str(ts.bestPlayer);
        j += ",\"revelacao\":"     + J::str(ts.revelation);
        j += ",\"melhorGoleiro\":" + J::str(ts.bestGoalkeeper);
        j += "}";
        res.set_content(j, "application/json");
    });

    // ===========================================================
    // POST /api/reset  — reinicia o torneio
    // ===========================================================
    svr.Post("/api/reset", [](const httplib::Request&, httplib::Response& res){
        cors(res);
        lock_guard<mutex> lk(tsMtx);
        ts = TournamentState();
        initStandings();
        res.set_content(R"({"success":true})", "application/json");
    });

    // ── Inicia servidor ────────────────────────────────────────
    cout << "\n==============================================" << endl;
    cout << "   COPA VISION 2026  |  API Server           " << endl;
    cout << "   Porta  : 8080                              " << endl;
    cout << "   Status : http://localhost:8080/api/status  " << endl;
    cout << "   Ctrl+C para encerrar                       " << endl;
    cout << "==============================================" << endl;

    svr.listen("0.0.0.0", 8080);
    return 0;
}
