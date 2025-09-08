#ifndef SRC_SCANNER_MD5FILEHASHER_H_
#define SRC_SCANNER_MD5FILEHASHER_H_

#include <optional>
#include <string>
#include <hash-library/md5.h>

///
/// \brief Класс-обёртка для алгоритма хэширования MD5
///
class MD5FileHasher {
public:
    MD5FileHasher() = delete;
    /// \brief Возвращает хэш файла
    static std::optional<std::string> fromFile(const std::string& filePath);
};

#endif // SRC_SCANNER_MD5FILEHASHER_H_
