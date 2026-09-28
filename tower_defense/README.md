# Tower Defense

Projeto de jogo Tower Defense desenvolvido em C++.

## Estrutura

- `include/`: arquivos de cabeçalho
- `src/`: código-fonte
- `tests/`: testes automatizados
- `build/`: arquivos compilados
- `assets/`: recursos do jogo
- `design/`: cartões CRC e User Stories do projeto

## Documentação inicial

O projeto está sendo desenvolvido como um jogo Tower Defense. O jogador
posiciona torres defensivas ao redor do caminho, enquanto ondas de inimigos
percorrem uma rota definida no mapa. As torres atacam os inimigos que entram
em seu alcance, e o objetivo é impedir que eles atravessem o caminho.

Neste checkpoint foi definido o contrato inicial da classe `Map`, em
`include/Map.hpp`. A classe representa o grid do cenário, controla células
livres e ocupadas e armazena os waypoints utilizados pela rota dos inimigos.
Ela não herda de `Entity`, pois representa o cenário e não uma entidade móvel
ou renderizável do jogo.

## Classe Map

`Map` fornece operações para consultar os limites do cenário, distinguir
terreno transitável de terreno bloqueado, ocupar e liberar células para
construção de torres e cadastrar os pontos da rota dos inimigos. A informação
de transitabilidade é mantida separada da ocupação por torres: uma célula pode
ser parte do terreno válido e estar temporariamente ocupada por uma torre.
Os métodos estão documentados com Doxygen no arquivo de cabeçalho. A
implementação dos métodos será adicionada em uma etapa posterior, mantendo
neste momento apenas o contrato solicitado para o sistema.
