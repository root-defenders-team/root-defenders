#ifndef WAVE_LOADER_HPP
#define WAVE_LOADER_HPP

#include <istream>
#include <string>
#include <vector>

#include "FileFormatException.hpp"
#include "Wave.hpp"

/**
 * @brief Lê as ondas de uma fase a partir de texto.
 *
 * Cada linha de grupo contém: onda tipo quantidade intervalo. Linhas vazias
 * e linhas iniciadas por // são ignoradas. As ondas começam em 1 e seguem
 * em ordem, sem pular números.
 */
class WaveLoader {
public:
    /**
     * @brief Abre um arquivo e carrega suas ondas.
     * @param filePath Caminho do arquivo de ondas.
     * @return Ondas na ordem do arquivo.
     * @throws FileFormatException se o arquivo não abrir ou for inválido.
     */
    static std::vector<Wave> load(const std::string& filePath);

    /**
     * @brief Converte um fluxo de texto em ondas.
     * @param input Texto já aberto.
     * @param sourceName Nome mostrado em mensagens de erro.
     * @return Ondas na ordem do arquivo.
     * @throws FileFormatException se a leitura ou o formato forem inválidos.
     */
    static std::vector<Wave> parse(std::istream& input, const std::string& sourceName);
};

#endif // WAVE_LOADER_HPP
