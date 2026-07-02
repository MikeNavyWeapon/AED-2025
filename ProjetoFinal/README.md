# Grafo de Conhecimento para Cyber Threat Intelligence

Projeto de Algoritmos e Estruturas de Dados (EN-AEL). Aplicação em C++ que
representa conhecimento sobre ameaças cibernéticas através de um grafo dirigido.

Estado atual: **Nível 5 — Interface gráfica demonstrativa** (projeto completo).

## Pré-requisitos

- Compilador C++20 (MSYS2 **g++ 15**, já instalado em `C:\msys64\mingw64\bin`)
- **CMake** ≥ 3.16
- **SQLite3** — já incluído no MSYS2 (`libsqlite3.a` + `sqlite3.h`); o CMake
  liga-o automaticamente, não é preciso instalar nada
- VS Code com as extensões *C/C++* e *CMake Tools* (recomendado)

## Compilar e correr (linha de comandos)

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\ctikg.exe          # demonstração por consola
ctest --test-dir build --output-on-failure   # testes
```

## Compilar e correr (VS Code)

As tarefas já estão definidas em `.vscode/tasks.json`:

- **Ctrl+Shift+B** → *CMake: compilar*
- *Terminal → Run Task… → Correr aplicação*
- *Terminal → Run Task… → Correr testes*
- **F5** → depurar com gdb (ver `.vscode/launch.json`)

## Estrutura do projeto

```
ProjetoFinal/
├── CMakeLists.txt          # build (biblioteca + app + testes)
├── README.md
├── .gitignore
├── .vscode/                # tarefas, debug e IntelliSense
├── include/ctikg/          # cabeçalhos públicos da biblioteca
│   ├── Node.hpp            # nó (entidade)
│   ├── Relationship.hpp    # relação dirigida
│   ├── Schema.hpp          # tipos e matriz de relações válidas
│   ├── Graph.hpp           # grafo + listas de adjacência
│   ├── SyntheticData.hpp   # gerador de dados sintéticos
│   ├── Database.hpp        # camada de persistência (SQLite)
│   ├── Analytics.hpp       # algoritmos de análise
│   ├── Json.hpp            # JSON mínimo próprio
│   ├── GraphService.hpp    # camada de serviço (acesso local)
│   └── ApiRoutes.hpp       # rotas da API REST
├── src/                     # implementações
│   ├── Schema.cpp · Graph.cpp · SyntheticData.cpp
│   ├── Database.cpp        # gravar/carregar grafo + pesquisas na BD
│   ├── Analytics.cpp       # BFS, DFS, caminho mais curto, PageRank, resolução
│   ├── Json.cpp           # serializar/parsear JSON
│   ├── GraphService.cpp   # fachada com validação + fila de eventos
│   ├── ApiRoutes.cpp      # endpoints REST
│   ├── api_main.cpp       # executável do servidor da API
│   └── main.cpp           # demonstração por consola
├── third_party/
│   └── httplib.h          # cpp-httplib (servidor HTTP, header único)
├── web/                    # interface gráfica (Nível 5)
│   ├── index.html         # visualização do grafo (HTML/CSS/JS)
│   └── vendor/
│       └── vis-network.min.js  # biblioteca de visualização (offline)
├── db/
│   └── schema.sql          # script de criação da base de dados
├── tests/                  # 6 conjuntos de testes
│   ├── test_graph.cpp · test_persistence.cpp · test_algorithms.cpp
│   ├── test_json.cpp       # testes do módulo JSON
│   ├── test_service.cpp    # testes do serviço (Nível 4)
│   └── test_api.cpp        # testes de integração da API (Nível 4)
└── docs/
    ├── relatorio.md          # relatório técnico (por nível) + rastreabilidade
    ├── api.md                # documentação dos endpoints
    ├── guiao_apresentacao.md # guião para a defesa/apresentação
    └── objeto_estudo.md      # guia completo: tudo o que o projeto contém
```

## API REST + Interface gráfica (Níveis 4 e 5)

```powershell
.\build\ctikg_api_server.exe          # http://127.0.0.1:8080
```

- **Interface gráfica**: abre **http://127.0.0.1:8080/** no browser. O mesmo
  servidor serve a API *e* a página web (pasta `web/`, sem depender de CDN).
- **API**: `curl http://127.0.0.1:8080/stats` — endpoints e exemplos em
  [`docs/api.md`](docs/api.md).

No VS Code há a tarefa *Run Task… → Arrancar API*. Depois é só abrir o endereço
no browser e carregar em **▶ Demonstração**.

A interface é uma aplicação com **abas**, onde se fazem **todas as operações**:

- **Grafo** — visualização com cores por tipo e tamanho por PageRank; filtros por
  tipo de entidade e de relação; procura de nós.
- **Detalhes** — detalhes e vizinhos do nó selecionado; apagar nó.
- **Rankings** — tabelas de PageRank e de centralidade por grau.
- **Criar** — formulários para criar nós e relações (com validação e confiança).
- **Consultar** — caminho mais curto (BFS), travessias BFS/DFS, listas de nós e
  relações.
- **Dados** — estatísticas, persistência (guardar/carregar SQLite, repor) e
  registo de eventos.

## Conceitos

- **Nó**: uma entidade (`threat-actor`, `malware`, `indicator`,
  `vulnerability`, `campaign`, `identity`, `attack-pattern`).
- **Relação**: ligação dirigida `origem --tipo--> destino` (ex.: `uses`).
- **Schema**: define os tipos válidos e a *matriz de relações permitidas*.
- **Grafo**: guarda nós (`unordered_map`) e relações, com **listas de
  adjacência** de saída e entrada para navegação eficiente.

## Resolução de problemas

- **`cc1plus: out of memory` ou `ld returned 1`** ao compilar: é uma
  instabilidade conhecida do MinGW/MSYS2 no Windows (fragmentação de heap), não
  um erro do código. Solução: voltar a compilar (**Ctrl+Shift+B**). Os alvos que
  usam o `cpp-httplib` já compilam com `-g0` para minimizar o problema.
- **A API mostra dados antigos**: pode ter ficado um `ctikg_api_server` de uma
  execução anterior a ocupar a porta. Fecha-o (`taskkill /IM ctikg_api_server.exe /F`)
  e arranca de novo.

## Roadmap dos níveis

- [x] **Nível 1** — grafo em memória, validação, dados sintéticos, consultas, testes
- [x] **Nível 2** — persistência em SQLite (gravar/carregar, pesquisas, testes)
- [x] **Nível 3** — algoritmos (BFS, DFS, caminho mais curto, PageRank, resolução de entidades)
- [x] **Nível 4** — camada de serviço + API REST (22 endpoints: CRUD completo, algoritmos, persistência), validação, testes de integração
- [x] **Nível 5** — interface web com abas, integrada com a API (todas as operações: criar, consultar, rankings, persistência, demo)
