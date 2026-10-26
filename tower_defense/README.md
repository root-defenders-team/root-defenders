# Tower Defense

Projeto de um jogo Tower Defense desenvolvido em C++. O jogador deverá
posicionar torres defensivas ao redor de um caminho para impedir que ondas de
inimigos atravessem o mapa. Cada torre poderá possuir características próprias,
como alcance, dano e cadência de ataque, enquanto os inimigos terão diferentes
velocidades, resistências e recompensas.

O objetivo do jogo é utilizar os recursos disponíveis de forma estratégica,
derrotar os inimigos e sobreviver ao maior número possível de ondas.

## Integrantes

- Sergio Amaral de Souza: 2026047310
- Thiago Arthur: 2026046020
- Thiago Calebe: 2026047175
- Lucas Gabriel: 2026047167
- Vitor Paixão: 2026047370

## Compilação e testes

Os testes usam o framework doctest. O cabeçalho oficial do doctest está
versionado em `tests/third_party`, portanto não exige instalação separada. É
necessário apenas um compilador com suporte a C++17 e o MinGW Make.

No Windows com MinGW Make, os comandos são:

```powershell
Set-Location tower_defense
mingw32-make test
```

A classe `Map` já possui sua implementação em `src/Map.cpp`. Seus testes
verificam a criação do mapa, os limites, o terreno, a ocupação de células e os
waypoints.

Para verificar cobertura após a implementação, instale `gcovr` e execute:

```powershell
mingw32-make coverage
```

O alvo de cobertura exige no mínimo 60% de cobertura de linhas dos arquivos
em `src/`, exceto `main.cpp`.
