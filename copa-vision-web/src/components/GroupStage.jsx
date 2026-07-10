// GroupStage.jsx — Fase de grupos com classificação manual ou automática
import { useState, useEffect } from 'react';
import { getGrupos, classificarGrupo, simularGrupos } from '../api/copaApi.js';
import ThirdPlaceSelection from './ThirdPlaceSelection.jsx';

export default function GroupStage({ onAdvance }) {
  const [grupos, setGrupos]         = useState([]);
  const [loading, setLoading]       = useState(true);
  const [error, setError]           = useState('');
  const [phase, setPhase]           = useState(null);
  const [saving, setSaving]         = useState({});
  const [showThirds, setShowThirds] = useState(false);

  // For each group, track the local selection dropdowns
  const [selections, setSelections] = useState({});

  const load = async () => {
    try {
      const data = await getGrupos();
      setGrupos(data);
      // Init selections if not yet set
      setSelections(prev => {
        const next = { ...prev };
        data.forEach(g => {
          if (!next[g.id]) {
            next[g.id] = [0, 1, 2, 3]; // default order
          }
        });
        return next;
      });

      // Check if all groups done → advance to thirds selection
      if (data.length > 0 && data.every(g => g.completo)) {
        setPhase('thirds');
      }
    } catch (e) {
      setError('Não foi possível conectar ao servidor C++. Verifique se server.exe está rodando em http://localhost:8080');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => { load(); }, []);

  const handleSimular = async () => {
    setLoading(true);
    try {
      await simularGrupos();
      await load();
    } catch (e) { setError(e.message); setLoading(false); }
  };

  const handleClassificar = async (gid) => {
    const ordem = selections[gid];
    // Check no duplicates
    if (new Set(ordem).size !== 4) {
      setError('Escolha 4 posições diferentes para o Grupo ' + gid);
      return;
    }
    setSaving(s => ({ ...s, [gid]: true }));
    try {
      await classificarGrupo(gid, ordem);
      await load();
    } catch (e) { setError(e.message); }
    setSaving(s => ({ ...s, [gid]: false }));
  };

  const updateSel = (gid, pos, val) => {
    setSelections(prev => {
      const arr = [...(prev[gid] || [0,1,2,3])];
      arr[pos] = parseInt(val);
      return { ...prev, [gid]: arr };
    });
  };

  const allDone = grupos.length > 0 && grupos.every(g => g.completo);
  const donePct = grupos.length ? Math.round((grupos.filter(g => g.completo).length / grupos.length) * 100) : 0;

  if (loading) return (
    <div className="loading-center">
      <div className="spinner" />
      <span style={{ color:'var(--text-muted)' }}>Carregando grupos...</span>
    </div>
  );

  if (error) return (
    <div className="empty-state">
      <div className="empty-icon">⚠️</div>
      <div className="empty-title">Servidor não encontrado</div>
      <div className="empty-desc" style={{ maxWidth:500 }}>{error}</div>
      <button className="btn btn-primary" style={{ marginTop:'1rem' }} onClick={load}>Tentar novamente</button>
    </div>
  );

  if (showThirds) return (
    <ThirdPlaceSelection
      onBack={() => setShowThirds(false)}
      onConfirm={() => { setShowThirds(false); onAdvance(); }}
    />
  );

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">FASE DE GRUPOS</div>
          <div className="section-subtitle">
            {grupos.filter(g => g.completo).length}/{grupos.length} grupos classificados
          </div>
        </div>
        <div style={{ display:'flex', gap:'0.5rem', flexWrap:'wrap' }}>
          <button className="btn btn-ghost btn-sm" onClick={handleSimular}>
            🎲 Simular Tudo
          </button>
          {allDone && (
            <button className="btn btn-gold btn-sm" onClick={() => setShowThirds(true)}>
              🏆 Selecionar Melhores Terceiros →
            </button>
          )}
        </div>
      </div>

      {/* Progress bar */}
      <div style={{ marginBottom:'1.5rem' }}>
        <div style={{ height:4, background:'var(--border)', borderRadius:999, overflow:'hidden' }}>
          <div style={{
            height:'100%', width:`${donePct}%`,
            background:'linear-gradient(90deg,var(--green-primary),var(--green-light))',
            borderRadius:999, transition:'width 0.4s ease'
          }} />
        </div>
        <div style={{ fontSize:'0.78rem', color:'var(--text-muted)', marginTop:4 }}>
          {donePct}% concluído
        </div>
      </div>

      {/* Groups grid */}
      <div className="groups-grid">
        {grupos.map(g => (
          <div key={g.id} className={`group-card${g.completo ? ' done' : ''}`}>
            <div className="group-header">
              <span className="group-letter">GRUPO {g.id}</span>
              {g.completo
                ? <span className="badge badge-green">✓ Classificado</span>
                : <span className="badge badge-gray">Pendente</span>
              }
            </div>

            <div className="group-body">
              {g.completo ? (
                /* Show standings */
                g.classificados.map((t, i) => t && (
                  <div key={i} className="group-team-row">
                    <span className={`group-pos${i===0?' p1':i===1?' p2':i===2?' p3':''}`}>{i+1}º</span>
                    <span className="group-name">{t.nome}</span>
                    {i < 2 && <span className="badge badge-green" style={{ fontSize:'0.65rem' }}>Classif.</span>}
                    {i === 2 && <span className="badge badge-gray" style={{ fontSize:'0.65rem' }}>3º</span>}
                  </div>
                ))
              ) : (
                /* Show selects */
                [0,1,2,3].map(pos => (
                  <div key={pos} className="group-team-row">
                    <span className="group-pos">{pos+1}º</span>
                    <select
                      className="group-select"
                      value={selections[g.id]?.[pos] ?? pos}
                      onChange={e => updateSel(g.id, pos, e.target.value)}
                    >
                      {g.timesOriginais.map(t => (
                        <option key={t.idx} value={t.idx}>{t.nome}</option>
                      ))}
                    </select>
                  </div>
                ))
              )}
            </div>

            {!g.completo && (
              <div className="group-actions">
                <button
                  className="btn btn-primary btn-sm"
                  style={{ flex:1 }}
                  disabled={saving[g.id]}
                  onClick={() => handleClassificar(g.id)}
                >
                  {saving[g.id] ? 'Salvando...' : '✓ Confirmar'}
                </button>
              </div>
            )}
          </div>
        ))}
      </div>
    </div>
  );
}
