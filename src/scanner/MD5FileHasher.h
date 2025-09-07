#ifndef MD5FILEHASHER_H
#define MD5FILEHASHER_H

#include <fstream>
#include "md5.h"
#include <optional>

class MD5FileHasher {
public:
    MD5FileHasher() = delete;
    static std::optional<std::string> fromFile(const std::string& filePath);
};

#endif // MD5FILEHASHER_H
