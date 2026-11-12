# User Stories — ROOT DEFENSE

Especificação de requisitos e critérios de aceitação do jogo **ROOT DEFENSE**
(tower defense de terminal em C++11). O jogador atua como administrador de um
servidor Linux (`root@server:~#`), iniciando com **200 ciclos de CPU** e
**100% de integridade no diretório `/root`**, para conter 8 ondas de malwares
que avançam da porta de entrada (`E`) até o `/root` (`@`).

---

## 1. Instalação de defesas (`install`)

**Como administrador**, quero instalar programas de defesa (`Firewall`,
`Antivírus` ou `Scanner`) em células livres (`.`) do mapa usando ciclos de CPU
para proteger o caminho até o diretório `/root`.

### Critérios de aceitação

- O jogador escolhe o tipo de defesa (`firewall`, `antivirus` ou `scanner`) e as
  coordenadas `(x, y)` pelo comando `install <tipo> <x> <y>`.
- A instalação só é permitida em células livres (`.`) dentro dos limites do
  mapa; células do caminho (`#`, `E`, `@`), obstáculos (`X`) ou já ocupadas são
  recusadas com mensagem de erro específica.
- Os custos iniciais são descontados dos **ciclos de CPU** disponíveis (saldo
  inicial de `200`): `Firewall` (`F`) custa `50`, `Antivírus` (`A`) custa `100`
  e `Scanner` (`S`) custa `120`.
- A instalação é impedida quando não há ciclos de CPU suficientes, informando o
  custo da defesa, o saldo atual e quantos ciclos de CPU faltam.
- Após instalada, a defesa aparece no mapa com seu símbolo (`F`, `A` ou `S`) e
  recebe a prioridade de alvo padrão `first`.

---

## 2. Ataque automático das defesas

**Como administrador**, quero que minhas defesas ataquem automaticamente os
malwares dentro do alcance durante as ondas para impedir que comprometam o
`/root`.

### Critérios de aceitação

- Cada defesa detecta malwares ativos dentro do seu raio de alcance e respeita
  seu intervalo mínimo entre disparos (*cooldown*):
  - **Firewall (`F`)**: alcance `2,5`, dano `6`, intervalo `0,5 s` e projétil
    rápido (`8,0` cél./s).
  - **Antivírus (`A`)**: alcance `4,0`, dano `30`, intervalo `2,0 s` e projétil
    pesado (`6,0` cél./s).
  - **Scanner (`S`)**: alcance `2,0`, dano `8`, intervalo `1,0 s` e dano em área
    diretamente em todos os malwares no alcance (sem projétil).
- O alvo principal é escolhido segundo a prioridade configurada na defesa.
- Projéteis (`*`) perseguem o alvo até causar dano e são desativados com
  segurança caso o malware seja eliminado antes do impacto.
- Cada malware destruído concede sua recompensa em ciclos de CPU e
  `recompensa × 10` pontos no placar da partida.

---

## 3. Evolução das defesas (`upgrade`)

**Como administrador**, quero evoluir minhas defesas instaladas investindo ciclos
de CPU para ampliar sua eficiência contra as ondas mais difíceis.

### Critérios de aceitação

- O jogador seleciona uma defesa instalada pelo comando `upgrade <x> <y>`.
- O custo de evolução do nível `L` para `L + 1` segue a fórmula
  `custo_base × L × 3 / 4` (divisão inteira) e é descontado dos ciclos de CPU
  disponíveis.
- Cada subclasse aprimora seu atributo específico ao evoluir: `Firewall` reduz o
  intervalo de disparo em `0,1 s`, `Antivírus` aumenta o dano em `+15` e
  `Scanner` amplia o alcance em `+0,5` célula.
- O nível da defesa sobe até o máximo permitido (`3`); evoluir acima do nível
  máximo, sem ciclos de CPU suficientes ou em célula vazia exibe mensagem
  `erro:` sem alterar o estado da partida.

---

## 4. Movimento dos malwares e integridade do `/root`

**Como administrador**, quero compreender a rota e as características de cada
malware até o `/root` para posicionar minhas defesas de forma estratégica.

### Critérios de aceitação

- Os malwares surgem na porta de entrada (`E`) e percorrem os *waypoints* do
  caminho (`#`) até o diretório `/root` (`@`), respeitando seus atributos:
  - **Vírus (`v`)**: `30` de vida, velocidade `1,0` cél./s, recompensa `10` CPU
    e dano de `10%` ao `/root`.
  - **Worm (`w`)**: `15` de vida, velocidade `2,0` cél./s, recompensa `8` CPU e
    dano de `5%` ao `/root`.
  - **Rootkit (`r`)**: `120` de vida, velocidade `0,6` cél./s, recompensa `30`
    CPU, dano de `25%` ao `/root` e blindagem que reduz cada golpe em `3`
    (dano mínimo de `1`).
- A integridade inicial do `/root` começa em `100%` e diminui conforme o dano
  de cada malware que alcança `@` (nunca ficando abaixo de `0%`).
- Quando a integridade do `/root` chega a `0%`, ocorre *Kernel Panic* e a
  partida termina em derrota sem conceder recompensa pelos malwares invasores.

---

## 5. Ondas de ataque e condições de vitória (`run`)

**Como administrador**, quero enfrentar ondas progressivamente mais intensas de
malwares para testar minha estratégia até sobreviver ao ataque completo.

### Critérios de aceitação

- Durante a fase de preparação, o comando `run` inicia a próxima onda carregada
  de `assets/waves.txt` (`8` ondas crescentes).
- Os grupos de malwares nascem conforme quantidade e intervalo definidos na
  onda, enquanto a simulação avança em ticks de `0,1 s` redesenhando o mapa.
- Ao concluir a onda `n` com integridade positiva, o jogador recebe bônus de
  `20 + 5 × n` ciclos de CPU e `+100` pontos, retornando à fase de preparação.
- Ao sobreviver à oitava onda com o `/root` íntegro, o jogador recebe bônus
  final de `integridade × 10` pontos e o jogo declara **Vitória**.
- Executar `run` após todas as ondas terem terminado exibe mensagem de erro sem
  alterar a pontuação.

---

## 6. Ranking top 5 persistente (`ranking` / `rank`)

**Como administrador**, quero registrar e consultar as 5 melhores pontuações em
`assets/ranking.txt` para acompanhar meu desempenho entre partidas.

### Critérios de aceitação

- Ao iniciar o jogo, o ranking é carregado de `assets/ranking.txt` no formato
  `<pontos>;<onda>;<nome>`, ordenado da maior para a menor pontuação (máximo de
  5 entradas).
- Linhas corrompidas no arquivo são descartadas sem interromper a execução e o
  Shell informa quantas linhas foram ignoradas antes do primeiro comando.
- Os comandos `ranking` e `rank` exibem as pontuações registradas no formato
  `<posição>. <nome> - <pontos> pts (onda <onda>)`.
- Ao encerrar a partida (vitória ou derrota), se a pontuação obtida se
  qualificar entre as 5 melhores, o jogo solicita o nome do jogador (removendo
  `;`, espaços extras e limitando a `20` caracteres, com padrão `anonimo` se
  vazio) e grava o ranking atualizado em `assets/ranking.txt`.
- O ranking salvo permanece disponível em execuções futuras; eventuais falhas
  ao gravar o arquivo exibem um aviso sem encerrar o jogo abruptamente.
