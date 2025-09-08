#include "TemporaryDirectory.h"

#include <filesystem>

TemporaryDirectory::TemporaryDirectory(const std::string &path)
    : TemporaryPath(path) {
    std::filesystem::create_directory(path);
}

bool TemporaryDirectory::blockRead() {
#if defined(_WIN32) || defined(_WIN64)
    std::string path = getPath();
    HANDLE hDir = CreateFileW(
        std::wstring(path.begin(), path .end()).c_str(),
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr);
    setHandle(hDir);
    if (hDir == INVALID_HANDLE_VALUE) {
        closeHandle();
        return false;
    }
    return true;
#else
    return TemporaryPath::blockRead();
#endif
}
