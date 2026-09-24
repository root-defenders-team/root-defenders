# User Stories — Tower Defense

## 1. Construção de torres

**Como jogador **, quero construir uma torre em uma célula livre do mapa para começar a defender o caminho dos inimigos.

### Critérios de aceitação

- O jogador consegue selecionar uma posição do mapa.
- A torre só pode ser construída em uma célula livre e válida.
- O custo da torre é descontado do ouro do jogador.
- A construção é impedida quando o jogador não possui ouro suficiente.

## 2. Ataque das torres

**Como jogador **, quero que minhas torres ataquem automaticamente os inimigos dentro do alcance para impedir que eles atravessem o caminho.

### Critérios de aceitação

- A torre identifica inimigos dentro do seu alcance.
- A torre seleciona um alvo válido para atacar.
- O ataque respeita a cadência de tiro da torre.
- O inimigo recebe o dano causado pelo ataque.

## 3. Evolução das torres

**Como jogador **, quero evoluir minhas torres para aumentar sua eficiência durante as ondas mais difíceis.

### Critérios de aceitação

- O jogador consegue selecionar uma torre construída.
- A evolução exige uma quantidade definida de ouro.
- O nível da torre é atualizado após a evolução.
- A evolução aumenta pelo menos um atributo da torre, como dano ou alcance.
- A evolução é impedida quando a torre já atingiu o nível máximo.

## 4. Movimento dos inimigos

**Como jogador**, quero que os inimigos sigam o caminho definido no mapa para que eu possa posicionar minhas torres de forma estratégica.

### Critérios de aceitação

- Os inimigos aparecem no ponto inicial da rota.
- Os inimigos seguem os waypoints na ordem correta.
- Cada tipo de inimigo respeita sua própria velocidade.
- Um inimigo é identificado quando completa o caminho.

## 5. Ondas de inimigos

**Como jogador **, quero enfrentar ondas progressivamente mais difíceis para testar minhas estratégias de defesa.

### Critérios de aceitação

- O jogo inicia a primeira onda corretamente.
- Os inimigos previstos para a onda são criados.
- Uma nova onda começa após a conclusão da onda anterior.
- A dificuldade aumenta progressivamente entre as ondas.
- O jogo informa quando todas as ondas forem concluídas.

## 6. Pontuação máxima

**Como jogador **, quero salvar e consultar minha pontuação máxima para acompanhar meu melhor desempenho.

### Critérios de aceitação

- A pontuação máxima é carregada ao iniciar o jogo.
- A pontuação é atualizada conforme o jogador derrota inimigos ou conclui ondas.
- A nova pontuação só substitui o recorde quando for maior.
- A pontuação máxima é salva em um arquivo de texto ao finalizar a partida.
- O recorde permanece disponível quando o jogo é executado novamente.
