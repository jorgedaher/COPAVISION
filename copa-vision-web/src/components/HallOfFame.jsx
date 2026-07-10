// HallOfFame.jsx — Quadro de Honra com confetti, troféu e escolha das premiações
import { useState, useEffect } from 'react';
import { getPremiacao, salvarPremiacao, buscarJogadores } from '../api/copaApi.js';

// ── Confetti Component ────────────────────────────────────────
const COLORS = ['#FFD700','#00A550','#E8174B','#fff','#4C8EFF','#FF6B8A'];

function Confetti() {
  const pieces = Array.from({ length: 60 }, (_, i) => ({
    id: i,
    color: COLORS[i % COLORS.length],
    left: `${Math.random()*100}%`,
    delay: `${Math.random()*3}s`,
    duration: `${2.5 + Math.random()*2}s`,
    size: `${6 + Math.random()*8}px`,
    rotate: Math.random() > 0.5 ? 'rotate' : 'skewX',
  }));

  return (
    <div className="confetti-container">
      {pieces.map(p => (
        <div
          key={p.id}
          className="confetti-piece"
          style={{
            left: p.left,
            width: p.size,
            height: p.size,
            background: p.color,
            animationDuration: p.duration,
            animationDelay: p.delay,
          }}
        />
      ))}
    </div>
  );
}

// ── Award Picker ──────────────────────────────────────────────
function AwardPicker({ icon, title, value, onChange }) {
  const [q, setQ]       = useState('');
  const [opts, setOpts] = useState([]);
  const [open, setOpen] = useState(false);

  useEffect(() => {
    if (q.trim().length < 2) { setOpts([]); return; }
    const t = setTimeout(async () => {
      try {
        const res = await buscarJogadores(q.trim());
        setOpts(res.slice(0, 10));
      } catch { setOpts([]); }
    }, 300);
    return () => clearTimeout(t);
  }, [q]);

  const select = (j) => {
    onChange(`${j.nome} (${j.selecao})`);
    setQ(''); setOpts([]); setOpen(false);
  };

  return (
    <div className="award-card">
      <div className="award-icon">{icon}</div>
      <div className="award-title">{title}</div>
      {value ? (
        <div style={{ display:'flex', alignItems:'center', gap:8 }}>
          <span style={{ fontWeight:700, flex:1 }}>{value}</span>
          <button className="btn btn-ghost btn-sm" onClick={() => onChange('')}>✕</button>
        </div>
      ) : (
        <div style={{ position:'relative' }}>
          <input
            type="text"
            className="input"
            placeholder="Buscar jogador..."
            value={q}
            onChange={e => { setQ(e.target.value); setOpen(true); }}
            onFocus={() => setOpen(true)}
          />
          {open && opts.length > 0 && (
            <div style={{
              position:'absolute', top:'calc(100% + 4px)', left:0, right:0, zIndex:10,
              background:'#1a2535', border:'1px solid var(--border)',
              borderRadius:'var(--radius-md)', overflow:'hidden',
              boxShadow:'0 8px 24px rgba(0,0,0,0.4)',
            }}>
              {opts.map((j, i) => (
                <div
                  key={i}
                  onClick={() => select(j)}
                  style={{
                    padding:'8px 12px', cursor:'pointer', fontSize:'0.85rem',
                    borderBottom:'1px solid var(--border)',
                    transition:'background 0.1s',
                  }}
                  onMouseEnter={e => e.currentTarget.style.background='rgba(0,165,80,0.1)'}
                  onMouseLeave={e => e.currentTarget.style.background=''}
                >
                  <strong>{j.nome}</strong>
                  <span style={{ color:'var(--text-muted)', marginLeft:6 }}>
                    {j.posicao} · {j.selecao}
                  </span>
                </div>
              ))}
            </div>
          )}
        </div>
      )}
    </div>
  );
}

// ── Main ──────────────────────────────────────────────────────
export default function HallOfFame() {
  const [premiacao, setPremiacao] = useState(null);
  const [loading, setLoading]     = useState(true);
  const [saving, setSaving]       = useState(false);
  const [saved, setSaved]         = useState(false);

  const [melhorJogador, setMelhorJogador]   = useState('');
  const [revelacao, setRevelacao]           = useState('');
  const [melhorGoleiro, setMelhorGoleiro]   = useState('');

  useEffect(() => {
    getPremiacao()
      .then(d => {
        setPremiacao(d);
        setMelhorJogador(d.melhorJogador || '');
        setRevelacao(d.revelacao || '');
        setMelhorGoleiro(d.melhorGoleiro || '');
      })
      .catch(() => {})
      .finally(() => setLoading(false));
  }, []);

  const handleSave = async () => {
    setSaving(true);
    try {
      await salvarPremiacao({ melhorJogador, revelacao, melhorGoleiro });
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    } catch(e) { alert(e.message); }
    setSaving(false);
  };

  if (loading) return <div className="loading-center"><div className="spinner" /></div>;

  const champion = premiacao?.campeao;
  const terceiro = premiacao?.terceiro;

  return (
    <div className="animate-in">
      {champion && <Confetti />}

      <div className="section-header">
        <div>
          <div className="section-title">QUADRO DE HONRA</div>
          <div className="section-subtitle">Premiações e homenagens finais da Copa Vision 2026</div>
        </div>
      </div>

      {/* ── Champion ── */}
      {champion ? (
        <div className="hall-champion">
          <span className="trophy-emoji">🏆</span>
          <div className="champion-label">CAMPEÃO DA COPA VISION 2026</div>
          <div className="champion-name">{champion}</div>
          {terceiro && (
            <div style={{ marginTop:'0.75rem', color:'var(--text-muted)', fontSize:'0.88rem' }}>
              🥉 3º Lugar: <strong style={{ color:'var(--text-primary)' }}>{terceiro}</strong>
            </div>
          )}
        </div>
      ) : (
        <div style={{
          background:'rgba(255,215,0,0.05)', border:'1px solid rgba(255,215,0,0.15)',
          borderRadius:'var(--radius-xl)', padding:'2rem', textAlign:'center', marginBottom:'2rem'
        }}>
          <div style={{ fontSize:'3rem', marginBottom:'0.5rem', opacity:0.4 }}>🏆</div>
          <div style={{ color:'var(--text-muted)' }}>O torneio ainda não foi concluído.</div>
          <div style={{ fontSize:'0.82rem', color:'var(--text-muted)', marginTop:4 }}>
            Complete o mata-mata para revelar o campeão.
          </div>
        </div>
      )}

      {/* ── Individual Awards ── */}
      <h3 style={{ marginBottom:'1rem', color:'var(--text-secondary)' }}>🏅 Premiações Individuais</h3>
      <div className="award-row" style={{ marginBottom:'1.5rem' }}>
        <AwardPicker
          icon="⭐"
          title="MELHOR JOGADOR DA COPA"
          value={melhorJogador}
          onChange={setMelhorJogador}
        />
        <AwardPicker
          icon="🌟"
          title="REVELAÇÃO DA COPA"
          value={revelacao}
          onChange={setRevelacao}
        />
        <AwardPicker
          icon="🧤"
          title="MELHOR GOLEIRO"
          value={melhorGoleiro}
          onChange={setMelhorGoleiro}
        />
      </div>

      <div style={{ display:'flex', gap:'0.75rem', alignItems:'center' }}>
        <button
          className="btn btn-gold btn-lg"
          onClick={handleSave}
          disabled={saving}
        >
          {saving ? 'Salvando...' : '✓ Salvar Premiações'}
        </button>
        {saved && (
          <span style={{ color:'var(--green-light)', fontSize:'0.88rem', animation:'slideUp 0.3s ease' }}>
            ✓ Premiações salvas com sucesso!
          </span>
        )}
      </div>

      {/* ── Summary ── */}
      {(melhorJogador || revelacao || melhorGoleiro || champion) && (
        <div style={{
          marginTop:'2.5rem',
          background:'linear-gradient(135deg,rgba(0,165,80,0.06),rgba(255,215,0,0.04))',
          border:'1px solid var(--border)', borderRadius:'var(--radius-xl)',
          padding:'2rem',
        }}>
          <h3 style={{ marginBottom:'1.25rem', fontFamily:'var(--font-display)', letterSpacing:2, fontSize:'1.4rem' }}>
            📋 RESUMO FINAL
          </h3>
          {[
            { label:'🏆 Campeão',             val: champion },
            { label:'🥉 3º Lugar',            val: terceiro },
            { label:'⭐ Melhor Jogador',      val: melhorJogador },
            { label:'🌟 Revelação',           val: revelacao },
            { label:'🧤 Melhor Goleiro',      val: melhorGoleiro },
          ].map(({ label, val }) => val ? (
            <div key={label} style={{
              display:'flex', gap:'1rem', alignItems:'center',
              padding:'0.6rem 0', borderBottom:'1px solid var(--border)',
            }}>
              <span style={{ fontSize:'0.82rem', color:'var(--text-muted)', minWidth:160 }}>{label}</span>
              <span style={{ fontWeight:600 }}>{val}</span>
            </div>
          ) : null)}
        </div>
      )}
    </div>
  );
}
