#include "TemporaryPath.h"

#include <filesystem>

TemporaryPath::TemporaryPath(const std::string &path) : m_path(path) {}
TemporaryPath::~TemporaryPath() {
#if defined(_WIN32) || defined(_WIN64)
    if (m_handleOpened) {
        CloseHandle(m_hPath);
    }
#endif
    std::filesystem::remove(m_path);
}
std::string TemporaryPath::getPath() {
    return m_path;
}
bool TemporaryPath::blockRead() {
#if defined(_WIN32) || defined(_WIN64)
    return false;
#else
    try {
        std::filesystem::permissions(m_path,
                                     std::filesystem::perms::owner_read |
                                         std::filesystem::perms::group_read |
                                         std::filesystem::perms::others_read,
                                     std::filesystem::perm_options::remove);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
#endif
    return true;
}
#if defined(_WIN32) || defined(_WIN64)
void TemporaryPath::setHandle(HANDLE handle) {
    closeHandle();
    m_hPath = handle;
    m_handleOpened = true;
}
void TemporaryPath::closeHandle() {
    if (m_handleOpened) {
        CloseHandle(m_hPath);
        m_handleOpened = false;
    }
}
#endif
