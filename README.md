# ROOT DEFENSE

**ROOT DEFENSE** é um jogo *tower defense* de terminal desenvolvido em **C++11** para a disciplina de Programação e Desenvolvimento de Software II (PDS 2 — DCC/UFMG).

Um servidor Linux está sob ataque: malwares entram pela porta de rede (`E`) e atravessam o sistema de arquivos tentando alcançar o diretório `/root` (`@`). Como administrador com `sudo`, sua missão é instalar, evoluir e gerenciar programas de defesa ao longo do caminho, pagando com ciclos de CPU.

Cada malware que atinge o `/root` reduz a integridade do sistema. Se a integridade chegar a `0%`, ocorre um *Kernel Panic* e a partida termina em derrota. Se o servidor sobreviver às 8 ondas de ataque, você vence e sua pontuação pode entrar no ranking das 5 melhores partidas.

---

## Sumário

1. [Requisitos e Ferramentas](#requisitos-e-ferramentas)
2. [Compilação, Execução e Testes](#compilação-execução-e-testes)
3. [Como Jogar](#como-jogar)
4. [Defesas e Malwares](#defesas-e-malwares)
5. [Arquitetura e Orientação a Objetos](#arquitetura-e-orientação-a-objetos)
6. [Modelagem: User Stories e Cartões CRC](#modelagem-user-stories-e-cartões-crc)
7. [Programação Defensiva e Exceções](#programação-defensiva-e-exceções)
8. [Estrutura do Repositório e Arquivos de Dados](#estrutura-do-repositório-e-arquivos-de-dados)
9. [Integrantes](#integrantes)

---

## Requisitos e Ferramentas

O jogo utiliza exclusivamente a biblioteca padrão do **C++11** (`-std=c++11 -Wall -Wextra -pedantic`), sem dependências externas para compilar ou rodar.

| Ferramenta | Finalidade | Instalação no Ubuntu / WSL |
|---|---|---|
| `g++` (C++11) | Compilação do projeto e dos testes | `sudo apt install build-essential` |
| `GNU Make` | Automação de build, execução, testes e documentação | Já incluído em `build-essential` |
| `doctest` (2.4.12) | Testes de unidade (já incluído em `tests/third_party/doctest/`) | Nenhuma instalação necessária |
| `gcovr` | Relatório de cobertura de testes (`make coverage`) | `sudo apt install gcovr` |
| `Doxygen` | Geração da documentação HTML (`make docs`) | `sudo apt install doxygen` |

---

## Compilação, Execução e Testes

Todos os comandos devem ser executados na raiz do repositório (no Linux ou no WSL):

```bash
make            # Compila o jogo em build/bin/root_defense
make run        # Compila (se necessário) e inicia o jogo no terminal
make test       # Compila e executa toda a suíte de testes de unidade (doctest)
make coverage   # Executa os testes com cobertura e exige no mínimo 60% de linhas cobertas
make docs       # Gera a documentação Doxygen em build/docs/html
make clean      # Remove os arquivos gerados em build/obj, build/bin e build/docs
```

Sem argumentos (`make run`), o jogo exibe um menu interativo permitindo escolher
entre os 5 níveis de dificuldade (`level1.txt` a `level5.txt`). Também é possível
passar o caminho do mapa diretamente ao iniciar o executável:

```bash
./build/bin/root_defense assets/maps/level2.txt
./build/bin/root_defense assets/maps/level3.txt
./build/bin/root_defense assets/maps/level4.txt
./build/bin/root_defense assets/maps/level5.txt
```

Os cinco mapas usam as mesmas oito ondas de `assets/waves.txt`. O caminho é
relativo à pasta em que o comando é executado. Um mapa inexistente ou inválido
mostra uma mensagem de erro; argumentos em excesso mostram a forma de uso.

Os mapas têm dificuldade crescente: as rotas ficam mais curtas e há menos células
livres para instalar defesas. O mapa 1 mantém seu desenho original.

| Mapa | Dificuldade | Células da rota | Células livres |
|---|---|---|---|
| `level1.txt` | Muito fácil | 37 | 159 |
| `level2.txt` | Fácil | 33 | 147 |
| `level3.txt` | Médio | 28 | 132 |
| `level4.txt` | Difícil | 24 | 97 |
| `level5.txt` | Muito difícil | 20 | 54 |

---

## Como Jogar

Uma partida alterna duas fases:
1. **Fase de Preparação (`root@server:~#`)**: você digita comandos para instalar defesas, escolher prioridades de alvo, evoluir ou remover defesas.
2. **Fase de Onda (`run`)**: a onda é simulada em ticks de `0,1 s`, redesenhando o mapa colorido em ASCII no terminal enquanto os malwares avançam e as defesas atiram.

### Legenda do Mapa

| Símbolo | Significado |
|---|---|
| `E` | Porta de entrada dos malwares (início do caminho) |
| `@` | Diretório `/root` (fim do caminho a ser protegido) |
| `#` | Caminho percorrido pelos malwares |
| `.` | Célula livre (permite instalar uma defesa) |
| `X` | Obstáculo (não é caminho e não aceita defesa) |
| `F`, `A`, `S` | Defesas instaladas (Firewall, Antivírus, Scanner) |
| `v`, `w`, `r` | Malwares ativos (Vírus, Worm, Rootkit) |
| `*` | Projétil em voo |

As coordenadas `(x, y)` aparecem numeradas nas bordas do mapa: `x` é a coluna (no topo) e `y` é a linha (à esquerda), começando em `0`.

### Comandos do Shell

| Comando | Descrição | Exemplo |
|---|---|---|
| `help` | Lista todos os comandos disponíveis com exemplos | `help` |
| `install <tipo> <x> <y>` | Instala uma defesa (`firewall`, `antivirus` ou `scanner`) na célula `(x, y)` | `install firewall 4 3` |
| `upgrade <x> <y>` | Evolui a defesa da célula `(x, y)` (nível máximo: 3) | `upgrade 4 3` |
| `rm <x> <y>` | Remove a defesa da célula `(x, y)` e devolve 50% da CPU investida | `rm 4 3` |
| `target <x> <y> <prioridade>` | Define a prioridade de alvo da defesa (`first`, `strong`, `weak` ou `close`) | `target 4 3 strong` |
| `ls` | Lista todas as defesas instaladas, níveis, atributos, prioridades e custos | `ls` |
| `status` (ou `map`) | Redesenha o mapa e o cabeçalho de status da partida | `status` |
| `man <nome>` | Exibe o manual e os atributos de uma defesa ou malware | `man rootkit` |
| `run` | Inicia a próxima onda de ataque | `run` |
| `ranking` (ou `rank`) | Mostra as 5 melhores pontuações registradas | `ranking` |
| `exit` | Encerra a partida | `exit` |

### Prioridades de Alvo (`target`)
- `first`: ataca o malware mais avançado no caminho (padrão de toda nova defesa).
- `strong`: ataca o malware com mais vida atual.
- `weak`: ataca o malware com menos vida atual.
- `close`: ataca o malware mais próximo da defesa.

### Economia e Pontuação
- **CPU Inicial**: `200`
- **Integridade Inicial do `/root`**: `100%`
- **Malware destruído**: `+ recompensa` de CPU e `+ recompensa × 10` pontos.
- **Onda `n` concluída**: `+ (20 + 5 × n)` de CPU e `+ 100` pontos.
- **Vitória**: bônus de `+ integridade final × 10` pontos.
- **Custo de evolução** (do nível `L` para `L + 1`): `custo_base × L × 3 / 4` (divisão inteira).
- **Reembolso de remoção (`rm`)**: `50%` de toda a CPU investida na defesa (divisão inteira).

---

## Defesas e Malwares

### Defesas

| Defesa | Símbolo | Custo | Alcance | Dano | Intervalo | Modo de Ataque | Melhoria por Nível (`upgrade`) |
|---|---|---|---|---|---|---|---|
| **Firewall** | `F` | 50 CPU | 2,5 | 6 | 0,5 s | Dispara projétil rápido (8 cél./s) | Intervalo `-0,1 s` |
| **Antivírus** | `A` | 100 CPU | 4,0 | 30 | 2,0 s | Dispara projétil pesado (6 cél./s) | Dano `+15` |
| **Scanner** | `S` | 120 CPU | 2,0 | 8 | 1,0 s | Dano em área em todos no alcance (sem projétil) | Alcance `+0,5` |

### Malwares

| Malware | Símbolo | Vida | Velocidade | Recompensa | Dano ao `/root` | Comportamento Especial |
|---|---|---|---|---|---|---|
| **Vírus** | `v` | 30 | 1,0 cél./s | 10 CPU | 10% | Equilibrado em vida e velocidade |
| **Worm** | `w` | 15 | 2,0 cél./s | 8 CPU | 5% | Muito rápido e frágil; ataca em grupos |
| **Rootkit** | `r` | 120 | 0,6 cél./s | 30 CPU | 25% | Blindagem: reduz cada golpe recebido em 3 (dano mínimo de 1) |

---

## Arquitetura e Orientação a Objetos

O sistema é dividido em **três camadas independentes**:
1. **Interface no Terminal**: `Shell`, `TerminalRenderer` e `CommandParser` (leem comandos de um `std::istream` e desenham em um `std::ostream`, facilitando os testes automatizados com `std::stringstream`).
2. **Regras de Negócio e Entidades**: `GameManager`, `Map`, `Wave`, `Entity`, `Tower` (e subclasses), `Enemy` (e subclasses), `Projectile`, `TowerFactory` e `EnemyFactory`. Nenhuma classe de regra conhece o terminal ou arquivos.
3. **Persistência em Arquivos**: `MapLoader`, `WaveLoader` e `ScoreBoard`.

### Herança, Polimorfismo e Padrões de Projeto
- **Hierarquia `Entity`**: classe base abstrata com posição, caixa de colisão e métodos virtuais puros `update(float)` e `getSymbol()`.
- **Hierarquia `Enemy`**: herda de `Entity` e é especializada em `VirusEnemy`, `WormEnemy` e `RootkitEnemy`. `RootkitEnemy` sobrescreve polimorficamente `takeDamage(float)` para aplicar sua blindagem.
- **Hierarquia `Tower` (Template Method)**: herda de `Entity` e é especializada em `FirewallTower`, `AntivirusTower` e `ScannerTower`. `Tower::tryAttack` implementa o padrão *Template Method*: verifica o cooldown, seleciona o alvo de acordo com a `TargetPriority` e delega o disparo para o método protegido polimórfico `fire(...)`, enquanto `upgrade()` valida e delega a melhoria específica para `applyUpgrade()`.
- **Simple Factory (`TowerFactory` e `EnemyFactory`)**: encapsulam a instanciação das subclasses concretas a partir de `TowerType`/`EnemyType` ou strings lidas de comandos e arquivos.

---

## Modelagem: User Stories e Cartões CRC

Os artefatos de levantamento de requisitos e projeto orientado a objetos ficam
em `design/`, detalhando todas as funcionalidades entregues em
`design/user-stories.md` e as responsabilidades de cada classe nos cartões de
`design/CRC Maker.pdf`.

### Resumo das 12 User Stories

| ID | História | Escopo Principal |
|---|---|---|
| **US 01** | Instalação de defesas (`install`) | Posicionamento de `Firewall`, `Antivírus` ou `Scanner` em células livres `.` usando ciclos de CPU (`200` iniciais) |
| **US 02** | Ataque automático das defesas | Detecção no alcance, *cooldown*, disparo de projéteis (`*`) ou dano em área (`Scanner`) e ganho de CPU/pontos |
| **US 03** | Evolução das defesas (`upgrade`) | Melhoria até o nível `3` com custo `custo_base × L × 3 / 4` (cadência, dano ou alcance por subclasse) |
| **US 04** | Movimento dos malwares e integridade | Trajeto `E` → `@` de `Vírus`, `Worm` e `Rootkit` (blindagem `-3`) reduzindo a integridade inicial de `100%` do `/root` |
| **US 05** | Ondas de ataque e vitória (`run`) | Execução das 8 ondas de `assets/waves.txt` em ticks de `0,1 s`, bônus por onda/vitória e *Kernel Panic* na derrota |
| **US 06** | Ranking top 5 persistente (`ranking`/`rank`) | Carga/gravação de `assets/ranking.txt` (`<pontos>;<onda>;<nome>`), sanitização de nome e tolerância a linhas corrompidas |
| **US 07** | Remoção e reembolso (`rm`) | Desinstalação de defesa liberando a célula e devolvendo `50%` de toda a CPU investida |
| **US 08** | Prioridade de alvo (`target`) | Seleção dinâmica de mira por defesa entre `first`, `strong`, `weak` e `close` |
| **US 09** | Manual e listagem (`man` e `ls`) | Consulta de atributos de defesas/malwares (`man`) e tabela de defesas com custos de evolução e reembolso (`ls`) |
| **US 10** | Seleção de nível (menu ou CLI) | Menu interativo com 5 mapas de dificuldade crescente (`level1.txt` a `level5.txt`) ou caminho direto na linha de comando |
| **US 11** | Interface ANSI e comandos informativos | Mapa colorido com coordenadas e legenda, redesenho por `status`/`map`, lista de comandos (`help`) e saída (`exit`) |
| **US 12** | Programação defensiva e exceções | Mensagens `erro: <motivo>` no `Shell` (`InvalidCommandException`) e arquivo/linha exata em `FileFormatException` |

### Classes dos Cartões CRC

- **Núcleo de Entidades e Simulação (`design/CRC Maker.pdf`)**:
  - `Entity` *(abstrata; subclasses `Tower`, `Enemy`, `Projectile`)*: posição `(x, y)`, dimensões, colisão AABB (`Rect`), estado ativo e contrato `update(float)` / `getSymbol()`. Colaboradores: `Map`, `GameManager`.
  - `Tower` *(abstrata; subclasses `FirewallTower`, `AntivirusTower`, `ScannerTower`)*: alcance, dano, intervalo, cooldown, nível (máx. `3`), `TargetPriority`, *Template Method* em `tryAttack(...)`/`upgrade()` e valor de reembolso (`50%`). Colaboradores: `Entity`, `Enemy`, `Projectile`, `Map`, `GameManager`.
  - `FirewallTower`, `AntivirusTower`, `ScannerTower`: especializam `fire(...)` (projétil rápido, projétil pesado e dano em área) e `applyUpgrade()` (cadência, dano e alcance). Colaboradores: `Tower`, `Enemy`, `Projectile`.
  - `Enemy` *(abstrata; subclasses `VirusEnemy`, `WormEnemy`, `RootkitEnemy`)*: vida, velocidade, recompensa em CPU, dano ao `/root`, avanço pelos waypoints e `takeDamage(float)`. Colaboradores: `Entity`, `Map`, `Tower`, `Projectile`, `GameManager`.
  - `VirusEnemy`, `WormEnemy`, `RootkitEnemy`: definem balanceamento e símbolos (`v`, `w`, `r`); `RootkitEnemy` sobrescreve `takeDamage(float)` aplicando blindagem. Colaboradores: `Enemy`.
  - `Projectile` *(herda de `Entity`)*: persegue o alvo ativo, aplica dano ao atingir e encerra a execução se o alvo cair antes do impacto. Colaboradores: `Entity`, `Tower`, `Enemy`, `GameManager`.
  - `Map`: grade de células (`Buildable`, `Path`, `Blocked`), ocupação por defesas e sequência de waypoints (`getPath()`). Colaboradores: `Tower`, `Enemy`, `GameManager`, `MapLoader`.
  - `Wave` *(com `SpawnGroup`)*: controla grupos de nascimento, intervalos e emissão de malwares por tick. Colaboradores: `EnemyType`, `GameManager`, `WaveLoader`.
  - `GameManager`: máquina de estados (`Preparing`, `WaveRunning`, `Victory`, `Defeat`), orquestração dos ticks, posse exclusiva das entidades, CPU, integridade do `/root` e pontuação (sem persistência em arquivo). Colaboradores: `Map`, `Wave`, `Tower`, `Enemy`, `Projectile`, `EnemyFactory`.
- **Fábricas (*Simple Factory*)**:
  - `TowerFactory` e `EnemyFactory`: centralizam o parsing de nomes e a criação polimórfica de defesas e malwares em `std::unique_ptr`. Colaboradores: subclasses de `Tower` e `Enemy`, `Shell`, `GameManager`, `WaveLoader`.
- **Interface de Terminal**:
  - `Shell`, `CommandParser` e `TerminalRenderer`: ciclo REPL de comandos, validação sintática contra regras (`Command`) e renderização ANSI de cabeçalho, mapa, defesas e ranking. Colaboradores: `GameManager`, `ScoreBoard`, `TowerFactory`, `EnemyFactory`, `InvalidCommandException`.
- **Persistência e Arquivos**:
  - `MapLoader`, `WaveLoader` e `ScoreBoard`: leitura/validação de mapas e ondas e persistência ordenada do ranking top 5 em `assets/ranking.txt` (`ScoreEntry`). Colaboradores: `Map`, `Wave`, `GameException`, `FileFormatException`.

---

## Programação Defensiva e Exceções

Nenhuma entrada inválida do usuário ou erro em arquivo derruba o programa de forma anormal:
- **`GameException`** (`std::runtime_error`): base das exceções de domínio do jogo.
  - **`InvalidCommandException`**: lançada pelo `CommandParser` quando um comando é desconhecido, tem número incorreto de argumentos ou coordenadas/prioridades inválidas. Capturada pelo `Shell`, que exibe `erro: <motivo>` e mantém a partida rodando.
  - **`FileFormatException`**: lançada por `MapLoader` e `WaveLoader` informando o nome do arquivo e o número exato da linha com erro. Capturada em `main()`.
- **Validações de invariantes**: construtores e setters das classes de domínio validam seus parâmetros e lançam `std::invalid_argument`, `std::out_of_range` ou `std::logic_error`.

---

## Estrutura do Repositório e Arquivos de Dados

```text
2026-2-METATURMA-grupo45/
├── Makefile                # Regras: all, run, test, coverage, docs, clean
├── Doxyfile                # Configuração da documentação Doxygen
├── README.md               # Documentação principal do projeto
├── include/                # Headers (.hpp), uma classe/tipo por arquivo
├── src/                    # Implementações (.cpp) + main.cpp
├── tests/                  # Testes de unidade com doctest
├── assets/
│   ├── maps/
│   │   ├── level1.txt     # Mapa padrão (E, @, #, ., X)
│   │   ├── level2.txt     # Fácil
│   │   ├── level3.txt     # Médio
│   │   ├── level4.txt     # Difícil
│   │   └── level5.txt     # Muito difícil
│   ├── waves.txt           # Definição das 8 ondas (<onda> <tipo> <qtd> <intervalo>)
│   └── ranking.txt         # Top 5 pontuações (<pontos>;<onda>;<nome>)
├── design/                 # User stories e cartões CRC
└── build/                  # Objetos, binários e documentação gerados
```

---

## Integrantes

- **Sergio Amaral de Souza** — 2026047310
- **Thiago Arthur** — 2026046020
- **Thiago Calebe** — 2026047175
- **Lucas Gabriel** — 2026047167
- **Vitor Paixão** — 2026047370
