#ifndef TEMPORARY_FILE_H
#define TEMPORARY_FILE_H

#include "TemporaryPath.h"

class TemporaryFile : public TemporaryPath {
public:
    TemporaryFile(const std::string &path, const std::string &content = "smth");
    bool makeInaccessible() override;
    void rewriteContent(const std::string &content);
};

#endif // TEMPORARY_FILE_H
