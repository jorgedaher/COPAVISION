import { useState, useCallback } from 'react';
import Dashboard      from './components/Dashboard.jsx';
import GroupStage     from './components/GroupStage.jsx';
import KnockoutBracket from './components/KnockoutBracket.jsx';
import PlayerSearch   from './components/PlayerSearch.jsx';
import HistoricalStats from './components/HistoricalStats.jsx';
import Curiosities    from './components/Curiosities.jsx';
import HallOfFame     from './components/HallOfFame.jsx';
import { resetTorneio } from './api/copaApi.js';

const PAGES = [
  { id: 'simulation', label: '⚽ Simulação' },
  { id: 'players',    label: '👥 Seleções' },
  { id: 'history',    label: '📊 Histórico' },
  { id: 'curiosities',label: '💡 Curiosidades' },
];

export default function App() {
  const [page, setPage] = useState('home');
  const [serverOk, setServerOk] = useState(null); // null=untested, true, false

  // Sub-page of simulation
  const [simPage, setSimPage] = useState('groups'); // groups | bracket | halloffame

  const navigate = useCallback((p) => {
    setPage(p);
    if (p === 'simulation') setSimPage('groups');
  }, []);

  const handleReset = async () => {
    if (!confirm('Reiniciar o torneio? Todo o progresso será perdido.')) return;
    try { await resetTorneio(); window.location.reload(); } catch(e) { alert(e.message); }
  };

  return (
    <div className="app-shell">
      {/* ── Topbar ── */}
      <header className="topbar">
        <div className="topbar-logo" onClick={() => setPage('home')}>
          🏆 COPA VISION
        </div>
        <nav className="topbar-nav">
          {PAGES.map(p => (
            <button
              key={p.id}
              className={`nav-btn${page === p.id ? ' active' : ''}`}
              onClick={() => navigate(p.id)}
            >{p.label}</button>
          ))}
          {page !== 'home' && (
            <button className="nav-btn" style={{ color: 'var(--red)' }} onClick={handleReset}>
              ↺ Reset
            </button>
          )}
        </nav>
      </header>

      {/* ── Content ── */}
      <main className="page-content">
        {page === 'home' && (
          <Dashboard onNavigate={navigate} />
        )}

        {page === 'simulation' && (
          <>
            {/* Sub-nav */}
            <div style={{ display:'flex', gap:'0.5rem', marginBottom:'1.5rem', flexWrap:'wrap' }}>
              {[
                { id:'groups',    label:'📋 Fase de Grupos' },
                { id:'bracket',   label:'🥊 Mata-Mata'      },
                { id:'halloffame',label:'🏅 Premiações'      },
              ].map(s => (
                <button
                  key={s.id}
                  className={`btn ${simPage===s.id ? 'btn-primary' : 'btn-ghost'} btn-sm`}
                  onClick={() => setSimPage(s.id)}
                >{s.label}</button>
              ))}
            </div>
            {simPage === 'groups'    && <GroupStage     onAdvance={() => setSimPage('bracket')} />}
            {simPage === 'bracket'   && <KnockoutBracket onFinish={() => setSimPage('halloffame')} />}
            {simPage === 'halloffame'&& <HallOfFame />}
          </>
        )}

        {page === 'players'     && <PlayerSearch />}
        {page === 'history'     && <HistoricalStats />}
        {page === 'curiosities' && <Curiosities />}
      </main>
    </div>
  );
}
