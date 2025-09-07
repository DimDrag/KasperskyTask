#ifndef TEMPORARY_DIRECTORY_H
#define TEMPORARY_DIRECTORY_H

#include "TemporaryPath.h"

class TemporaryDirectory : public TemporaryPath {
public:
    TemporaryDirectory(const std::string &path);
    bool blockRead() override;
};

#endif // TEMPORARY_DIRECTORY_H
