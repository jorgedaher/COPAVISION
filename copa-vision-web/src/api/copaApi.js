// src/api/copaApi.js — Todas as chamadas ao servidor C++ em um só lugar

const BASE = 'http://localhost:8080/api';

const req = async (path, opts = {}) => {
  const res = await fetch(BASE + path, {
    headers: { 'Content-Type': 'application/json' },
    ...opts,
  });
  if (!res.ok) {
    const err = await res.json().catch(() => ({ error: res.statusText }));
    throw new Error(err.error || 'Erro na requisição');
  }
  return res.json();
};

// ── Status ────────────────────────────────────────────────────
export const getStatus      = ()       => req('/status');

// ── Seleções ──────────────────────────────────────────────────
export const getSelecoes    = ()       => req('/selecoes');
export const getJogadores   = (pais)   => req(`/selecoes/${encodeURIComponent(pais)}/jogadores`);

// ── Grupos ────────────────────────────────────────────────────
export const getGrupos      = ()       => req('/grupos');
export const classificarGrupo = (id, ordem) =>
  req(`/grupos/${encodeURIComponent(id)}/classificar`, { method: 'POST', body: JSON.stringify({ ordem }) });
export const simularGrupos  = ()       => req('/grupos/simular', { method: 'POST' });

// ── Terceiros ─────────────────────────────────────────────────
export const getTerceiros   = ()       => req('/terceiros');
export const confirmarTerceiros = (grupos) =>
  req('/terceiros', { method: 'POST', body: JSON.stringify({ grupos }) });

// ── Mata-mata ─────────────────────────────────────────────────
export const getMataMata    = ()       => req('/matamata');
export const submitResultado = (fase, jogo, gols1, gols2, penalties, penWinner1) =>
  req('/matamata/resultado', {
    method: 'POST',
    body: JSON.stringify({ fase, jogo, gols1, gols2, penalties, penWinner1 }),
  });

// ── Histórico ─────────────────────────────────────────────────
export const getHistorico   = ()       => req('/historico');
export const getCuriosidades = ()      => req('/curiosidades');

// ── Busca de jogadores ────────────────────────────────────────
export const buscarJogadores = (q)     => req(`/jogadores/busca?q=${encodeURIComponent(q)}`);

// ── Premiação ─────────────────────────────────────────────────
export const getPremiacao   = ()       => req('/premiacao');
export const salvarPremiacao = (data)  =>
  req('/premiacao', { method: 'POST', body: JSON.stringify(data) });

// ── Reset ─────────────────────────────────────────────────────
export const resetTorneio   = ()       => req('/reset', { method: 'POST' });
