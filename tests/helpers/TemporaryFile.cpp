#include "TemporaryFile.h"

#include <fstream>

TemporaryFile::TemporaryFile(const std::string &path, const std::string &content)
    : TemporaryPath(path) {
    std::ofstream ofs(getPath());
    ofs << content;
}
bool TemporaryFile::makeInaccessible() {
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
    return true;
#else
    return TemporaryPath::makeInaccessible();
#endif
}
void TemporaryFile::rewriteContent(const std::string &content) {
    std::ofstream ofs(getPath());
    ofs << content;
}
