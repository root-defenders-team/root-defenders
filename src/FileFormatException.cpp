#include "FileFormatException.hpp"

FileFormatException::FileFormatException(const std::string& fileName, int lineNumber,
                                       const std::string& message)
    : GameException(fileName + ", linha " + std::to_string(lineNumber) + ": " + message),
      fileName(fileName), lineNumber(lineNumber) {
}

const std::string& FileFormatException::getFileName() const {
    return fileName;
}

int FileFormatException::getLineNumber() const {
    return lineNumber;
}
