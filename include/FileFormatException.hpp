#ifndef FILE_FORMAT_EXCEPTION_HPP
#define FILE_FORMAT_EXCEPTION_HPP

#include <string>

#include "GameException.hpp"

/**
 * @brief Informa um erro de leitura ou de formato em um arquivo de dados.
 *
 * As linhas começam em 1. A linha 0 indica um problema do arquivo inteiro,
 * como arquivo vazio, ausência de entrada ou falha ao abrir o arquivo.
 */
class FileFormatException : public GameException {
public:
    /**
     * @brief Cria uma exceção com o local e a descrição do erro.
     * @param fileName Nome do arquivo ou da entrada de dados.
     * @param lineNumber Linha do erro; 0 quando não há uma linha específica.
     * @param message Mensagem que explica o problema.
     */
    FileFormatException(const std::string& fileName, int lineNumber,
                        const std::string& message);

    /**
     * @brief Consulta o nome do arquivo que apresentou o erro.
     * @return Referência constante para o nome do arquivo.
     */
    const std::string& getFileName() const;

    /**
     * @brief Consulta a linha que apresentou o erro.
     * @return Número da linha ou 0 para erros gerais do arquivo.
     */
    int getLineNumber() const;

private:
    std::string fileName; ///< Arquivo ou entrada em que ocorreu o erro.
    int lineNumber; ///< Linha do erro, começando em 1; 0 para erro geral.
};

#endif // FILE_FORMAT_EXCEPTION_HPP
