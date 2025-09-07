#include "TemporaryFile.h"

#include <fstream>
#include <filesystem>

TemporaryFile::TemporaryFile(const std::string &path, const std::string &content)
    : TemporaryPath(path) {
    std::ofstream ofs(getPath());
    ofs << content;
}
bool TemporaryFile::blockRead() {
    if (m_specialModeOn)
        return false;
#if defined(_WIN32) || defined(_WIN64)
    std::string path = getPath();
    HANDLE hFile = CreateFileW(
        std::wstring(path.begin(), path .end()).c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );
    setHandle(hFile);
    if (hFile == INVALID_HANDLE_VALUE) {
        closeHandle();
        return false;
    }
    m_specialModeOn = true;
    return true;
#else
    if (TemporaryPath::blockRead()) {
        m_specialModeOn = true;
        return true;
    }
    return false;

#endif
}
bool TemporaryFile::blockWrite() {
    if (m_specialModeOn)
        return false;
#if defined(_WIN32) || defined(_WIN64)
    std::string path = getPath();
    HANDLE hFile = CreateFileW(
        std::wstring(path.begin(), path .end()).c_str(),
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );
    setHandle(hFile);
    if (hFile == INVALID_HANDLE_VALUE) {
        closeHandle();
        return false;
    }
    m_specialModeOn = true;
    return true;
#else
    try {
        std::filesystem::permissions(getPath(),
                                     std::filesystem::perms::owner_write |
                                         std::filesystem::perms::owner_write |
                                         std::filesystem::perms::owner_write,
                                     std::filesystem::perm_options::remove);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
#endif
    m_specialModeOn = true;
    return true;
}
void TemporaryFile::rewriteContent(const std::string &content) {
    std::ofstream ofs(getPath());
    ofs << content;
}
