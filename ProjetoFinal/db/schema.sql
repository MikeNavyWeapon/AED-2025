-- Script de criação da base de dados do grafo de conhecimento (Nível 2).
-- Pode ser executado manualmente:  sqlite3 grafo.db < db/schema.sql
-- (a aplicação também cria estas tabelas automaticamente em Database::initSchema)

PRAGMA foreign_keys = ON;

-- Nós do grafo (entidades de CTI).
CREATE TABLE IF NOT EXISTS nodes (
    id    TEXT PRIMARY KEY,   -- identificador único (ex.: threat-actor--apt29)
    type  TEXT NOT NULL,      -- tipo de entidade (ex.: threat-actor)
    name  TEXT NOT NULL       -- nome legível
);

-- Atributos livres de cada nó (relação 1-para-N com nodes).
-- Permite preservar o mapa de "properties" do Node sem perder informação.
CREATE TABLE IF NOT EXISTS node_properties (
    node_id TEXT NOT NULL,
    key     TEXT NOT NULL,
    value   TEXT,
    PRIMARY KEY (node_id, key),
    FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE
);

-- Relações dirigidas entre nós.
CREATE TABLE IF NOT EXISTS relationships (
    id         TEXT PRIMARY KEY,
    source     TEXT NOT NULL,    -- id do nó de origem
    target     TEXT NOT NULL,    -- id do nó de destino
    type       TEXT NOT NULL,    -- tipo de relação (ex.: uses)
    confidence INTEGER NOT NULL DEFAULT 50,  -- grau de confiança (0..100)
    FOREIGN KEY (source) REFERENCES nodes(id) ON DELETE CASCADE,
    FOREIGN KEY (target) REFERENCES nodes(id) ON DELETE CASCADE
);

-- Índices para acelerar as pesquisas pedidas no enunciado.
CREATE INDEX IF NOT EXISTS idx_nodes_type    ON nodes(type);
CREATE INDEX IF NOT EXISTS idx_rel_source    ON relationships(source);
CREATE INDEX IF NOT EXISTS idx_rel_target    ON relationships(target);
CREATE INDEX IF NOT EXISTS idx_rel_type      ON relationships(type);
