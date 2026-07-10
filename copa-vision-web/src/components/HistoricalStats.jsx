// HistoricalStats.jsx — Dados históricos por seleção com estatísticas animadas
import { useState, useEffect } from 'react';
import { getHistorico } from '../api/copaApi.js';

function AnimatedNumber({ value, duration = 800 }) {
  const [display, setDisplay] = useState(0);
  useEffect(() => {
    let start = 0;
    const step = Math.ceil(value / (duration / 16));
    const timer = setInterval(() => {
      start += step;
      if (start >= value) { setDisplay(value); clearInterval(timer); }
      else setDisplay(start);
    }, 16);
    return () => clearInterval(timer);
  }, [value, duration]);
  return <>{display}</>;
}

export default function HistoricalStats() {
  const [data, setData]         = useState(null);
  const [loading, setLoading]   = useState(true);
  const [selected, setSelected] = useState('');
  const [search, setSearch]     = useState('');

  useEffect(() => {
    getHistorico()
      .then(setData)
      .catch(() => {})
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <div className="loading-center"><div className="spinner" /></div>;

  if (!data) return (
    <div className="empty-state">
      <div className="empty-icon">⚠️</div>
      <div className="empty-title">Dados históricos não disponíveis</div>
      <div className="empty-desc">Verifique se o servidor está rodando e o CSV carregado.</div>
    </div>
  );

  // All countries with any data
  const allCountries = new Set([
    ...Object.keys(data.titulos),
    ...Object.keys(data.participacoes),
    ...Object.keys(data.artilheiros),
    ...(data.campeas || []).map(c => c.selecao),
  ]);

  const filtered = [...allCountries]
    .filter(c => c.toLowerCase().includes(search.toLowerCase()))
    .sort();

  const titulos     = selected ? (data.titulos[selected]      ?? 0) : null;
  const participacoes = selected ? (data.participacoes[selected] ?? 0) : null;
  const artilheiro  = selected ? (data.artilheiros[selected]  ?? '—') : null;

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">DADOS HISTÓRICOS</div>
          <div className="section-subtitle">Títulos, participações e artilheiros históricos</div>
        </div>
      </div>

      {/* ── Champion list ── */}
      <div style={{ marginBottom:'2rem' }}>
        <h3 style={{ marginBottom:'1rem', color:'var(--gold)' }}>🏆 Seleções Campeãs</h3>
        <div className="grid-4">
          {(data.campeas || []).map((c, i) => (
            <div key={i} className="stat-card" style={{ cursor:'pointer' }}
              onClick={() => setSelected(c.selecao)}>
              <div className="stat-number"><AnimatedNumber value={c.titulos} /></div>
              <div className="stat-label">título{c.titulos !== 1 ? 's' : ''}</div>
              <div className="stat-value">{c.selecao}</div>
            </div>
          ))}
        </div>
      </div>

      {/* ── Country search ── */}
      <div style={{ marginBottom:'1rem' }}>
        <input
          type="text"
          className="input"
          placeholder="Buscar país..."
          value={search}
          onChange={e => setSearch(e.target.value)}
          style={{ maxWidth:320 }}
        />
      </div>

      <div style={{ display:'flex', gap:'0.4rem', flexWrap:'wrap', marginBottom:'1.5rem', maxHeight:160, overflowY:'auto' }}>
        {filtered.map(c => (
          <button
            key={c}
            className={`country-pill${selected===c?' selected':''}`}
            onClick={() => setSelected(selected===c ? '' : c)}
          >{c}</button>
        ))}
      </div>

      {/* ── Country detail ── */}
      {selected && (
        <div className="animate-in">
          <div style={{ fontFamily:'var(--font-display)', fontSize:'2.2rem', letterSpacing:3, marginBottom:'1.25rem', color:'var(--text-primary)' }}>
            {selected}
          </div>
          <div className="grid-3" style={{ marginBottom:'1.5rem' }}>
            <div className="stat-card">
              <div className="stat-number" style={{ color:'var(--gold)' }}>
                <AnimatedNumber value={titulos} />
              </div>
              <div className="stat-label">Títulos Mundiais</div>
            </div>
            <div className="stat-card">
              <div className="stat-number" style={{ color:'var(--green-light)' }}>
                <AnimatedNumber value={participacoes} />
              </div>
              <div className="stat-label">Participações em Copas</div>
            </div>
            <div className="stat-card">
              <div className="stat-label" style={{ marginBottom:'0.5rem' }}>Maior Artilheiro Histórico</div>
              <div className="stat-value" style={{ fontSize:'1.2rem' }}>{artilheiro}</div>
            </div>
          </div>
        </div>
      )}

      {!selected && (
        <div className="empty-state" style={{ padding:'2rem' }}>
          <div className="empty-icon">📊</div>
          <div className="empty-title">Selecione um país</div>
          <div className="empty-desc">Clique em um país acima para ver suas estatísticas detalhadas</div>
        </div>
      )}
    </div>
  );
}
