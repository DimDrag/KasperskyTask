#ifndef MD5FILEHASHER_H
#define MD5FILEHASHER_H

#include <optional>
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

#endif // MD5FILEHASHER_H
