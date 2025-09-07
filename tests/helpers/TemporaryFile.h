#ifndef TEMPORARY_FILE_H
#define TEMPORARY_FILE_H

#include "TemporaryPath.h"

class TemporaryFile : public TemporaryPath {
public:
    TemporaryFile(const std::string &path, const std::string &content = "smth");
    bool blockRead() override;
    bool blockWrite();
    void rewriteContent(const std::string &content);

private:
    bool m_specialModeOn = false; //!< специальный режим установлен для этого файла
};

#endif // TEMPORARY_FILE_H
