// Curiosities.jsx — Carrossel de curiosidades históricas da Copa
import { useState, useEffect, useRef } from 'react';
import { getCuriosidades } from '../api/copaApi.js';

export default function Curiosities() {
  const [curiosidades, setCuriosidades] = useState([]);
  const [current, setCurrent]           = useState(0);
  const [loading, setLoading]           = useState(true);
  const [paused, setPaused]             = useState(false);
  const timerRef = useRef(null);

  useEffect(() => {
    getCuriosidades()
      .then(data => { setCuriosidades(data); setLoading(false); })
      .catch(() => setLoading(false));
  }, []);

  // Auto-advance
  useEffect(() => {
    if (paused || curiosidades.length <= 1) return;
    timerRef.current = setTimeout(() => {
      setCurrent(c => (c + 1) % curiosidades.length);
    }, 6000);
    return () => clearTimeout(timerRef.current);
  }, [current, paused, curiosidades.length]);

  const go = (idx) => {
    setCurrent(idx);
    setPaused(true);
    setTimeout(() => setPaused(false), 8000);
  };
  const prev = () => go((current - 1 + curiosidades.length) % curiosidades.length);
  const next = () => go((current + 1) % curiosidades.length);

  if (loading) return <div className="loading-center"><div className="spinner" /></div>;

  if (curiosidades.length === 0) return (
    <div className="empty-state">
      <div className="empty-icon">💡</div>
      <div className="empty-title">Sem curiosidades carregadas</div>
      <div className="empty-desc">Verifique se dados_historicos_copa.csv contém linhas do tipo "curiosidade".</div>
    </div>
  );

  return (
    <div className="animate-in">
      <div className="section-header">
        <div>
          <div className="section-title">CURIOSIDADES</div>
          <div className="section-subtitle">Fatos históricos e recordes da Copa do Mundo</div>
        </div>
        <span className="badge badge-green">{curiosidades.length} curiosidades</span>
      </div>

      {/* Carousel */}
      <div className="curiosity-carousel">
        <div className="curiosity-track" style={{ transform:`translateX(-${current*100}%)` }}>
          {curiosidades.map((c, i) => (
            <div key={i} className="curiosity-slide">
              <div className="curiosity-card">
                <div className="curiosity-number">{String(i+1).padStart(2,'0')}</div>
                <div style={{ fontSize:'3rem', marginBottom:'1rem' }}>💡</div>
                <p className="curiosity-text">{c}</p>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Navigation */}
      <div className="carousel-nav" style={{ marginTop:'1rem' }}>
        <button className="btn btn-ghost btn-sm" onClick={prev}>← Anterior</button>
        <div style={{ display:'flex', flexDirection:'column', alignItems:'center', gap:8 }}>
          <div className="carousel-dots">
            {curiosidades.map((_, i) => (
              <button
                key={i}
                className={`carousel-dot${i===current?' active':''}`}
                onClick={() => go(i)}
                title={`Curiosidade ${i+1}`}
              />
            ))}
          </div>
          <div style={{ fontSize:'0.75rem', color:'var(--text-muted)' }}>
            {current+1} / {curiosidades.length}
            {!paused && <span style={{ marginLeft:8, color:'var(--green-primary)' }}>▶ Auto</span>}
          </div>
        </div>
        <button className="btn btn-ghost btn-sm" onClick={next}>Próxima →</button>
      </div>

      {/* All facts grid */}
      <div style={{ marginTop:'3rem' }}>
        <h3 style={{ marginBottom:'1rem', color:'var(--text-secondary)' }}>Todas as curiosidades</h3>
        <div style={{ display:'flex', flexDirection:'column', gap:'0.75rem' }}>
          {curiosidades.map((c, i) => (
            <div
              key={i}
              onClick={() => go(i)}
              style={{
                background: i===current ? 'rgba(0,165,80,0.08)' : 'var(--bg-card)',
                border: `1px solid ${i===current ? 'var(--green-primary)' : 'var(--border)'}`,
                borderRadius:'var(--radius-md)', padding:'0.9rem 1.2rem',
                cursor:'pointer', transition:'all var(--transition)',
                display:'flex', gap:'0.75rem', alignItems:'flex-start',
              }}
            >
              <span style={{
                fontFamily:'var(--font-display)', fontSize:'1.4rem',
                color: i===current ? 'var(--green-primary)' : 'var(--text-muted)',
                minWidth:32, transition:'color var(--transition)'
              }}>{i+1}</span>
              <p style={{ fontSize:'0.9rem', lineHeight:1.6, color:'var(--text-primary)', margin:0 }}>{c}</p>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}
