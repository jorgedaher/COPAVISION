// PlayerSearch.jsx — Busca de jogadores e listagem de elencos por seleção
import { useState, useEffect, useRef } from 'react';
import { buscarJogadores, getSelecoes, getJogadores } from '../api/copaApi.js';

const POS_CLASS = { GK:'GK', GL:'GK', G:'GK', DF:'DF', D:'DF', ZAG:'DF', LAT:'DF',
                    MF:'MF', M:'MF', MEI:'MF', VOL:'MF', MC:'MF',
                    FW:'FW', A:'FW', AT:'FW', CA:'FW', PO:'FW', SA:'FW' };

function posClass(posicao) {
  const key = posicao?.toUpperCase().split('/')[0].trim();
  for (const k of Object.keys(POS_CLASS)) {
    if (key?.startsWith(k)) return POS_CLASS[k];
  }
  return 'MF';
}

function PlayerCard({ j }) {
  const pc = posClass(j.posicao);
  return (
    <div className="player-card animate-in">
      <div className="player-number">{j.camisa}</div>
      <div className="player-info">
        <div className="player-name">{j.nome}</div>
        <div className="player-meta">
          <span className={`pos-badge pos-${pc}`}>{j.posicao}</span>
          {j.selecao && <span className="player-team">🏳 {j.selecao}</span>}
        </div>
      </div>
    </div>
  );
}

export default function PlayerSearch() {
  const [tab, setTab]           = useState('search'); // 'search' | 'team'
  const [query, setQuery]       = useState('');
  const [results, setResults]   = useState([]);
  const [searching, setSearching] = useState(false);

  const [selecoes, setSelecoes] = useState([]);
  const [selGroup, setSelGroup] = useState('all');
  const [selPais, setSelPais]   = useState('');
  const [squad, setSquad]       = useState(null);
  const [squadLoading, setSquadLoading] = useState(false);

  const debounceRef = useRef(null);

  // Load teams list on mount
  useEffect(() => {
    getSelecoes().then(setSelecoes).catch(() => {});
  }, []);

  // Live search
  useEffect(() => {
    if (tab !== 'search') return;
    if (query.trim().length < 2) { setResults([]); return; }
    setSearching(true);
    clearTimeout(debounceRef.current);
    debounceRef.current = setTimeout(async () => {
      try {
        const data = await buscarJogadores(query.trim());
        setResults(data);
      } catch (e) { setResults([]); }
      setSearching(false);
    }, 300);
    return () => clearTimeout(debounceRef.current);
  }, [query, tab]);

  // Load squad when country selected
  useEffect(() => {
    if (!selPais) { setSquad(null); return; }
    setSquadLoading(true);
    getJogadores(selPais)
      .then(setSquad)
      .catch(() => setSquad(null))
      .finally(() => setSquadLoading(false));
  }, [selPais]);

  // Groups list for filter
  const groups = [...new Set(selecoes.map(s => s.grupo))].sort();
  const filteredSelecoes = selGroup === 'all' ? selecoes : selecoes.filter(s => s.grupo === selGroup);

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">SELEÇÕES</div>
          <div className="section-subtitle">Busque jogadores ou explore o elenco de cada país</div>
        </div>
        <div style={{ display:'flex', gap:'0.5rem' }}>
          <button className={`btn btn-sm ${tab==='search'?'btn-primary':'btn-ghost'}`} onClick={() => setTab('search')}>
            🔍 Buscar Jogador
          </button>
          <button className={`btn btn-sm ${tab==='team'?'btn-primary':'btn-ghost'}`} onClick={() => setTab('team')}>
            🏳 Elenco por Seleção
          </button>
        </div>
      </div>

      {/* ── TAB: Search ── */}
      {tab === 'search' && (
        <>
          <div className="search-bar-wrap">
            <span className="search-icon">🔍</span>
            <input
              id="player-search-input"
              type="text"
              className="search-input"
              placeholder="Digite o nome ou prefixo do jogador (ex: Vin, Mbap, Rodri...)"
              value={query}
              onChange={e => setQuery(e.target.value)}
              autoFocus
            />
          </div>

          {searching && (
            <div style={{ textAlign:'center', padding:'1rem', color:'var(--text-muted)' }}>
              Buscando...
            </div>
          )}

          {!searching && query.length >= 2 && results.length === 0 && (
            <div className="empty-state">
              <div className="empty-icon">🤷</div>
              <div className="empty-title">Nenhum jogador encontrado</div>
              <div className="empty-desc">Tente um nome diferente ou verifique a ortografia</div>
            </div>
          )}

          {!searching && results.length > 0 && (
            <>
              <div style={{ fontSize:'0.82rem', color:'var(--text-muted)', marginBottom:'0.75rem' }}>
                {results.length} jogador{results.length !== 1 ? 'es' : ''} encontrado{results.length !== 1 ? 's' : ''}
              </div>
              <div className="grid-auto">
                {results.map((j, i) => <PlayerCard key={i} j={j} />)}
              </div>
            </>
          )}

          {query.length < 2 && (
            <div className="empty-state">
              <div className="empty-icon">⚽</div>
              <div className="empty-title">Digite pelo menos 2 letras para buscar</div>
              <div className="empty-desc">
                A busca funciona por prefixo de nome ou sobrenome, igual à função <code>barraDePesquisa</code> do C++
              </div>
            </div>
          )}
        </>
      )}

      {/* ── TAB: Team Squad ── */}
      {tab === 'team' && (
        <>
          {/* Group filter */}
          <div style={{ display:'flex', gap:'0.5rem', marginBottom:'1rem', flexWrap:'wrap' }}>
            <button
              className={`country-pill${selGroup==='all'?' selected':''}`}
              onClick={() => setSelGroup('all')}
            >Todos</button>
            {groups.map(g => (
              <button
                key={g}
                className={`country-pill${selGroup===g?' selected':''}`}
                onClick={() => setSelGroup(g)}
              >Grupo {g}</button>
            ))}
          </div>

          {/* Country list */}
          <div style={{ display:'flex', gap:'0.6rem', flexWrap:'wrap', marginBottom:'1.5rem' }}>
            {filteredSelecoes.map(s => (
              <button
                key={s.nome}
                className={`country-pill${selPais===s.nome?' selected':''}`}
                onClick={() => setSelPais(s.nome)}
              >
                {s.nome}
              </button>
            ))}
          </div>

          {/* Squad */}
          {squadLoading && <div className="loading-center"><div className="spinner" /></div>}

          {!squadLoading && squad && (
            <div>
              <div style={{ marginBottom:'1.25rem' }}>
                <div style={{ fontFamily:'var(--font-display)', fontSize:'2rem', letterSpacing:2, color:'var(--text-primary)' }}>
                  {squad.selecao}
                </div>
                <div style={{ fontSize:'0.82rem', color:'var(--text-muted)' }}>
                  Grupo {squad.grupo} · {squad.jogadores.length} jogadores convocados
                </div>
              </div>
              <div className="grid-auto">
                {squad.jogadores.map((j, i) => <PlayerCard key={i} j={j} />)}
              </div>
            </div>
          )}

          {!squadLoading && !squad && selPais && (
            <div className="empty-state">
              <div className="empty-icon">⚠️</div>
              <div className="empty-title">Elenco não carregado</div>
            </div>
          )}

          {!selPais && (
            <div className="empty-state">
              <div className="empty-icon">🏳</div>
              <div className="empty-title">Selecione uma seleção acima</div>
              <div className="empty-desc">Filtre por grupo para encontrar mais facilmente</div>
            </div>
          )}
        </>
      )}
    </div>
  );
}
