#include "MD5FileHasher.h"

std::optional<std::string> MD5FileHasher::fromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open() || !file)
        return std::nullopt;

    MD5 md5;
    char buffer[4096];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        md5.add(reinterpret_cast<const void*>(buffer), file.gcount());
    }
    return md5.getHash();
}
