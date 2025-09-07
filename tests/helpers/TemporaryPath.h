#ifndef TEMPORARY_PATH_H
#define TEMPORARY_PATH_H

#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

class TemporaryPath {
public:
    TemporaryPath(const std::string &path);
    TemporaryPath(const TemporaryPath &other) = delete;
    TemporaryPath& operator=(const TemporaryPath& other) = delete;
    virtual ~TemporaryPath();
    std::string getPath();
    virtual bool makeInaccessible();
#if defined(_WIN32) || defined(_WIN64)
    void setHandle(HANDLE handle);
    void closeHandle();
#endif
private:
    std::string m_path;
#if defined(_WIN32) || defined(_WIN64)
    HANDLE m_hPath{};
    bool m_handleOpened = false;
#endif
};

#endif // TEMPORARY_PATH_H
