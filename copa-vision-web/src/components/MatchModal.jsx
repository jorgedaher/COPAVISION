// MatchModal.jsx — Modal para inserção de gols, prorrogação e pênaltis
import { useState } from 'react';
import { submitResultado } from '../api/copaApi.js';

export default function MatchModal({ match, fase, jogoIdx, onClose, onSaved }) {
  const [g1, setG1] = useState('');
  const [g2, setG2] = useState('');
  const [penW1, setPenW1] = useState(null); // null = not in penalties
  const [saving, setSaving] = useState(false);
  const [error, setError]   = useState('');

  const n1 = parseInt(g1) || 0;
  const n2 = parseInt(g2) || 0;
  const draw = g1 !== '' && g2 !== '' && n1 === n2;
  const needPen = draw; // in knockout, draws go to penalties

  const handleSubmit = async () => {
    if (g1 === '' || g2 === '') { setError('Insira os gols de ambos os times.'); return; }
    if (needPen && penW1 === null) { setError('Defina o vencedor dos pênaltis.'); return; }
    setSaving(true);
    try {
      await submitResultado(
        fase,
        jogoIdx,
        n1,
        n2,
        needPen,
        needPen ? (penW1 === 1) : false
      );
      onSaved();
      onClose();
    } catch (e) {
      setError(e.message);
    } finally {
      setSaving(false);
    }
  };

  if (!match) return null;

  return (
    <div className="modal-overlay" onClick={e => e.target === e.currentTarget && onClose()}>
      <div className="modal-box">
        {/* Header */}
        <div style={{ marginBottom:'1.25rem' }}>
          <div style={{ fontSize:'0.75rem', color:'var(--green-primary)', textTransform:'uppercase', letterSpacing:2, marginBottom:4 }}>
            {fase.toUpperCase()}  ·  Jogo {jogoIdx + 1}
          </div>
          <div className="modal-title">RESULTADO</div>
        </div>

        {/* Score row */}
        <div className="score-row">
          <div className="score-team">{match.team1 || '—'}</div>
          <input
            id="score-g1"
            type="number" min="0" max="99"
            className="score-input"
            value={g1}
            onChange={e => { setG1(e.target.value); setPenW1(null); setError(''); }}
            placeholder="0"
          />
        </div>
        <div style={{ textAlign:'center', margin:'-0.5rem 0 0.75rem', color:'var(--text-muted)', fontFamily:'var(--font-display)', fontSize:'1.2rem', letterSpacing:2 }}>
          VS
        </div>
        <div className="score-row">
          <div className="score-team">{match.team2 || '—'}</div>
          <input
            id="score-g2"
            type="number" min="0" max="99"
            className="score-input"
            value={g2}
            onChange={e => { setG2(e.target.value); setPenW1(null); setError(''); }}
            placeholder="0"
          />
        </div>

        {/* Penalty section */}
        {needPen && (
          <div style={{
            background:'rgba(255,215,0,0.08)', border:'1px solid rgba(255,215,0,0.25)',
            borderRadius:'var(--radius-md)', padding:'1rem', marginTop:'0.5rem', marginBottom:'0.5rem'
          }}>
            <div style={{ fontSize:'0.82rem', color:'var(--gold)', marginBottom:'0.75rem', textAlign:'center', fontWeight:600 }}>
              ⚡ EMPATE — DECISÃO NOS PÊNALTIS
            </div>
            <div style={{ display:'flex', gap:'0.5rem' }}>
              <button
                className={`btn btn-sm ${penW1===1?'btn-gold':'btn-ghost'}`}
                style={{ flex:1 }}
                onClick={() => { setPenW1(1); setError(''); }}
              >
                {match.team1}
              </button>
              <button
                className={`btn btn-sm ${penW1===2?'btn-gold':'btn-ghost'}`}
                style={{ flex:1 }}
                onClick={() => { setPenW1(2); setError(''); }}
              >
                {match.team2}
              </button>
            </div>
          </div>
        )}

        {error && (
          <div style={{ color:'var(--red)', fontSize:'0.83rem', marginBottom:'0.75rem', textAlign:'center' }}>
            ⚠️ {error}
          </div>
        )}

        {/* Actions */}
        <div style={{ display:'flex', gap:'0.75rem', marginTop:'1rem' }}>
          <button className="btn btn-ghost" style={{ flex:0 }} onClick={onClose}>
            Cancelar
          </button>
          <button
            className="btn btn-primary"
            style={{ flex:1 }}
            disabled={saving}
            onClick={handleSubmit}
          >
            {saving ? 'Salvando...' : '✓ Confirmar Resultado'}
          </button>
        </div>
      </div>
    </div>
  );
}
