#ifndef TEMPORARY_DIRECTORY_H
#define TEMPORARY_DIRECTORY_H

#include "TemporaryPath.h"

///
/// \brief Создаёт временную директорию.
/// По выходу из области видимости удаляет директорию
///
class TemporaryDirectory : public TemporaryPath {
public:
    TemporaryDirectory(const std::string &path);
    /// \brief Блокировать чтение директории
    bool blockRead() override;
};

#endif // TEMPORARY_DIRECTORY_H
