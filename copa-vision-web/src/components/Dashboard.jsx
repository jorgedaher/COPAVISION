// Dashboard.jsx — Menu principal com hero animado e cards de navegação

const CARDS = [
  {
    id: 'simulation',
    icon: '⚽',
    title: 'Simulação de Copa',
    desc: 'Fase de grupos, mata-mata, pênaltis e premiações.',
    color: '#00A550',
  },
  {
    id: 'players',
    icon: '👥',
    title: 'Seleções & Jogadores',
    desc: 'Busque jogadores por nome ou explore o elenco de cada país.',
    color: '#4C8EFF',
  },
  {
    id: 'history',
    icon: '📊',
    title: 'Dados Históricos',
    desc: 'Títulos, participações e artilheiros por seleção.',
    color: '#FFD700',
  },
  {
    id: 'curiosities',
    icon: '💡',
    title: 'Curiosidades',
    desc: 'Fatos históricos e recordes memoráveis da Copa do Mundo.',
    color: '#FF6B8A',
  },
];

export default function Dashboard({ onNavigate }) {
  return (
    <div className="animate-in">
      {/* ── Hero ── */}
      <div className="dashboard-hero">
        <div className="hero-year">2026</div>
        <div className="hero-subtitle">COPA VISION</div>
        <p className="hero-desc">
          Simule o torneio completo da Copa do Mundo 2026 com os 48 países,
          fase de grupos, mata-mata oficial e premiações.
        </p>
        <div className="hero-status">
          <span className="status-dot" />
          48 Seleções · 12 Grupos · Mata-Mata Oficial
        </div>

        {/* ── Cards ── */}
        <div className="dash-grid">
          {CARDS.map(c => (
            <div
              key={c.id}
              className="dash-card"
              style={{ '--card-color': c.color }}
              onClick={() => onNavigate(c.id)}
            >
              <span className="dash-card-icon">{c.icon}</span>
              <div className="dash-card-title">{c.title}</div>
              <div className="dash-card-desc">{c.desc}</div>
              <span className="dash-card-arrow">→</span>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}
