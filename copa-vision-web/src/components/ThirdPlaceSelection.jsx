// ThirdPlaceSelection.jsx — Seleção dos 8 melhores terceiros colocados
import { useState, useEffect } from 'react';
import { getTerceiros, confirmarTerceiros } from '../api/copaApi.js';

export default function ThirdPlaceSelection({ onBack, onConfirm }) {
  const [terceiros, setTerceiros] = useState([]);
  const [selected, setSelected]  = useState(new Set());
  const [loading, setLoading]    = useState(true);
  const [saving, setSaving]      = useState(false);
  const [error, setError]        = useState('');

  useEffect(() => {
    getTerceiros()
      .then(setTerceiros)
      .catch(e => setError(e.message))
      .finally(() => setLoading(false));
  }, []);

  const toggle = (grupo) => {
    setSelected(prev => {
      const next = new Set(prev);
      if (next.has(grupo)) next.delete(grupo);
      else if (next.size < 8) next.add(grupo);
      return next;
    });
    setError('');
  };

  const handleConfirm = async () => {
    if (selected.size !== 8) {
      setError('Selecione exatamente 8 terceiros colocados.');
      return;
    }
    setSaving(true);
    try {
      await confirmarTerceiros([...selected]);
      onConfirm();
    } catch (e) {
      setError(e.message);
    } finally {
      setSaving(false);
    }
  };

  if (loading) return (
    <div className="loading-center"><div className="spinner" /></div>
  );

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">MELHORES TERCEIROS</div>
          <div className="section-subtitle">
            Selecione os 8 melhores 3ºs colocados para o mata-mata oficial — {selected.size}/8 selecionados
          </div>
        </div>
        <button className="btn btn-ghost btn-sm" onClick={onBack}>← Voltar</button>
      </div>

      <div style={{
        background:'rgba(255,215,0,0.05)', border:'1px solid rgba(255,215,0,0.2)',
        borderRadius:'var(--radius-md)', padding:'0.75rem 1.25rem',
        fontSize:'0.85rem', color:'var(--gold)', marginBottom:'1.5rem'
      }}>
        ⚠️ A combinação escolhida deve ser compatível com o chaveamento oficial da Copa 2026.
        Caso inválida, o servidor retornará um erro.
      </div>

      <div className="grid-3" style={{ marginBottom:'1.5rem' }}>
        {terceiros.map(t => {
          const sel = selected.has(t.grupo);
          return (
            <div
              key={t.grupo}
              onClick={() => toggle(t.grupo)}
              style={{
                background: sel ? 'rgba(0,165,80,0.12)' : 'var(--bg-card)',
                border: `1px solid ${sel ? 'var(--green-primary)' : 'var(--border)'}`,
                borderRadius:'var(--radius-lg)', padding:'1.1rem 1.25rem',
                cursor:'pointer', transition:'all var(--transition)',
                display:'flex', alignItems:'center', gap:'0.75rem',
              }}
            >
              <div style={{
                width:24, height:24, borderRadius:'50%',
                border:`2px solid ${sel ? 'var(--green-primary)' : 'var(--border)'}`,
                background: sel ? 'var(--green-primary)' : 'transparent',
                display:'flex', alignItems:'center', justifyContent:'center',
                fontSize:'0.75rem', flexShrink:0,
                transition:'all 0.15s'
              }}>
                {sel ? '✓' : ''}
              </div>
              <div>
                <div style={{ fontSize:'0.75rem', color:'var(--text-muted)', marginBottom:2 }}>
                  Grupo {t.grupo} — 3º lugar
                </div>
                <div style={{ fontWeight:600, fontSize:'0.92rem' }}>{t.nome}</div>
              </div>
            </div>
          );
        })}
      </div>

      {error && (
        <div style={{
          background:'rgba(232,23,75,0.1)', border:'1px solid rgba(232,23,75,0.3)',
          borderRadius:'var(--radius-md)', padding:'0.75rem 1.25rem',
          color:'var(--red)', fontSize:'0.88rem', marginBottom:'1rem'
        }}>⚠️ {error}</div>
      )}

      <button
        className="btn btn-gold btn-lg"
        onClick={handleConfirm}
        disabled={selected.size !== 8 || saving}
      >
        {saving ? 'Confirmando...' : `🏆 Confirmar ${selected.size}/8 Terceiros → Iniciar Mata-Mata`}
      </button>
    </div>
  );
}
