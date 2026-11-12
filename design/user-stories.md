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

---

## 7. Remoção de defesas e reembolso de CPU (`rm`)

**Como administrador**, quero desinstalar uma defesa posicionada em `(x, y)` para
liberar a célula e recuperar metade da CPU investida ao adaptar minha defesa.

### Critérios de aceitação

- O jogador remove uma defesa instalada usando o comando `rm <x> <y>`.
- O sistema reembolsa **50% de toda a CPU investida** na defesa (custo inicial
  somado aos custos das evoluções já realizadas, em divisão inteira).
- A célula antes ocupada volta a ficar livre (`.`) no mapa, permitindo instalar
  uma nova defesa na mesma posição.
- O Shell confirma a remoção informando a célula, a CPU devolvida e o saldo
  atualizado de ciclos de CPU.
- Tentar remover em célula sem defesa ou fora dos limites exibe
  `erro: não há defesa em (x, y)` sem alterar o mapa nem a CPU.

---

## 8. Prioridade de seleção de alvo (`target`)

**Como administrador**, quero configurar o critério de mira de cada defesa para
direcionar seus ataques aos malwares mais críticos em cada onda.

### Critérios de aceitação

- O jogador altera a prioridade de uma defesa instalada com o comando
  `target <x> <y> <prioridade>`.
- São aceitas quatro prioridades:
  - `first`: ataca o malware mais avançado no caminho (padrão inicial de toda
    nova defesa).
  - `strong`: ataca o malware com maior vida atual dentro do alcance.
  - `weak`: ataca o malware com menor vida atual dentro do alcance.
  - `close`: ataca o malware mais próximo da defesa em distância euclidiana.
- Prioridades inválidas disparam `InvalidCommandException`, exibindo mensagem de
  erro com as opções válidas e preservando a prioridade anterior da defesa.
- Informar coordenadas sem defesa instalada exibe `erro: não há defesa em (x, y)`.

---

## 9. Manual técnico e listagem de defesas (`man` e `ls`)

**Como administrador**, quero consultar os atributos das defesas e malwares no
próprio jogo e listar minhas defesas ativas com custos de evolução e reembolso
para tomar decisões sem sair do terminal.

### Critérios de aceitação

- O comando `man <nome>` exibe descrição resumida e ficha técnica de qualquer
  defesa (`firewall`, `antivirus`, `scanner`, aceitando inicial maiúscula) com
  custo, alcance, dano e intervalo, ou de qualquer malware (`virus`, `worm`,
  `rootkit`) com vida, velocidade, recompensa em CPU e dano ao `/root`.
- Consultar um nome desconhecido no `man` exibe `erro: não há manual para <nome>`.
- O comando `ls` lista todas as defesas instaladas mostrando coordenadas
  `(x, y)`, símbolo (`F`, `A`, `S`), nível, alcance, dano, intervalo,
  prioridade de alvo, custo da próxima evolução (`<valor> CPU` ou `MAX` no
  nível 3) e valor de reembolso em CPU.
- Quando nenhuma defesa estiver instalada, `ls` informa
  `Nenhuma defesa instalada.`.

---

## 10. Seleção de nível por menu inicial ou CLI

**Como administrador**, quero escolher entre os 5 mapas de dificuldade crescente
no menu inicial ou informar um mapa diretamente pela linha de comando para
variar o desafio da partida.

### Critérios de aceitação

- Ao executar o jogo sem argumentos (`./build/bin/root_defense` ou `make run`),
  um menu interativo exibe os 5 níveis de dificuldade (`level1.txt` Muito fácil
  até `level5.txt` Muito difícil).
- Pressionar Enter sem digitar número, encerrar a entrada ou informar opção fora
  de `1` a `5` seleciona o nível padrão `assets/maps/level1.txt`.
- Ao passar um caminho de mapa como argumento na CLI
  (`./build/bin/root_defense <arquivo_do_mapa>`), o jogo pula o menu e carrega
  diretamente o arquivo informado com as 8 ondas de `assets/waves.txt`.
- Informar argumentos em excesso na CLI exibe a mensagem de uso em `std::cerr`
  e encerra o programa com código `1`.

---

## 11. Interface colorida ANSI e comandos informativos (`status`, `map` e `help`)

**Como administrador**, quero visualizar o mapa com coordenadas e cores ANSI e
consultar o estado da partida ou a lista de comandos a qualquer momento durante
a preparação.

### Critérios de aceitação

- O `TerminalRenderer` desenha o cabeçalho (`ROOT DEFENSE`, CPU, Integridade com
  cor verde `>= 70`, amarelo `>= 35` e vermelho `< 35`, Onda e Pontos), as
  coordenadas `(x, y)` nas bordas e a legenda completa dos símbolos (`E`, `@`,
  `#`, `X`, `.`, `*`, `F`, `A`, `S`, `v`, `w`, `r`).
- Ao abrir o `Shell`, o quadro inicial e instruções rápidas de uso são exibidos
  antes do prompt verde `root@server:~# `.
- Os comandos `status` e `map` redesenham o quadro completo (`drawFrame`) sem
  avançar ondas nem alterar o estado do jogo.
- O comando `help` lista todos os comandos do Shell com sintaxe e exemplos;
  `exit` ou fim de entrada (`Ctrl+D`) encerram o loop de forma limpa.

---

## 12. Programação defensiva e tratamento de erros (`erro: <motivo>` e linha)

**Como administrador**, quero que comandos incorretos e arquivos de dados
corrompidos sejam tratados com mensagens claras de erro sem causar encerramento
inesperado do jogo.

### Critérios de aceitação

- Linhas vazias ou com apenas espaços no `Shell` são ignoradas silenciosamente.
- Comandos desconhecidos, quantidade incorreta de argumentos, coordenadas não
  inteiras ou prioridades inválidas lançam `InvalidCommandException` (subclasse
  de `GameException`).
- O `Shell` captura `GameException` e erros de validação sem encerrar a
  partida, exibindo `erro: <motivo>` e mantendo o jogo pronto para o próximo
  comando.
- Arquivos de mapa (`MapLoader`) ou de ondas (`WaveLoader`) ausentes ou
  corrompidos (linha vazia, largura irregular, caractere desconhecido, entrada/
  destino ausente ou duplicado, caminho interrompido/bifurcado/desconectado ou
  ondas fora de ordem) lançam `FileFormatException` informando o nome do arquivo
  e o número exato da linha (`<arquivo>, linha <n>: <motivo>`), exibida em
  `std::cerr` pelo `main()` com retorno `1`.
