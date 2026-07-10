// KnockoutBracket.jsx — Visualização interativa do bracket mata-mata
import { useState, useEffect, useCallback } from 'react';
import { getMataMata } from '../api/copaApi.js';
import MatchModal from './MatchModal.jsx';

const PHASES = [
  { key:'r16', label:'16-avos', count:16 },
  { key:'r8',  label:'Oitavas', count:8  },
  { key:'qf',  label:'Quartas', count:4  },
  { key:'sf',  label:'Semifinal',count:2 },
  { key:'final',label:'Final',  count:1  },
];

const PHASE_NUM = { r16:2, r8:3, qf:4, sf:5, final:6, terceiro:6, done:7 };

function MatchCard({ match, fase, idx, currentPhase, onClick }) {
  if (!match) return <div className="bracket-match" style={{ height:62, opacity:0.3 }} />;

  const phaseNum = PHASE_NUM[fase] ?? 0;
  const isPlayable = match.team1 && match.team2 && !match.played && currentPhase === phaseNum;
  const isDone = match.played;

  const teamRow = (name, goals, isWinner) => (
    <div className={`bracket-team${isWinner?' winner':isDone?' loser':''}${!name?' empty':''}`}>
      <span style={{ flex:1, overflow:'hidden', textOverflow:'ellipsis', whiteSpace:'nowrap' }}>
        {name || '—'}
      </span>
      {isDone && <span className="bracket-goals">{goals}</span>}
    </div>
  );

  return (
    <div
      className={`bracket-match${isPlayable?' playable':''}${isDone?' done':''}`}
      onClick={() => isPlayable && onClick(match, fase, idx)}
      title={isPlayable ? 'Clique para inserir o resultado' : ''}
    >
      {teamRow(match.team1, match.goals1, match.winner === match.team1)}
      {teamRow(match.team2, match.goals2, match.winner === match.team2)}
      {match.penalties && isDone && (
        <div style={{ fontSize:'0.65rem', color:'var(--gold)', textAlign:'center', padding:'2px 6px', background:'rgba(255,215,0,0.08)' }}>
          Pênaltis
        </div>
      )}
      {isPlayable && <span className="play-icon">▶</span>}
    </div>
  );
}

export default function KnockoutBracket({ onFinish }) {
  const [data, setData]       = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError]     = useState('');
  const [modal, setModal]     = useState(null); // {match,fase,idx}

  const load = useCallback(async () => {
    try {
      const d = await getMataMata();
      setData(d);
      if (d.fase === 7) onFinish(); // PHASE_DONE
    } catch (e) {
      setError(e.message);
    } finally {
      setLoading(false);
    }
  }, [onFinish]);

  useEffect(() => { load(); }, [load]);

  const openModal = (match, fase, idx) => setModal({ match, fase, idx });
  const closeModal = () => setModal(null);
  const afterSave = () => { load(); };

  if (loading) return <div className="loading-center"><div className="spinner" /></div>;

  if (error) return (
    <div className="empty-state">
      <div className="empty-icon">⚠️</div>
      <div className="empty-title">Servidor não encontrado</div>
      <div className="empty-desc">{error}</div>
      <button className="btn btn-primary" style={{ marginTop:'1rem' }} onClick={load}>Tentar novamente</button>
    </div>
  );

  if (!data) return null;

  const { r16=[], r8=[], qf=[], sf=[], terceiroLugar, final:fin, fase, faseNome, campeao } = data;

  const stageSets = [
    { key:'r16',  matches: r16,  label:'16-avos de Final'  },
    { key:'r8',   matches: r8,   label:'Oitavas de Final'  },
    { key:'qf',   matches: qf,   label:'Quartas de Final'  },
    { key:'sf',   matches: sf,   label:'Semifinal'         },
    { key:'final',matches: fin?.team1 ? [fin] : [], label:'Final' },
  ];

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">MATA-MATA</div>
          <div className="section-subtitle">Clique em uma partida disponível para inserir o resultado</div>
        </div>
        <button className="btn btn-ghost btn-sm" onClick={load}>🔄 Atualizar</button>
      </div>

      {/* Phase banner */}
      <div className="phase-banner">
        <div>
          <div className="phase-label">Fase atual</div>
          <div className="phase-name">{faseNome}</div>
        </div>
        {campeao && (
          <div style={{ marginLeft:'auto', textAlign:'right' }}>
            <div style={{ fontSize:'0.75rem', color:'var(--gold)' }}>🏆 CAMPEÃO</div>
            <div style={{ fontFamily:'var(--font-display)', fontSize:'1.4rem', color:'var(--gold)' }}>{campeao}</div>
          </div>
        )}
      </div>

      {/* Big bracket — scrollable */}
      <div className="bracket-container">
        <div className="bracket-scroll">
          {stageSets.map(({ key, matches, label }) => (
            <div key={key} className="bracket-round">
              <div className="bracket-round-title">{label}</div>
              {matches.length === 0 ? (
                <div style={{ flex:1, display:'flex', flexDirection:'column', justifyContent:'space-around' }}>
                  {[...Array(key==='r16'?16:key==='r8'?8:key==='qf'?4:key==='sf'?2:1)].map((_,i) => (
                    <MatchCard key={i} match={null} fase={key} idx={i}
                      currentPhase={fase} onClick={() => {}} />
                  ))}
                </div>
              ) : (
                <div style={{ flex:1, display:'flex', flexDirection:'column', justifyContent:'space-around' }}>
                  {matches.map((m, i) => (
                    <MatchCard
                      key={i} match={m} fase={key} idx={i}
                      currentPhase={fase} onClick={openModal}
                    />
                  ))}
                </div>
              )}
            </div>
          ))}
        </div>
      </div>

      {/* 3rd place match — always shown separately */}
      {(fase >= 6) && (
        <div style={{ marginTop:'2rem' }}>
          <div style={{
            fontSize:'0.9rem', color:'var(--text-muted)', textTransform:'uppercase',
            letterSpacing:1, marginBottom:'0.75rem'
          }}>🥉 Disputa do 3º Lugar</div>
          <div style={{ maxWidth:220 }}>
            {terceiroLugar && (
              <MatchCard
                match={terceiroLugar} fase="terceiro" idx={0}
                currentPhase={fase} onClick={openModal}
              />
            )}
          </div>
        </div>
      )}

      {campeao && (
        <div style={{ textAlign:'center', marginTop:'2rem' }}>
          <button className="btn btn-gold btn-lg" onClick={onFinish}>
            🏅 Ver Premiações →
          </button>
        </div>
      )}

      {/* Match Modal */}
      {modal && (
        <MatchModal
          match={modal.match}
          fase={modal.fase}
          jogoIdx={modal.idx}
          onClose={closeModal}
          onSaved={afterSave}
        />
      )}
    </div>
  );
}
