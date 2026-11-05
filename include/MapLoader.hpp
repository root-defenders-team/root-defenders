#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include <istream>
#include <string>

#include "FileFormatException.hpp"
#include "Map.hpp"

/**
 * @brief Lê um arquivo de texto e cria o mapa da fase.
 *
 * O arquivo usa '.' para área construível, '#' para caminho, 'X' para
 * obstáculo, 'E' para a entrada dos inimigos e '@' para o destino final.
 * O caminho deve ser único, contínuo e sem bifurcações. Os erros informam
 * a origem e a linha do problema por FileFormatException.
 */
class MapLoader {
public:
    /**
     * @brief Abre um arquivo de mapa e o converte em Map.
     * @param filePath Caminho do arquivo de mapa.
     * @return Mapa criado a partir do arquivo.
     * @throws FileFormatException se o arquivo não puder ser lido ou for inválido.
     */
    static Map load(const std::string& filePath);

    /**
     * @brief Converte o conteúdo de uma entrada em um Map.
     * @param input Texto do mapa já aberto.
     * @param sourceName Nome usado nas mensagens de erro.
     * @return Mapa criado a partir da entrada.
     * @throws FileFormatException se a leitura falhar ou o mapa for inválido.
     */
    static Map parse(std::istream& input, const std::string& sourceName);
};

#endif // MAP_LOADER_HPP
